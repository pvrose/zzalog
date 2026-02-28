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
#include "club_handler.h"

#include "book.h"
#include "adi_writer.h"
#include "fields.h"
#include "keyring.h"
#include "main.h"
#include "qsl_dataset.h"
#include "qso_manager.h"
#include "record.h"
#include "url_handler.h"

#include "zc_file_holder.h"
#include "zc_status.h"
#include <zc_utils.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

// Constructor 
club_handler::club_handler() {
	// Create the URL handler if it hasn't already been done
	if (!url_handler_) url_handler_ = new url_handler;
	// Initialise thread interface
	run_threads_ = true;
	upload_response_ = 0;
	key_ = keyring_->key("Clublog");
	if (DEBUG_THREADS) printf("CLUBLOG MAIN: Starting std::thread\n");
	th_upload_ = new std::thread(thread_run, this);

	set_adif_fields();

}

// Destructor
club_handler::~club_handler() {
	run_threads_ = false;
	th_upload_->join();
	delete th_upload_;
}

// Upload the saved log to ClubLog using putlogs.php interface
bool club_handler::upload_log(book* book) {
	if (book->size()) {
		status_->misc_status(ST_NOTE, "CLUBLOG: Uploading log to clublog.org");
		// Get the book data and write it to the stream
		std::stringstream ss;
		adi_writer* writer = new adi_writer;
		std::set<std::string> adif_fields;
		writer->store_book(book, ss, false, &adif_fields_);
		// Get back to start of stream
		ss.seekg(ss.beg);
		// Get the parameters and make available for the HTTP POST FORM
		std::vector<url_handler::field_pair> fields;
		generate_form(fields, nullptr);
		std::stringstream resp;
		// Post the form
		bool ok;
		if (!url_handler_->post_form("https://clublog.org/putlogs.php", fields, &ss, &resp)) {
			// Display error message received from post
			char* message = new char[resp.str().length() + 30];
			sprintf(message, "CLUBLOG: Upload failed - %s", resp.str().c_str());
			status_->misc_status(ST_ERROR, message);
			ok = false;
		}
		else {
			// Update all records sent with the fact that they have been uploaded and when
			status_->misc_status(ST_OK, "CLUBLOG: Upload successful");
			book_->enable_save(false, "Updating Clublog status");
			ok = true;
			std::string today = zc::now(false, "%Y%m%d");
			for (auto it = book->begin(); it != book->end(); it++) {
				(*it)->item("CLUBLOG_QSO_UPLOAD_DATE", today);
				(*it)->item("CLUBLOG_QSO_UPLOAD_STATUS", std::string("Y"));
			}
			// Go back to last entry in book.
			book_->selection(book_->size() - 1, HT_SELECTED);
			book_->enable_save(true, "Updated Clublog status");
		}
		return ok;
	}
	else {
		status_->misc_status(ST_WARNING, "CLUBLOG: No data to upload!");
		return false;
	}
}

// Generate the fields in the form
void club_handler::generate_form(std::vector<url_handler::field_pair>& fields, record* the_qso) {
	// Read the settings that define user's access 
	server_data_t* club_data = qsl_dataset_->get_server_data("Club");
	std::string email = club_data->user;
	fields.push_back({"email", email, "", ""});
	std::string password = club_data->password;
	fields.push_back({ "password", password, "", "" });
	if (the_qso != nullptr) {
		// get logging callsign from QSO record
		std::string callsign = qso_manager_->get_default(qso_manager::CALLSIGN);
		fields.push_back({ "callsign", callsign.c_str(), "", "" });
		// Get string ADIF
		fields.push_back({ "adif", single_qso_, "", "" });
	}
	else {
		// Get callsign from settings
		std::string callsign = qso_manager_->get_default(qso_manager::CALLSIGN);
		fields.push_back({ "callsign", callsign.c_str(), "", ""});
		// Set file to empty string to use the supplied data stream
		fields.push_back({ "file", "", "clublog.adi", "application/octet-stream" });
	}
	// Hard-coded API Key for this application
	fields.push_back({ "api", key_, "", "" });
}

