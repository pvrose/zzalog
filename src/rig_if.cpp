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
#include "rig_if.h"

#include "main.h"
#include "zc_status.h"
#include "zc_utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>
#include <thread>

#include <FL/Fl.H>

#include <hamlib/rig.h>

// Convert s-meter reading into display format
std::string rig_if::get_smeter(bool max) {
	char text[100];
	int value = max ? (int)rig_data_.s_value : (int)rig_data_.s_meter;
	// SMeter value is relative to S9
	// Smeter value 0 indicates S9,
	// 1 S-point is 6 dB
	if (value < -54) {
		// Below S0
		snprintf(text, 100, " S0%ddB", 54 + value);
	}
	else if (value <= 0) {
		// Below S9 - convert to S points (6dB per S-point
		snprintf(text, 100, " S%1d", (54 + value) / 6);
	}
	else {
		// S9+
		snprintf(text, 100, " S9+%ddB", value);
	}

	return text;
}

// Return the transmit or receive frequency
double rig_if::get_dfrequency(bool tx) {
	// Read the frequency (in MHz)
	double frequency;
	if (tx) {
		frequency = rig_data_.tx_frequency / 1000000.0;
	}
	else {
		frequency = rig_data_.rx_frequency / 1000000.0;
	}
	switch (hamlib_data_->freq_mode) {
	case VFO:
		break;
	case XTAL:
		frequency = hamlib_data_->frequency;
		break;
	default:
		frequency = 0.0;
		break;
	}
	if (hamlib_data_->accessory & TRANSVERTER) {
		frequency += hamlib_data_->freq_offset;
	}
	return frequency;
}

std::string rig_if::get_frequency(bool tx) {
	double frequency = get_dfrequency(tx);
	char text[15];
	snprintf(text, 15, "%0.6f", frequency);
	return text;
}

// Return the power
double rig_if::get_dpower(bool max) {
	double value = max ? (double)rig_data_.pwr_value : (double)rig_data_.pwr_meter;
	switch (hamlib_data_->power_mode) {
	case RF_METER:
		break;
	case DRIVE_LEVEL:
		// rig_data_.drive is a %age.
		value = rig_data_.drive * hamlib_data_->max_power * 0.01;
		break;
	case MAX_POWER:
		value = hamlib_data_->max_power;
		break;
	default:
		value = 0.0;
	}
	if (hamlib_data_->accessory & TRANSVERTER) {
		value = hamlib_data_->tvtr_power;
	}
	if (hamlib_data_->accessory & AMPLIFIER) {
		double gain = pow(10.0, ((double)hamlib_data_->gain) / 10.0);
		value *= gain;
	}
	return value;
}

// Convert power to string
std::string rig_if::get_tx_power(bool max) {
	char text[100];
	snprintf(text, 100, "%g", get_dpower(max));
	return text;
}

// Converts rig mode to ADIF mode/submode
void rig_if::get_string_mode(std::string& mode, std::string& submode) {
	rig_mode_t rig_mode = rig_data_.mode;
	mode = "";
	submode = "";
	switch (rig_mode) {
	case GM_INVALID:
		mode = "???";
		return;
	case GM_DIGL:
		mode = "DATA L";
		return;
	case GM_DIGU:
		mode = "DATA U";
		return;
	case GM_LSB:
		mode = "SSB";
		submode = "LSB";
		return;
	case GM_USB:
		mode = "SSB";
		submode = "USB";
		return;
	case GM_CWU:
	case GM_CWL:
		mode = "CW";
		return;
	case GM_FM:
		mode = "FM";
		return;
	case GM_AM:
		mode = "AM";
		return;
	case GM_DSTAR:
		mode = "DIGITALVOICE";
		submode = "DSTAR";
		return;
	default:
		return;
	}
}

// Get current PTT state
bool rig_if::get_ptt() {
	return rig_data_.ptt;
}

// Get current Split value
bool rig_if::get_split() {
	return rig_data_.split;
}

