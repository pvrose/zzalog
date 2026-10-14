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
#include "mmsstv_handler.h"

#include "import_data.h"
#include "qso_data.h"
#include "qso_manager.h"
#include "record.h"
#include "zc_status.h"
#include "cty_data.h"

#include <string>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>

#include <FL/Fl.H>

//! This class handles the interface to the MMSSTV application.
//! It snoops the MMSSTV log file and updates the ZZALOG log file with any new QSOs.

//! Band enumeration - these are the bands supported by MMSSTV.
enum band_t : uint8_t {
	B_NULL,
	B_19,
	B_35,
	B_38,
	B_7,
	B_10,
	B_14,
	B_18,
	B_21,
	B_24,
	B_28,
	B_50,
	B_144,
	B_430,
	B_1200,
	B_2400,
	B_5600,
	B_101G,
	B_104G,
	B_24G,
	B_47G,
	B_75G,
	B_142G,
	B_248G,
	B_4630,
	B_220,
	B_SAT,
};

enum mode_t : uint8_t {
	M_NULL,
	M_CW,
	M_SSB,
	M_AM,
	M_FM,
	M_RTTY,
	M_PAC,
	M_FAX,
	M_SSTV,
	M_ATV,
	M_TV,
	M_FSTV,
	M_A1,
	M_A2,
	M_A3,
	M_A3A,
	M_A3H,
	M_A3J,
	M_A4,
	M_A5,
	M_A5C,
	M_A5J,
	M_A9,
	M_A9C,
	M_F1,
	M_F2,
	M_F3,
	M_F4,
	M_F5,
	M_F9,
	M_P0,
	M_P1,
	M_P2D,
	M_P2E,
	M_P2F,
	M_P3D,
	M_P3E,
	M_P3F,
	M_P9,
	M_U1,
	M_U2,
	M_U3,
	M_U4,
	M_PSK,
	M_BPSK,
	M_QPSK,
	M_HELL,
	M_MFSK
};

extern zc_status* status_;
extern cty_data* cty_data_;
extern qso_manager* qso_manager_;

mmsstv_handler* mmsstv_handler_ = nullptr;

//! Constructor
mmsstv_handler::mmsstv_handler() :
	log_file_(""),
	last_record_num_(0),
	has_data_(false)
{
}

//! Destructor
mmsstv_handler::~mmsstv_handler()
{
}

const char EXPECTED_HEADER[sizeof(mmlog_header_t::id)] = "MMLOG DATA Ver1.00\x1A";

//! Start the handler - opens the log file and reads the header.
//! If the log file does not exists it returns false.
//! If the log file exists it reads the header and checks it's a valid MMLOG log file,
//! if not it closes the file and returns false.
//! If the log file is valid it starts a timer to snoop the log file every 5 seconds closes it and returns true.
//! \param log_file The path to the log file to be opened and read.
//! \param last_record The last record number that was read from the log file.
bool mmsstv_handler::start(const std::string& log_file, int last_record)
{
	log_file_ = log_file;
	last_record_num_ = last_record;
	std::ifstream isss(log_file_, std::ios::binary);

	if (!isss.is_open()) {
		status_->misc_status(ST_ERROR, "MMSSTV: Log file not found: %s", log_file_.c_str());
		return false;
	}

	// Read the header and check it's a valid MMLOG log file
	char header[MML_HEADER_SIZE];
	isss.read(header, MML_HEADER_SIZE);

	if (strncmp(header, EXPECTED_HEADER, sizeof(EXPECTED_HEADER) - 1) != 0) {
		status_->misc_status(ST_ERROR, "MMSSTV: Invalid log file header: %s", log_file_.c_str());
		isss.close();
		return false;
	}

	isss.close();
	// Start a timer to snoop the log file every 5 seconds
	Fl::add_timeout(5.0, timer_cb, this);
	return true;
}

// Stop the handler - stops the timer.
bool mmsstv_handler::stop()
{
	Fl::remove_timeout(timer_cb, this);
	has_data_ = false;
	return true;
}

// Timer callback - called every 5 seconds to snoop the log file for new records.
void mmsstv_handler::timer_cb(void* self)
{
	mmsstv_handler* handler = static_cast<mmsstv_handler*>(self);
	handler->snoop();
	// Re-arm the timer
	Fl::repeat_timeout(5.0, timer_cb, self);
}

