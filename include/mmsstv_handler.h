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
#pragma once

#include <cstdint>
#include <string>

//! This class handles the interface to the MMSSTV application.
//! It snoops the MMSSTV log file and updates the ZZALOG log file with any new QSOs.
//! The data structures are defined in the source files at https://github.com/n5ac/mmsstv 
//! and reproduced here under the terms of the LGPL.
//! The MMSSTV log file is a binary file. The first 256 bytes are a header, followed by a series of records each 256 bytes long.

constexpr int MML_HEADER_SIZE = 256; //!< Size of the header in bytes
//! File header structure
typedef struct {
	char   id[20];          //!< "MMLOG DATA Ver1.00\x1A\0"
	char   dummy[4];        //!< dummy bytes
	uint16_t multi;         //!< Multi information size (not used?)
	char   td;              //!< Time difference (from JST: not used?)
	char   hash;            //!< Hash value (not used? - set to 02)
	uint32_t num_records;   //!< Number of records in the file
	char   master;          //!< Master flag (not used? - set to 0)
	char   dummy2[15];      //!< dummy bytes
	char   mode[32][6];     //!< User defined modes (not used? set to null strings)
} mmlog_header_t;

//! Field lengths - these are the lengths of the fields in the record structure not including the null terminator
constexpr int MLCALL = 16;      //!< Length of callsign field
constexpr int MLRST = 20;       //!< Length of RST field
constexpr int MLREMARK = 56;    //!< Length of remark field
constexpr int MLNAME = 16;      //!< Length of name field
constexpr int MLQTH = 28;	   //!< Length of QTH field
constexpr int MLQSL = 54;      //!< Length of QSL field
constexpr int MLPOWER = 4;     //!< Length of power field
constexpr int MLOPT = 8;       //!< Length of options field

enum band_t : uint8_t;
enum mmode_t : uint8_t;

constexpr int MML_RECORD_SIZE = 256; //!< Size of the record in bytes
//! Record structure
typedef struct {
	char year;             //!< Year (last two digits < 50 = 20xx, >=50 = 19xx)
	char day[2];             //!< Day of year (month * 100 + day) - in JST
	char start_time[2];       //!< Start time (number of 2-second intervals since midnight JST)
	char end_time[2];         //!< End time (number of 2-second intervals since midnight JST)
	char call[MLCALL + 1];        //!< Callsign
	char rst_sent[MLRST + 1];    //!< RST sent
	char rst_recv[MLRST + 1];    //!< RST received
	band_t band;             //!< Band (enum band_t)
	char freq_offset[2];     //!< Frequency offset (in kHz from band edge)
	mmode_t mode;             //!< Mode (enum mmode_t)
	char power[MLPOWER + 1];      //!< Power
	char name[MLNAME + 1];      //!< Name
	char qth[MLQTH + 1];        //!< QTH
	char qsl[MLQSL + 1];      //!< QSL
	char qsl_send;        //!< QSL send flag (Y/N?)
	char qsl_recv;        //!< QSL received flag (Y/N?)
	char cq;           //!< CQ flag (CQ or call)
	char env[2];             //!< Environment (not used? - set to 0)
	char remarks[MLREMARK + 1];   //!< Remarks
	char option1[MLOPT + 1];     //!< Options (used for DXCC main prefix)
	char option2[MLOPT + 1];     //!< Options (used for continent)
} mmlog_record_t;

// ZZALOG ADIF record structure.
class record;
// ZZALOG import records (vector of record pointers) - used to update the log file with new records.
class import_data;

class mmsstv_handler {

public:

	//! Constructor
	mmsstv_handler();

	//! Destructor
	~mmsstv_handler();

	//! Start the handler - opens the log file and reads the header.
	//! If the log file does not exists it returns false.
	//! If the log file exists it reads the header and checks it's a valid MMLOG log file,
	//! if not it closes the file and returns false.
	//! If the log file is valid it starts a timer to snoop the log file every 5 seconds closes it and returns true.
	//! \param log_file The path to the log file to be opened and read.
	//! \param last_record The last record number that was read from the log file.
	bool start(const std::string& log_file, int last_record);

	//! Stop the handler - stops the timer.
	bool stop();

	//! Timer callback - called every 5 seconds to snoop the log file for new records.
	static void timer_cb(void* self);

	//! Returns true if the handler has seen data in the log file, false otherwise.
	bool has_data() const { return has_data_; }

	//! Returns the last record number that was read from the log file.
	int last_record_num() const { return last_record_num_; }

private:

	//! Snoop the log file - reads any new records and updates the ZZALOG log file.
	//! If the log file is not open it returns false.
	//! If the log file is open it reads any new records and updates the ZZALOG log file.
	void snoop();
	//! Read a record from the log file - reads a record from the log file and converts it to a ZZALOG record.
	//! \param data The data of the record to be read.
	//! \param rec The ZZALOG record to be filled with the data from the MMSSTV record.
	//! \return true if the record was read successfully, false if an error occurred.
	bool read_record(const std::string& data, record* rec);
	//! Convert a MMSSTV band and frequency offset to an ADIF frequency.
	//! \param band The MMSSTV band to be converted.
	//! \param freq_offset The frequency offset (in kHz from band edge) to be converted.
	//! \return The ADIF frequency corresponding to the MMSSTV band and frequency offset.
	std::string convert_freq(band_t band, uint16_t freq_offset);
	//! Convert a MMSSTV mode to an ADIF mode.
	//! \param mode The MMSSTV mode to be converted.
	//! \return The ADIF mode corresponding to the MMSSTV mode.
	std::string convert_mode(mmode_t mode);
	//! Convert MMSSTV date/time (in JST: UTC+9) to ADIF date/time (in UTC).
	//! \param year The year to be converted.
	//! \param day_of_year The day of the year to be converted.
	//! \param time_intervals The start time intervals to be converted.
	//! \param adif_date Returns the ADIF date corresponding to the MMSSTV date.
	//! \param adif_time Returns the ADIF time corresponding to the MMSSTV time.
	//! \return true if the conversion was successful, false otherwise.
	bool convert_datetime(int year, int day_of_year, int start_time_intervals, std::string& adif_date, std::string& adif_time);

	//! File name of the MMSSTV log file.
	std::string log_file_;

	//! The last record number that was read from the log file.
	int last_record_num_;

	//! handler has seen data in the log file.
	bool has_data_;

};
	
extern mmsstv_handler* mmsstv_handler_;