// Constructor
rig_if::rig_if(const char* name, hamlib_data_t* data) 
{
	// Default action
	rig_ = nullptr;
	error_code_ = 0;
	my_rig_name_ = name;
	hamlib_data_ = data;
	run_read_ = false;
	// Set initial state
	if (hamlib_data_ && hamlib_data_->port_type != RIG_PORT_NONE) {
		state_.store(DISCONNECTED);
	} else {
		state_.store(NOT_CONNECTABLE);
	}
	// Last PTT off
	last_ptt_off_ = std::chrono::system_clock::now();
	thread_ = nullptr;
	// Access flags
	toc_split_ = 0;
	has_drive_ = true;
	has_smeter_ = true;
	if (hamlib_data_ && hamlib_data_->power_mode == RF_METER) {
		has_rf_meter_ = true;
	}
	else {
		has_rf_meter_ = false;
	}
	count_down_ = 0;
	sum_smeters_ = 0;
	read_item_ = "";
	
	// If the name has been set, there is a rig
	if (hamlib_data_ && my_rig_name_.length()) {
		if (hamlib_data_->port_type != RIG_PORT_NONE) {
			open();
		}
		else {
			char msg[128];
			snprintf(msg, 128, "RIG: Not opening %s - no CAT available", my_rig_name_.c_str());
			status_->misc_status(ST_WARNING, msg);
		}
	}
}

// Destructor
rig_if::~rig_if()
{
	// close the connection
	close();
}

// Clsoe the connection
void rig_if::close() {


	if (rig_ != nullptr) {
		if (state_.load() == CONNECTED_OK || state_.load() == CONNECTED_SLOW || state_.load() == CONNECTED_ERROR) {
		// If we have a connection and it's open, close it and tidy memory used by hamlib
		// Delete the thread that reads the required rig values
			char msg[128];
			snprintf(msg, 128, "RIG: Closing connection %s (%s/%s on port %s)",
				my_rig_name_.c_str(),
				hamlib_data_->mfr.c_str(),
				hamlib_data_->model.c_str(),
				hamlib_data_->port_name.c_str());
			status_->misc_status(ST_NOTE, msg);

			run_read_ = false;
			if (thread_) {
				thread_->join();
				delete thread_;
				thread_ = nullptr;
			}
			rig_close(rig_);
			rig_cleanup(rig_);
			rig_ = nullptr;
		}
	}
	state_.store(DISCONNECTED);
}