// Download the exception file
bool club_handler::download_exception(std::string filename) {
	// Start downloading exception file
	status_->misc_status(ST_NOTE, "CLUBLOG: Downloading exception file");
	std::string zip_filename = filename + ".gz";
	std::ofstream os(zip_filename, std::ios::trunc | std::ios::out | std::ios::binary);
	std::string url = "https://cdn.clublog.org/cty.php?api=" + key_;
	if (url_handler_->read_url(url, &os)) {
		os.close();
		status_->misc_status(ST_OK, "CLUBLOG: Downloaded OK");
		return unzip_exception(zip_filename);
	} else {
		os.close();
		status_->misc_status(ST_ERROR, "CLUBLOG: Exception file download failed.");
		return false;
	}
}

// Unzip the downloaded  exceptions file
bool club_handler::unzip_exception(std::string filename) {
	// Read the settings that define user's access 
	std::string ref_dir = file_holder_->get_directory(DATA_WORKING);
	char cmd[256];
#ifdef _WIN32
	snprintf(cmd, sizeof(cmd), "\"%s\" e %s -o%s -y", "C:/Program Files/7-Zip/7z", filename.c_str(), ref_dir.c_str());
#else
	snprintf(cmd, sizeof(cmd), "gunzip -f %s", filename.c_str());
#endif

	char msg[128];
	snprintf(msg, sizeof(msg), "CLUBLOG: Unzipping exception file: %s", cmd);
	status_->misc_status(ST_NOTE, msg);
	int result = system(cmd);
#ifdef _WIN32
	if (result < 0) {
		status_->misc_status(ST_ERROR, "CLUBLOG: Unzipping failed - check if 7z is available");
		return false;
	}
	// This assumes 7z is the executble
	switch (result) {
	case 0:
		status_->misc_status(ST_OK, "CLUBLOG: Unzipping OK");
		return true;
	case 1:
		status_->misc_status(ST_WARNING, "CLUBLOG: Unzipping incurred a warning");
		return true;
	case 2:
		status_->misc_status(ST_ERROR, "CLUBLOG: Unzipping failed - fatal error");
		return false;
	case 7:
		status_->misc_status(ST_ERROR, "CLUBLOG: Unzipping failed - command-line error");
		return false;
	case 8:
		status_->misc_status(ST_ERROR, "CLUBLOG: Unzipping failed - insufficient memory");
		return false;
	case 255:
		status_->misc_status(ST_ERROR, "CLUBLOG: Unzipping failed - stopped by user");
		return false;
	}
	return true;
#else
	if (result != 0) {
		status_->misc_status(ST_ERROR, "CLUBLOG: Unzipping failed");
		return false;
	} else {
		status_->misc_status(ST_OK, "CLUBLOG: Unzipping OK");
		return true;
	}
#endif
}

// Upload the single specified QSO in real time
bool club_handler::upload_single_qso(qso_num_t record_num) {
	server_data_t* club_data = qsl_dataset_->get_server_data("Club");
	bool upload_qso = club_data->upload_per_qso;
	if (upload_qso == false) {
		status_->misc_status(ST_WARNING, "CLUBLOG: Uploading per QSO is disabled.");
	}
	record* this_record = book_->get_record(record_num, false);
	if (this_record->item("CLUBLOG_QSO_UPLOAD_STATUS") == "Y") {
		upload_qso = false;
	}
	else if (this_record->item("CLUBLOG_QSO_UPLOAD_STATUS") == "N") {
		upload_qso = false;
	}
	if (upload_qso) {
		// Suspend saving
		book_->enable_save(false, "Uploading to Clublog");
		record* this_record = book_->get_record(record_num, false);
		if (DEBUG_THREADS) printf("CLUBLOG MAIN: Queueing request %s\n", this_record->item("CALL").c_str());
		upload_lock_.lock();
		upload_queue_.push(this_record);
		upload_done_queue_.push(this_record);
		upload_lock_.unlock();
	}
	return false;
}

// Upload QSO to clublog (in thread)
void club_handler::th_upload(record* this_record) {
	std::set<std::string> adif_fields;
	single_qso_ = to_adif(this_record, adif_fields_);
	// Get the parameters and make available for the HTTP POST FORM
	std::vector<url_handler::field_pair> fields;
	generate_form(fields, this_record);
	std::stringstream resp;
	// Post the form
	bool ok;
	if (!url_handler_->post_form("https://clublog.org/realtime.php", fields, nullptr, (std::ostream*)&resp)) {
		ok = false;
		upload_error_ = resp.str();
	}
	else {
		ok = true;
		upload_error_ = "";

	}
	upload_response_ = ok;
	if (DEBUG_THREADS) printf("CLUBLOG THREAD: Calling std::thread callback result = %d(%s)\n",
	    ok, upload_error_.c_str());
	Fl::awake(cb_upload_done, (void*)this);
	std::this_thread::yield();
}