// Snoop the log file - reads any new records and updates the ZZALOG log file.
// If the log file is not open it returns false.
void mmsstv_handler::snoop()
{
	std::ifstream isss(log_file_, std::ios::binary);
	if (!isss.is_open()) {
		status_->misc_status(ST_WARNING, "MMSSTV: Log file not available: %s", log_file_.c_str());
		return;
	}
	// Read the header and check it's a valid MMLOG log file
	char header[MML_HEADER_SIZE];
	isss.read(header, MML_HEADER_SIZE);
	if (strncmp(header, EXPECTED_HEADER, sizeof(EXPECTED_HEADER) - 1) != 0) {
		status_->misc_status(ST_ERROR, "MMSSTV: Invalid log file header: %s", log_file_.c_str());
		isss.close();
		return;
	}
	mmlog_header_t* mmlog_header = reinterpret_cast<mmlog_header_t*>(header);
	uint32_t num_records = mmlog_header->num_records;
	if (num_records <= last_record_num_) {
		// No new records
		isss.close();
		return;
	}
	has_data_ = true;
	status_->misc_status(ST_NOTE, "MMSSTV: Found %d new records in log file", num_records - last_record_num_);
	for (uint32_t i = last_record_num_; i < num_records; ++i) {
		mmlog_record_t record_data;
		isss.seekg(MML_HEADER_SIZE + i * sizeof(mmlog_record_t), std::ios::beg);
		isss.read(reinterpret_cast<char*>(&record_data), sizeof(mmlog_record_t));
		record* rec = qso_manager_->start_modem_qso(std::string(record_data.call), qso_data::QSO_COPY_MMSSTV);
		if (read_record(std::string(reinterpret_cast<char*>(&record_data), sizeof(mmlog_record_t)), rec)) {
			qso_manager_->update_modem_qso(true);
			status_->misc_status(ST_OK, "MMSSTV: Imported record for CALL: %s", rec->item("CALL").c_str());
		}
		else {
			qso_manager_->cancel_modem_qso();
			status_->misc_status(ST_WARNING, "MMSSTV: Failed to read record %d", i + 1);
		}
	}
	last_record_num_ = num_records;
	isss.close();
}

// Read a record from the log file - reads a record from the log file and converts it to a ZZALOG record.
// \param data The data of the record to be read.	
// \param rec The ZZALOG record to be filled with the data from the MMSSTV record.
// \return true if the record was read successfully, false if an error occurred.
bool mmsstv_handler::read_record(const std::string& data, record* rec)
{
	if (data.size() != sizeof(mmlog_record_t)) {
		return false;
	}
	const mmlog_record_t* mmlog_record = reinterpret_cast<const mmlog_record_t*>(data.data());
	// Convert the MMSSTV record to a ZZALOG record
	rec->item("CALL", std::string(mmlog_record->call));
	rec->item("RST_SENT", std::string(mmlog_record->rst_sent));
	rec->item("RST_RCVD", std::string(mmlog_record->rst_recv));
	uint16_t freq_offset = (static_cast<uint8_t>(mmlog_record->freq_offset[1]) << 8) | static_cast<uint8_t>(mmlog_record->freq_offset[0]);
	rec->item("FREQ", convert_freq(mmlog_record->band, freq_offset));
	rec->item("MODE", convert_mode(mmlog_record->mode));
	rec->item("TX_PWR", std::string(mmlog_record->power));
	rec->item("NAME", std::string(mmlog_record->name));
	rec->item("QTH", std::string(mmlog_record->qth));
	rec->item("NOTES", std::string(mmlog_record->remarks));
	rec->item("DXCC", std::to_string(cty_data_->dxcc_id_from_nickname(std::string(mmlog_record->option1))));
	rec->item("CONT", std::string(mmlog_record->option2));
	std::string date_on;
	std::string time_on;
	uint16_t start_time_intervals = (static_cast<uint8_t>(mmlog_record->start_time[1]) << 8) | static_cast<uint8_t>(mmlog_record->start_time[0]);
	uint16_t end_time_intervals = (static_cast<uint8_t>(mmlog_record->end_time[1]) << 8) | static_cast<uint8_t>(mmlog_record->end_time[0]);
	uint16_t day_of_year = (static_cast<uint8_t>(mmlog_record->day[1]) << 8) | static_cast<uint8_t>(mmlog_record->day[0]);
	convert_datetime(mmlog_record->year, day_of_year, start_time_intervals, date_on, time_on);
	rec->item("QSO_DATE", date_on);
	rec->item("TIME_ON", time_on);
	std::string date_off;
	std::string time_off;
	convert_datetime(mmlog_record->year, day_of_year, end_time_intervals, date_off, time_off);
	if (date_on != date_off) {
		rec->item("QSO_DATE_OFF", date_off);
	}
	rec->item("TIME_OFF", time_off);
	return true;
}