// Opens the connection associated with the rig
bool rig_if::open() {
	char msg[256];
	if (hamlib_data_->port_type == RIG_PORT_NONE) {
		snprintf(msg, 256, "RIG: Connection %s/%s No port to connect",
			hamlib_data_->mfr.c_str(),
			hamlib_data_->model.c_str()
		);
		status_->misc_status(ST_WARNING, msg);
 	} 
	else if (hamlib_data_->port_name.length() == 0) {
		snprintf(msg, 256, "RIG: Connection %s/%s No port supplied - open failed",
			hamlib_data_->mfr.c_str(),
			hamlib_data_->model.c_str()
		);
		status_->misc_status(ST_WARNING, msg);
		return false;

	}
	if (DEBUG_THREADS) printf("RIG MAIN: Starting rig %s/%s port %s access thread\n",
		hamlib_data_->mfr.c_str(), hamlib_data_->model.c_str(), hamlib_data_->port_name.c_str());
	if (rig_ == nullptr) close();
	state_.store(CONNECTING, std::memory_order_seq_cst);
	thread_ = new std::thread(th_sopen_rig, this);
	std::chrono::system_clock::time_point wait_start = std::chrono::system_clock::now();
	int timeout = 40000;
	snprintf(msg, sizeof(msg), "RIG: Connecting %s/%s on port %s - timeout = %d ms",
		hamlib_data_->mfr.c_str(),
		hamlib_data_->model.c_str(),
		hamlib_data_->port_name.c_str(),
		timeout);
	status_->misc_status(ST_NOTE, msg);
	while(state_.load() == CONNECTING) {
		// Allow FLTK scheduler in
		Fl::check();
		if (std::chrono::system_clock::now() - wait_start > std::chrono::milliseconds(timeout)) {
			char msg[128];
			snprintf(msg, sizeof(msg), "RIG: Connecting %s/%s abandoned after %d ms", 
				hamlib_data_->mfr.c_str(),
				hamlib_data_->model.c_str(),
				timeout);
			status_->misc_status(ST_WARNING, msg);
		}
	}
	if (DEBUG_THREADS) printf("RIG MAIN: Finished opening rig (state = %s)\n", STATE_MAP.at(state_.load()).c_str());
	thread_->join();
	delete thread_;
	thread_ = nullptr;

		
	if (connected()) {

		if (hamlib_data_->port_type == RIG_PORT_SERIAL) {
			snprintf(msg, 256, "RIG: Connection %s/%s on port %s opened OK",
				hamlib_data_->mfr.c_str(),
				hamlib_data_->model.c_str(),
				hamlib_data_->port_name.c_str());
		}
		else {
			snprintf(msg, 256, "RIG: Connection %s/%s on port %s (%s) opened OK",
				hamlib_data_->mfr.c_str(),
				hamlib_data_->model.c_str(),
				hamlib_data_->port_name.c_str(),
				rig_get_info(rig_));
		}
		status_->misc_status(ST_OK, msg);

		if (DEBUG_THREADS) printf("RIG MAIN: Starting reading rig data\n");
		thread_ = new std::thread(th_run_rig, this);

		return true;
	}
	else {
		snprintf(msg, 256, "open(): %s/%s port %s",
			hamlib_data_->mfr.c_str(),
			hamlib_data_->model.c_str(),
			hamlib_data_->port_name.c_str()
		);
		status_->misc_status(ST_WARNING, error_message(msg).c_str());
		close();
		return false;
	}
}

// This is within the thread
void rig_if::th_sopen_rig(rig_if* that) {
	if (DEBUG_THREADS) printf("RIG THREAD: Opening rig\n");
	that->th_open_rig(that);
	if (DEBUG_THREADS) printf("RIG THREAD: Opened (or not) rig\n");
}

void rig_if::th_open_rig(rig_if* that) {
	// Get the rig interface
	if (DEBUG_RIGS) printf("RIGS: Initialising rig %s/%s\n", hamlib_data_->mfr.c_str(), hamlib_data_->model.c_str());
	rig_ = rig_init(hamlib_data_->model_id);
	if (rig_ != nullptr) {
		switch (hamlib_data_->port_type) {
		case RIG_PORT_SERIAL:
			// Successful - set up the serial port parameters
			strcpy(rig_->state.rigport.pathname, hamlib_data_->port_name.c_str());
			rig_->state.rigport.parm.serial.rate = hamlib_data_->baud_rate;
			rig_->state.timeout = (int)(hamlib_data_->timeout * 1000.);
			break;
	// open r
		case RIG_PORT_NETWORK:
		case RIG_PORT_USB:
			rig_set_conf(rig_, rig_token_lookup(rig_, "rig_pathname"), hamlib_data_->port_name.c_str());
			rig_->state.timeout = (int)(hamlib_data_->timeout * 1000.);
			break;
		default:
			break;
		}
	} 
	// open rig connection over serial port
	if (DEBUG_RIGS) printf("RIGS: Opening rig %s/%s\n", hamlib_data_->mfr.c_str(), hamlib_data_->model.c_str());
	error_code_ = abs(rig_open(rig_));
	switch(error_code_) {
	case RIG_OK:
		state_.store(CONNECTED_OK);
		break;
	case RIG_EIO:
		// IO error - including open failed
		// Not opened, tidy hamlib memory usage and mark it so.
		// rig_cleanup(rig_);
		// rig_ = nullptr;
		state_.store(DISCONNECTED);
		Fl::awake(cb_rig_error, that);
		break;
	default:
		// Other errors probably indicate connected by something is wrong
		state_.store(CONNECTED_ERROR);
		Fl::awake(cb_rig_warning, that);
		break;
	}
}