// Method in the main thread that is 
// called when the thread handling upload has finished
// Updates the QSO recotrd that the update has been completed
bool club_handler::upload_done(bool response) {
	char message[200];
	record* this_record = upload_done_queue_.front();
	if (response == false) {
		// Display error message received from post
		snprintf(message, 200, "CLUBLOG: %s %s %s QSL upload failed",
			this_record->item("QSO_DATE").c_str(),
			this_record->item("TIME_ON").c_str(),
			this_record->item("CALL").c_str());
		status_->misc_status(ST_ERROR, message);
		if (fl_choice("Upload failed - Do you want to allow it to try again or be ignored?", "Try later", "Ignore", nullptr) == 1) {
			this_record->item("CLUBLOG_QSO_UPLOAD_STATUS", std::string("N"));
			book_->enable_save(true, "Not uploaded to Clublog");
		}
	}
	else {
		// Update all records sent with the fact that they have been uploaded and when
		snprintf(message, 200, "CLUBLOG: %s %s %s QSL uploaded",
			this_record->item("QSO_DATE").c_str(),
			this_record->item("TIME_ON").c_str(),
			this_record->item("CALL").c_str());
		status_->misc_status(ST_OK, message);
		std::string today = zc::now(false, "%Y%m%d");
		this_record->item("CLUBLOG_QSO_UPLOAD_DATE", today);
		this_record->item("CLUBLOG_QSO_UPLOAD_STATUS", std::string("Y"));
		book_->enable_save(true, "Uploaded to Clublog");
	}
	upload_done_queue_.pop();
	return response;
}

// Static interface between upload thread and main thread to handle upload complete
void club_handler::cb_upload_done(void* v) {
	if (DEBUG_THREADS) printf("CLUBLOG MAIN: Entered std::thread callback handler\n");
	club_handler* that = (club_handler*)v;
	that->upload_done(that->upload_response_);
}

// Start and progress the thread that handles uploads without stalling main thread
void club_handler::thread_run(club_handler* that) {
	if (DEBUG_THREADS) printf("CLUBLOG THREAD: Thread started\n");
	while (that->run_threads_) {
		// Wait until qso placed on interface
		while (that->run_threads_ && that->upload_queue_.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		// Process it
		that->upload_lock_.lock();
		if (!that->upload_queue_.empty()) {
			record* qso = that->upload_queue_.front();
			that->upload_queue_.pop();
			if (DEBUG_THREADS) printf("CLUBLOG THREAD: Received request %s\n", qso->item("CALL").c_str());
			that->upload_lock_.unlock();
			that->th_upload(qso);
		}
		else {
			that->upload_lock_.unlock();
		}
		std::this_thread::yield();
	}
}

// Convert this record to ADIF: uses adi_writer
std::string club_handler::to_adif(record* this_record, field_list &fields) {
	std::string result = "";
	for (auto it = fields.begin(); it != fields.end(); it++) {
		result += adi_writer::item_to_adif(this_record, *it) + " ";
	}
	result += " <EOR>";
	return result;
}

// Specify the fields requested by Clublog.org
void club_handler::set_adif_fields() {
	// Ser default values if necessary
	(void)fields_->collection("Upload/ClubLog", CLUBLOG_FIELDS);
	// Now copy to the set 
	adif_fields_ = fields_->field_names("Upload/ClubLog");
}

// Download the OQRS list of QSL requests
bool club_handler::download_oqrs(std::stringstream* adif) {
	status_->misc_status(ST_NOTE, "CLUBLOG: Downloading OQRS");
	std::stringstream request;
	generate_oqrs(request);
	if (!url_handler_->post_url("https://clublog.org/getadif.php", "", &request, adif)) {
		status_->misc_status(ST_ERROR, "CLUBLOG: Download OQRS failed");
		return false;
	}
	return true;
}

// Generate the HTTP POST reqyest to download OQRS
void club_handler::generate_oqrs(std::ostream& request) {
	// Read the settings that define user's access 
	server_data_t* club_data = qsl_dataset_->get_server_data("Club");
	std::string email = club_data->user;
	request << "email=" << email << "&";
	std::string password = club_data->password;
	request << "password=" << password << "&";
	std::string my_call = qso_manager_->get_default(qso_manager::CALLSIGN);
	request << "call=" << my_call << "&type=dxqsl";
}

