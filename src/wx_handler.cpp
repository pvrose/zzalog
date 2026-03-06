/*
	Copyright 2025-2026, Philip Rose, GM3ZZA
	
    This file is part of ZZALOG. Amateur Radio Logging Software.

    ZZALOG is free software: you can redistribute it and/or modify it under the
	terms of the Lesser GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

    ZZALOG is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
	PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with ZZALOG. 
	If not, see <https://www.gnu.org/licenses/>. 

*/
#include "wx_handler.h"

#include "cty_data.h"
#include "keyring.h"
#include "qso_manager.h"
#include "record.h"
#include "main.h"   
#include "stn_data.h"
#include "url_handler.h"

#include "zc_status.h"
#include "zc_ticker.h"
#include "zc_utils.h"

#include "nlohmann/json.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_PNG_Image.H>

using json = nlohmann::json;

const double MPH2MPS = 1.0 / 3600.0 * (1760.0 * 36.0) * 25.4 / 1000.0;

const double LONG_DELAY = 30. * 60. * 10.;
const double SHORT_DELAY = 3. * 60. * 10.;

std::string wx_handler::wind_cardinal(int dirn) {
    int temp = dirn * 32 / 360;
    temp += 1;
    temp %= 32;
    temp /= 2;
    std::string cardinals[16] =
    { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
      "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };
    return cardinals[temp];
}

std::string wx_handler::beaufort(float speed) {
    if (speed <= 0.2) return "Calm";
    if (speed <= 1.5) return "Light air";
    if (speed <= 3.3) return "Light breeze";
    if (speed <= 5.4) return "Gentle breeze";
    if (speed <= 7.9) return "Moderate breeze";
    if (speed <= 10.7) return "Fresh breeze";
    if (speed <= 13.8) return "Strong breeze";
    if (speed <= 17.1) return "Moderate gale";
    if (speed <= 20.7) return "Gale";
    if (speed <= 24.4) return "Severe gale";
    if (speed <= 28.4) return "Storm";
    if (speed <= 32.6) return "Violent storm";
    return "Hurricane";
}

// Deserialise from JSON to wx_report
static void from_json(const json& j, wx_report& s) {
    json jcoord = j.at("coord");
    jcoord.at("lon").get_to(s.city_location.longitude);
    jcoord.at("lat").get_to(s.city_location.latitude);
    auto weather = j.at("weather").get<std::vector<json>>();
    json jweather = weather[0];
    jweather.at("description").get_to(s.description);
    std::string icon;
    jweather.at("icon").get_to(icon);
    s.icon = wx_handler::fetch_icon(icon);
    json jmain = j.at("main");
    jmain.at("temp").get_to(s.temperature_K);
    jmain.at("feels_like").get_to(s.subjective_K);
    jmain.at("pressure").get_to(s.pressure_hPa);
    jmain.at("humidity").get_to(s.humidity_pc);
    j.at("visibility").get_to(s.visibility_m);
    json jwind = j.at("wind");
    jwind.at("speed").get_to(s.wind_speed_ms);
    jwind.at("deg").get_to(s.wind_dirn);
    json jcloud = j.at("clouds");
    jcloud.at("all").get_to(s.cloud_cover);
    j.at("dt").get_to(s.updated);
    json jsys = j.at("sys");
    jsys.at("country").get_to(s.iso_country);
    jsys.at("sunrise").get_to(s.sunrise);
    jsys.at("sunset").get_to(s.sunset);
    int tz_sec;
    j.at("timezone").get_to(tz_sec);
    s.timezone_hr = tz_sec / 3600.0F;
    j.at("id").get_to(s.city_id);
    j.at("name").get_to(s.city_name);
    s.wind_cardinal = wx_handler::wind_cardinal(s.wind_dirn);
    s.wind_name = wx_handler::beaufort(s.wind_speed_ms);
}

// Constructor
wx_handler::wx_handler() :
    wx_thread_(nullptr),
    wx_valid_(false)
{
	enable_fetch_.store(true);
	do_fetch_.store(false);
	report_.icon = nullptr;
	key_ = keyring_->key("Weather");
    // Start thread
    wx_thread_ = new std::thread(do_thread, this);
    // Start ticker - 30 minutes
    ticker_->add_ticker(this, cb_ticker, DEBUG_QUICK ? SHORT_DELAY : LONG_DELAY);


};

// Destructor   
wx_handler::~wx_handler() {
    ticker_->remove_ticker(this);
	enable_fetch_.store(false);
    // Close the thread down cleanly
    if (wx_thread_) wx_thread_->join();
};

// Do thread - when told to by wx_fetch_ fetch the WX data.
// Abandon when run_thread_ is deasserted
void wx_handler::do_thread(wx_handler* that) {
	while (that->enable_fetch_.load()) {
		while (!that->do_fetch_.load() && that->enable_fetch_.load()) std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (DEBUG_THREADS) printf("WX THREAD: staring to fetch\n");
		if (!that->update()) {
			Fl::awake(cb_fetch_error, (void*)that);
		}
		else {
			Fl::awake(cb_fetch_done, (void*)that);
		}
		if (DEBUG_THREADS) printf("WX THREAD: fetching complete\n");
		that->do_fetch_.store(false);
	}
}

// Update weather report - forecd
bool wx_handler::update() {
    // Create a dummy record to get own location
    record* dummy = qso_manager_->dummy_qso();
    std::string qth_id = qso_manager_->get_default(qso_manager::QTH);
    zc::lat_long_t location = { nan(""), nan("") };
	int dxcc_id;
	char url[1024];
	if (qth_id.length()) {
        const qth_info_t* info = stn_data_->get_qth(qth_id);
        if (info != nullptr && info->data.find(LOCATOR) != info->data.end()) {
            dummy->item("MY_GRIDSQUARE", info->data.at(LOCATOR));
        }
		if (info != nullptr && info->data.find(DXCC_ID) != info->data.end()) {
			dummy->item("MY_DXCC", info->data.at(DXCC_ID));
			dxcc_id = std::stoi(info->data.at(DXCC_ID));
		}
    }
    location = dummy->location(true);
    if (location.is_nan()) {
		const qth_info_t* info = stn_data_->get_qth(qth_id);
		std::string city = info->data.at(CITY);
		std::string cc = cty_data_->iso_cc(qso_manager_->get_default(qso_manager::CALLSIGN));
		snprintf(url, sizeof(url), "http://api.openweathermap.org/geo/1.0/direct?q=%s,,%s&limit=5&appid=%s&mode=json",
			city.c_str(),
			cc.c_str(),
			key_.c_str()
		);
		std::stringstream ss_city;
		if (url_handler_->read_url(std::string(url), &ss_city)) {
			ss_city.seekg(std::ios::beg);
			try {
				json j;
				ss_city >> j;
				auto jall = j.get<std::vector<json>>();
				for (auto& jcity : jall) {
					std::string scity;
					jcity["name"].get_to(scity);
					if (scity == city) {
						jcity["lat"].get_to(location.latitude);
						jcity["lon"].get_to(location.longitude);
						break;
					}
				}
			}
			catch (const json::exception& e) {
				printf("WX THREAD: Failed to decode geofetch: %d (%s)\n",
					e.id, e.what());

			}
		}
		else {
			printf("WX THREAD: FAiled to fetch geocoding for %s,%s", qth_id.c_str(), cc.c_str());
		}
		if (location.is_nan()) {
			report_ = wx_report();
			report_.city_name = "Not known";
			return false;
		}
    }
    snprintf(url, sizeof(url), "https://api.openweathermap.org/data/2.5/weather?lat=%f&lon=%f&appid=%s&mode=json",
        location.latitude,
        location.longitude,
        key_.c_str());
	std::stringstream ss;
    if (url_handler_->read_url(std::string(url), &ss)) {
        ss.seekg(std::ios::beg);
        try {
            json j;
            ss >> j;
            j.get_to(report_);
        }
		catch (const json::exception& e) {
			printf("WX THREAD: Failed to decode WX: %d (%s)\n",
			    e.id, e.what());
            
		}
	}
	else {
		return false;
	}
	return true;
}

// Timer - called every 30 minutes
void wx_handler::ticker() {
	status_->misc_status(ST_NOTE, "WX_HANDLER: Downloading weather data");
	if (DEBUG_THREADS) printf("WX MAIN: Starting WX fetch\n");
	// Momentarily allow thread to fetch WX report.
	do_fetch_.store(true);
	std::this_thread::yield();
}

// Static
void wx_handler::cb_ticker(void* v) {
    ((wx_handler*)v)->ticker();
}

// Static call back: WX fetch complete
void wx_handler::cb_fetch_done(void* v) {
    wx_handler* that = (wx_handler*)v;
    status_->misc_status(ST_OK, "WX_HANDLER: Downloaded OK: %s %0.0f\302\260C %0.0fMPH %s %0.0f hPa. %0.0f%% cloud",
        that->description().c_str(), 
        that->temperature(), 
        that->wind_speed(), 
        that->wind_direction().c_str(), 
        that->pressure(), 
        that->cloud() * 100);
	that->wx_valid_ = true;
    qso_manager_->enable_widgets();
}

// Static call back: WX fetch complete
void wx_handler::cb_fetch_error(void* v) {
	wx_handler* that = (wx_handler*)v;
	status_->misc_status(ST_ERROR, "WX_HANDLER: Error downloading weather report");
	that->wx_valid_ = false;
    qso_manager_->enable_widgets();
}

// Get the various weather items - 
// summation icon
Fl_Image* wx_handler::icon() const {
    return report_.icon;
}

// Description
std::string wx_handler::description() const {
    return report_.description;
}

// Temperature (C)
float wx_handler::temperature() const {
    return report_.temperature_K - 273.15;
}

// Wind-speed (MPH)
float wx_handler::wind_speed() const {
    return report_.wind_speed_ms / MPH2MPS;
}

// Wind-speed name
std::string wx_handler::wind_name() const {
    return report_.wind_name;
}

// Wind direction (16th cardinals)
std::string wx_handler::wind_direction() const {
    if (report_.wind_cardinal == "") return "---";
    else return report_.wind_cardinal;
}

// Wind direction (degrees)
unsigned int wx_handler::wind_degrees() const {
    if (report_.wind_cardinal == "") return -1;
    else return report_.wind_dirn;
}

// Cloud cover
float wx_handler::cloud() const {
    return ((float)report_.cloud_cover)/ 100.0;
}

// Cloud description
std::string wx_handler::cloud_name() const {
    return report_.cloud_name;
}

// Sunrise
time_t wx_handler::sun_rise() const {
    return report_.sunrise;
}

// Sunset
time_t wx_handler::sun_set() const { 
    return report_.sunset;
}

// Last updated
time_t wx_handler::last_updated() const {
    return report_.updated;
}

// Location
std::string wx_handler::location() const {
    return report_.city_name;
}

// Latlong location
std::string wx_handler::latlong() const {
    std::string result = zc::degrees_to_dms(report_.city_location.latitude, true);
    result += " ";
    result += zc::degrees_to_dms(report_.city_location.longitude, false);
    return result;
}

// Pressure
float wx_handler::pressure() const {
    return report_.pressure_hPa;
}

// Fetch icon
Fl_Image* wx_handler::fetch_icon(std::string name) {
    char url[1024];
    snprintf(url, sizeof(url), "https://openweathermap.org/img/wn/%s.png", name.c_str());
    std::stringstream ss;
    if (url_handler_->read_url(std::string(url), &ss)) {
        ss.seekg(std::ios::beg);
        Fl_PNG_Image* result = new Fl_PNG_Image(nullptr, (unsigned char*)ss.str().c_str(), ss.str().length());
        return result;
    } else {
        Fl::awake(cb_fetch_error, (void *)"WX_HANDLER: WX icon read failed");
        return nullptr;
    }
}

// Return the location of the city in the supplied QSO
zc::lat_long_t wx_handler::get_city_location(const record* qso) const {
	std::string city = qso->item("QTH");
	if (city.length() == 0) {
		return { nan(""), nan("") };
	}
	zc::lat_long_t location = { nan(""), nan("") };
	char url[256];
	std::string call = qso->item("CALL");
	std::string cc = cty_data_->iso_cc(call);
	snprintf(url, sizeof(url), "http://api.openweathermap.org/geo/1.0/direct?q=%s,,%s&limit=5&appid=%s&mode=json",
		city.c_str(),
		cc.c_str(),
		key_.c_str()
	);
	std::stringstream ss_city;
	if (url_handler_->read_url(std::string(url), &ss_city)) {
		ss_city.seekg(std::ios::beg);
		try {
			json j;
			ss_city >> j;
			auto jall = j.get<std::vector<json>>();
			for (auto& jcity : jall) {
				std::string scity;
				jcity["name"].get_to(scity);
				if (scity == city) {
					jcity["lat"].get_to(location.latitude);
					jcity["lon"].get_to(location.longitude);
					break;
				}
			}
		}
		catch (const json::exception& e) {
			printf("WX THREAD: Failed to decode geofetch: %d (%s)\n",
				e.id, e.what());

		}
	}
	return location;
}

bool wx_handler::wx_valid() {
	return wx_valid_;
}