// Return rig name
std::string& rig_if::rig_name() {
	// Read capabilities to get manufacturer and model name
	full_rig_name_ = rig_get_info(rig_);
	return full_rig_name_;
}

// Thraed method
void rig_if::th_run_rig(rig_if* that) {
	if (DEBUG_THREADS) printf("RIG THREAD: Rig access thread started\n");
	// run_read_ will be cleared when the rig closes or errors.
	that->th_read_values();
	if (that->state_.load() == CONNECTED_OK || that->state_.load() == CONNECTED_SLOW) {
		if (DEBUG_THREADS) printf("RIG THREAD: Reading from rig\n");
		that->run_read_ = true;
		while (that->run_read_ && 
			(that->state_.load() == CONNECTED_OK || that->state_.load() == CONNECTED_SLOW)
		) {
			// Read the values from the rig once per second
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			that->th_read_values();
		}
		if (that->state_.load() != CONNECTED_OK) {
			Fl::awake(cb_rig_error, that);
		}
	} else {
		Fl::awake(cb_rig_error, that);
	}
}

// Read the data from the rig
void rig_if::th_read_values() {
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	if (hamlib_data_->port_type == RIG_PORT_NONE) {
		return;
	}
	// Assume powerd-on unless otherwise discover it
	// WFView (accessed using rigctl_d doesn't have powered state)
	if (hamlib_data_->model_id != 2) {
		read_item_ = "powerstat";
		// Check powered on
		powerstat_t power_state;
		if (DEBUG_RIGS) printf("RIGS: Reading power status\n");
		error_code_ = rig_get_powerstat(rig_, &power_state);
		if (error_handler(error_code_, "Power status", nullptr, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		switch (power_state) {
		case RIG_POWER_ON:
		{
			if (DEBUG_RIGS) printf("RIGS: Power status - ON\n");
			break;
		}
		case RIG_POWER_STANDBY:
		{
			if (DEBUG_RIGS) printf("RIGS: Power status - STANDBY\n");
			break;
		}
		case RIG_POWER_OFF:
		{
			if (DEBUG_RIGS) printf("RIGS: Power status - OFF\n");
			state_.store(UNPOWERED);
			return;
		}
		default:
			if (DEBUG_RIGS) printf("RIGS: Power status - Unknown\n");
			break;
		}
	}
	// Split
	bool previous_split = rig_data_.split;
	if (toc_split_ < hamlib_data_->max_to_count) {
		read_item_ = "split";
		vfo_t TxVFO;
		split_t split;
		if (DEBUG_RIGS) printf("RIGS: Reading Split mode\n");
		error_code_ = rig_get_split_vfo(rig_, RIG_VFO_CURR, &split, &TxVFO);
		if (DEBUG_RIGS) printf("RIGS: Read Split - %d (TX VFO = %d)\n", (int)split, (int)TxVFO);
		if (error_handler(error_code_, "Split mode", nullptr, &toc_split_)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		rig_data_.split = split == split_t::RIG_SPLIT_ON;
	}
	// PTT value
	read_item_ = "PTT";
	ptt_t ptt;
	bool current_ptt = rig_data_.ptt;
	if (DEBUG_RIGS) printf("RIGS: Reading PTT\n");
	error_code_ = rig_get_ptt(rig_, RIG_VFO_CURR, &ptt);
	if (DEBUG_RIGS) printf("RIGS: Read PTT - %d\n", (int)ptt);
	if (error_handler(error_code_, "PTT", nullptr, nullptr)) {
		state_.store(CONNECTED_ERROR);
		return;
	}
	rig_data_.ptt = (ptt != ptt_t::RIG_PTT_OFF);
	// If we are releasing PTT record the time
	if (current_ptt && (ptt != ptt_t::RIG_PTT_OFF)) {
		last_ptt_off_ = std::chrono::system_clock::now();
	}
	// Read frequencies
	double d_temp;
	if (rig_data_.split) {
		// Read TX and RX frequencies per split VFO_TX and VFO_CURR
		read_item_ = "TX Frequency";
		if (DEBUG_RIGS) printf("RIGS: Reading TX Frequency\n");
		error_code_ = rig_get_freq(rig_, RIG_VFO_TX, &d_temp);
		if (DEBUG_RIGS) printf("RIGS: Read TX Frequency - %g Hz\n", d_temp);
		if (error_handler(error_code_, "TX Frequency", nullptr, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		rig_data_.tx_frequency = d_temp;
		// Read RX frequency
		read_item_ = "RX Frequency";
		if (DEBUG_RIGS) printf("RIGS: Reading RX Frequency\n");
		error_code_ = rig_get_freq(rig_, RIG_VFO_CURR, &d_temp);
		if (DEBUG_RIGS) printf("RIGS: Read RX Frequency - %g Hz\n", d_temp);
		if (error_handler(error_code_, "RX Frequency", nullptr, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		rig_data_.rx_frequency = d_temp;
	} else {
		// Read current VFO - this is mostly a work-round for repeater duplex operation
		// flrig does not support repeater shift and offset yet. 
		read_item_ = "Current Frequency";
		if (DEBUG_RIGS) printf("RIGS: Reading current Frequency\n");
		error_code_ = rig_get_freq(rig_, RIG_VFO_CURR, &d_temp);
		if (DEBUG_RIGS) printf("RIGS: Read current Frequency - %g Hz\n", d_temp);
		if (error_handler(error_code_, "Current Frequency", nullptr, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		// Set RX frequency to current while RX and TX frequency while TX.
		double previous_freq = rig_data_.rx_frequency;
		if (!rig_data_.ptt) {
			rig_data_.rx_frequency = d_temp;
			// If we have QSY'd then set TX frequency to RX frequency
			if (previous_freq != d_temp || previous_split) rig_data_.tx_frequency = d_temp;
		} else {
			rig_data_.tx_frequency = d_temp;
		}
	}
	// Read mode
	read_item_ = "mode";
	rmode_t mode;
	shortfreq_t bandwidth;
	if (DEBUG_RIGS) printf("RIGS: Reading Mode\n");
	error_code_ = rig_get_mode(rig_, RIG_VFO_CURR, &mode, &bandwidth);
	if (error_handler(error_code_, "Mode/Bandwidth", nullptr, nullptr)) {
		state_.store(CONNECTED_ERROR);
		return;
	}
	// Convert hamlib mode encoding to ZLG encoding
	rig_data_.mode = GM_INVALID;
	if (mode & RIG_MODE_AM) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - AM\n");
		rig_data_.mode = GM_AM;
	}
	if (mode & RIG_MODE_CW) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - CW\n");
		rig_data_.mode = GM_CWU;
	}
	if (mode & RIG_MODE_CWR) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - CW (reveresed)\n");
		rig_data_.mode = GM_CWL;
	}
	if (mode & RIG_MODE_LSB) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - LSB\n");
		rig_data_.mode = GM_LSB;
	}
	if (mode & RIG_MODE_USB) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - USB\n");
		rig_data_.mode = GM_USB;
	}
	if (mode & RIG_MODE_FM) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - FM\n");
		rig_data_.mode = GM_FM;
	}
	if (mode & (RIG_MODE_RTTY | RIG_MODE_PKTLSB)) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - Data LSB\n");
		rig_data_.mode = GM_DIGL;
	}
	if (mode & (RIG_MODE_RTTYR | RIG_MODE_PKTUSB)) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - Data USB\n");
		rig_data_.mode = GM_DIGU;
	}
	if (mode & RIG_MODE_DSTAR) {
		if (DEBUG_RIGS) printf("RIGS: Read Mode - DStar\n");
		rig_data_.mode = GM_DSTAR;
	}
	if (has_drive_) {
		read_item_ = "TX Drive";
		// Read drive level
		value_t drive_level;
		if (DEBUG_RIGS) printf("RIGS: Reading TX Drive\n");
		error_code_ = rig_get_level(rig_, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &drive_level);
		if (DEBUG_RIGS) printf("RIGS: Read TX Drive - %g\n", drive_level.f);
		if (error_handler(error_code_, "Drive level", &has_drive_, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		rig_data_.drive = drive_level.f * 100;
	}
	value_t meter_value;
	if (has_smeter_) {
		read_item_ = "S-meter";
		// S-meter - set to max value during RX and last RX value during TX
		if (DEBUG_RIGS) printf("RIGS: Reading S-meter\n");
		error_code_ = rig_get_level(rig_, RIG_VFO_CURR, RIG_LEVEL_STRENGTH, &meter_value);
		if (DEBUG_RIGS) printf("RIGS: Read S-meter - %d\n", meter_value.i);
		if (error_handler(error_code_, "S-meter", &has_smeter_, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		// TX->RX (or changed RX frequency - use read value
		if (current_ptt && !rig_data_.ptt) {
			// empty smeter queue
			smeters_.clear();
		}
		// Push value into smeters stack
		if (!rig_data_.ptt) smeters_.push_back(meter_value.i);
		while (smeters_.size() > hamlib_data_->num_smeters) {
			smeters_.erase(smeters_.begin());
		}
		// Get the max value in the stack
		int max_smeter = -100;
		for (auto it = smeters_.begin(); it != smeters_.end(); it++) {
			max_smeter = std::max<int>(max_smeter, *it);
		}
		rig_data_.s_value = max_smeter;
		rig_data_.s_meter = meter_value.i;
	}
	if (has_rf_meter_) {
		read_item_ = "RF meter";
		// Power meter
		if (DEBUG_RIGS) printf("RIGS: Reading RF meter\n");
		error_code_ = rig_get_level(rig_, RIG_VFO_CURR, RIG_LEVEL_RFPOWER_METER_WATTS, &meter_value);
		if (DEBUG_RIGS) printf("RIGS: Read RF meter - %g\n", meter_value.f);
		if (error_handler(error_code_, "RF Power", &has_rf_meter_, nullptr)) {
			state_.store(CONNECTED_ERROR);
			return;
		}
		// RX->TX - reset the power level unless...
		if (!current_ptt && rig_data_.ptt) {
			// ...PTT is released for only a few seconds (e.g. CW break-in or SSB VOX)
			std::chrono::seconds gap = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_ptt_off_);
			if (gap > std::chrono::seconds(5)) {
				rig_data_.pwr_value = meter_value.f;
			}
		}
		// TX use accumulated maximum value
		else if (rig_data_.ptt) {
			if ((double)meter_value.f > rig_data_.pwr_value) {
				rig_data_.pwr_value = (double)meter_value.f;
			}
		}
		// Also store iommediate value
		rig_data_.pwr_meter = (double)meter_value.f;
	}
	read_item_ = "";
	// All successful
	count_down_ = FAST_RIG_TIMER;
	// Check rig response time and inform use if it's slowed down or back to normal
	char msg[128];
	std::chrono::system_clock::time_point finish = std::chrono::system_clock::now();
	std::chrono::milliseconds response = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	if (state_.load() == CONNECTED_SLOW) {
		if (response < std::chrono::milliseconds(100)) {
			snprintf(msg, sizeof(msg), "RIG %s Responding normally %d ms", 
				my_rig_name_.c_str(), (int)response.count());
			warning_message_ = msg;
			state_.store(CONNECTED_OK);
			Fl::awake(cb_rig_warning, this);
		} 
	} else if (response > std::chrono::milliseconds((int)(hamlib_data_->timeout * 1000.0))) {
			snprintf(msg, sizeof(msg), "RIG %s Responding slowly %d ms", 
				my_rig_name_.c_str(), (int)response.count());
			warning_message_ = msg;
			state_.store(CONNECTED_SLOW);
			Fl::awake(cb_rig_warning, this);
	}

	return;
}

// Handle hamlib responses - 
bool rig_if::error_handler(int code, const char* meter, bool* flag, int* to_count) {
	char msg[128];
	bool ok = false;
	switch (abs(code)) {
	case RIG_OK:
		return false;
	case RIG_ENAVAIL:
		if (flag) {
			snprintf(msg, sizeof(msg), "RIG: Access is %s is not available - turn off future access", meter);
			Fl::awake(cb_rig_warning, msg);
			*flag = false;
			ok = false;
		}
		else {
			ok = true;
		}
		break;
	case RIG_ETIMEOUT:
		if (to_count) {
			(*to_count)++;
			if (*to_count < hamlib_data_->max_to_count) {
				snprintf(msg, sizeof(msg), "RIG: Access to %s timed out - continuing", meter);
				Fl::awake(cb_rig_warning, msg);
			}
			else {
				snprintf(msg, sizeof(msg), "RIG: Access to %s has timed out %d times, no further access", meter, *to_count);
				Fl::awake(cb_rig_warning, msg);
			}
			ok = false;
		}
		break;
	default:
		ok =  true;
		break;
	}
	return ok;
}

void rig_if::cb_rig_error(void* v) {
	rig_if* that = (rig_if*)v;
	printf("DEBUG: Rig error in %p\n", that);
	char msg[128];
	snprintf(msg, sizeof(msg), "RIG: Error response %s", that->error_message(that->read_item_.c_str()).c_str());
	status_->misc_status(ST_ERROR, msg);
	that->close();
}

void rig_if::cb_rig_warning(void* v) {
	rig_if* that = (rig_if*)v;
	char msg[128];
	snprintf(msg, sizeof(msg), "RIG: Warning response %s", that->warning_message_.c_str());
	status_->misc_status(ST_WARNING, msg);
}

// Return the most recent error message
std::string rig_if::error_message(const char* func_name) {
	char msg[256];
	snprintf(msg, 256, "RIG: Hamlib error \"%s\" processing %s",
		error_text((rig_errcode_e)abs(error_code_)),
		func_name);
	return std::string(msg);
}

// Return the text for the error code.
const char* rig_if::error_text(rig_errcode_e code) {
	switch (code) {
	case RIG_OK:
		return "No error, operation completed successfully";
	case RIG_EINVAL:
		return "invalid parameter";
	case RIG_ECONF:
		return "invalid configuration (serial,..)";
	case RIG_ENOMEM:
		return "memory shortage";
	case RIG_ENIMPL:
		return "function not implemented, but will be";
	case RIG_ETIMEOUT:
		return "communication timed out";
	case RIG_EIO:
		return "IO error, including open failed";
	case RIG_EINTERNAL:
		return "Internal Hamlib error, huh!";
	case RIG_EPROTO:
		return "Protocol error";
	case RIG_ERJCTED:
		return "Command rejected by the rig";
	case RIG_ETRUNC:
		return "Command performed, but arg truncated";
	case RIG_ENAVAIL:
		return "function not available";
	case RIG_ENTARGET:
		return "VFO not targetable";
	case RIG_BUSERROR:
		return "Error talking on the bus";
	case RIG_BUSBUSY:
		return "Collision on the bus";
	case RIG_EARG:
		return "NULL RIG handle or any invalid pointer parameter in get arg";
	case RIG_EVFO:
		return "Invalid VFO";
	case RIG_EDOM:
		return "Argument out of domain of func";
	default:
		return "Error not defined";
	}
};

// Set frequency
bool rig_if::set_frequency(double f) {
	error_code_ = rig_set_freq(rig_, RIG_VFO_A, f * 1000000.0);
	if (error_handler(error_code_, "Set frequency", nullptr, nullptr)) {
		return false;
	} else {
		return true;
	}
}

// Get rig state
rig_if::rig_state_t rig_if::state() {
	return state_.load();
}

// Connected
bool rig_if::connected() {
	switch(state_.load()) {
	case CONNECTED_OK:
	case CONNECTED_ERROR:
	case CONNECTED_SLOW:
		return true;
	default:
		return false;
	}
}