std::map<band_t, double> band_freq_map = {
	{B_19, 1.9},
	{B_35, 3.0},
	{B_38, 3.0},
	{B_7, 7.0},
	{B_10, 10.0},
	{B_14, 14.0},
	{B_18, 18.0},
	{B_21, 21.0},
	{B_24, 24.0},
	{B_28, 28.0},
	{B_50, 50.0},
	{B_144, 144.0},
	{B_430, 430.0},
	{B_1200, 1200.0},
	{B_2400, 2400.0},
	{B_5600, 5600.0},
	{B_101G, 10100.0},
	{B_104G, 10400.0},
	{B_24G, 24000.0},
	{B_47G, 47000.0},
	{B_75G, 75000.0}, 
	{B_142G, 142000.0},
	{B_248G, 248000.0},
	{B_4630, 463000.0},
	{B_220, 220.0}
};

// Convert a MMSSTV band and frequency offset to an ADIF frequency.
// \param band The MMSSTV band to be converted.
// \param freq_offset The frequency offset (in kHz from band edge) to be converted.
// \return The ADIF frequency corresponding to the MMSSTV band and frequency offset.
std::string mmsstv_handler::convert_freq(band_t band, uint16_t freq_offset)
{
	// Convert MMSSTV band and frequency offset to ADIF frequency
	double freq_mhz = 0.0;
	if (band_freq_map.find(band) != band_freq_map.end()) {
		freq_mhz = band_freq_map[band] + (freq_offset / 1000.0);
	}
	else {
		status_->misc_status(ST_WARNING, "MMSSTV: Unknown band: %d", band);
	}
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%.3f", freq_mhz);
	return std::string(buffer);
}

// Conversion map for MMSSTV modes to ADIF modes
std::map<mode_t, std::string> mode_map = {
	{M_CW, "CW"},
	{M_SSB, "SSB"},
	{M_AM, "AM"},
	{M_FM, "FM"},
	{M_RTTY, "RTTY"},
	{M_PAC, "PAC"},
	{M_FAX, "FAX"},
	{M_SSTV, "SSTV"},
	{M_ATV, "ATV"},
	{M_TV, "ATV"},
	{M_FSTV, "ATV"},
	{M_A1, "CW"},
	{M_A2, "CW"},
	{M_A3, "AM"},
	{M_A3H, "SSB"},
	{M_A3J, "SSB"},
	{M_F3, "FM"},
	// Add more mappings as needed
};

// Convert a MMSSTV mode to an ADIF mode.
// \param mode The MMSSTV mode to be converted.
std::string mmsstv_handler::convert_mode(mode_t mode)
{
	if (mode_map.find(mode) != mode_map.end()) {
		return mode_map[mode];
	}
	else {
		status_->misc_status(ST_WARNING, "MMSSTV: Unknown mode: %d", mode);
		return "UNKNOWN";
	}
}

// Convert MMSSTV date/time (in JST: UTC+9) to ADIF date/time (in UTC).
// \param year The year to be converted.
// \param day_of_year The day of the year to be converted.
// \param time_intervals The start time intervals to be converted.
// \param adif_date Returns the ADIF date corresponding to the MMSSTV date.
// \param adif_time Returns the ADIF time corresponding to the MMSSTV time.
bool mmsstv_handler::convert_datetime(int year, int day_of_year, int start_time_intervals, std::string& adif_date, std::string& adif_time)
{
	// Convert MMSSTV date/time (in JST: UTC+9) to ADIF date/time (in UTC)
	// MMSSTV uses a 2-digit year, so we need to convert it to a 4-digit year
	int full_year = (year < 50) ? (2000 + year) : (1900 + year);
	// day_of_year in form month*100 + day, so we need to extract the month and day
	int month = day_of_year / 100;
	int day = day_of_year % 100;
	// start_time_intervals is in 2-second intervals, so we need to convert it to hours, minutes, and seconds
	int total_seconds = start_time_intervals * 2;
	int hours = total_seconds / 3600;
	int minutes = (total_seconds % 3600) / 60;
	int seconds = total_seconds % 60;
	// Convert day of year to month and day
	std::tm timeinfo = {};
	timeinfo.tm_year = full_year - 1900;
	timeinfo.tm_mday = day;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_hour = hours;
	timeinfo.tm_min = minutes;
	timeinfo.tm_sec = seconds;
	std::time_t t = std::mktime(&timeinfo);
	std::tm* local_time = std::localtime(&t);
	// Adjust for JST to UTC (-9 hours)
	local_time->tm_hour -= 9;
	std::mktime(local_time); // Normalize the time structure
	char date_buffer[9];
	char time_buffer[7];
	snprintf(date_buffer, sizeof(date_buffer), "%04d%02d%02d", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday);
	snprintf(time_buffer, sizeof(time_buffer), "%02d%02d%02d", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	adif_date = std::string(date_buffer);
	adif_time = std::string(time_buffer);
	return true;
}