#include "zl_server.h"

#include "zlip.h"
#include "zl_record.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <thread>

//! Constructor
zl_server::zl_server(std::string filename) {
	filename_ = filename;
	// Reserve the broadcast client ID
	attached_clients_.insert(0);
	load_logbook_data(filename_);
	start_server();
}

//! Destructor
zl_server::~zl_server() {
	send_server_shutdown();
	save_logbook_data(filename_);
}

//! Start the server
//! Start 2 threads: 
//! one to send heartbeat packets, 
//! and one to update log-book data to file periodically.
void zl_server::start_server() {
	heartbeat_thread_ = new std::thread(run_heartbeat, this);
	save_thread_ = new std::thread(run_save, this);
}

//! Load log-book data from file - ignore null records.
bool zl_server::load_logbook_data(std::string filename) {
	std::ifstream in(filename.c_str());
	if (!in.is_open()) {
		return false;
	}
	logbook_data_.clear();
	json j;
	in >> j;
	for (const auto& item : j) {
		// /todo: validate and process each item
		if (!item.is_null()) {
			update_derived_fields(const_cast<json&>(item));
			logbook_data_.push_back(item);
		}
	}
	return true;
}

//! Save log-book data to file
bool zl_server::save_logbook_data(std::string filename) {
	std::ofstream out(filename.c_str());
	if (!out.is_open()) {
		return false;
	}
	logbook_mutex_.lock();
	json j = json::array();
	for (size_t i = 0; i < logbook_data_.size(); i++) {
		j.push_back(logbook_data_[i]);
		auto it = dirty_qsos_.find(i);
		if (it != dirty_qsos_.end()) {
			dirty_qsos_.erase(it);
		}
	}
	out << std::setw(4) << j << std::endl;
	logbook_mutex_.unlock();
	return true;
}


//! The method running the heartbeat thread
//! \param that The zl_server instance
//! \note Sends heartbeat packets at regular intervals.
//! \note Interval defined by HEARTBEAT_INTERVAL constant.
//! \note Runs in its own std::thread.
void zl_server::run_heartbeat(zl_server* that) {
	while (true) {
		that->send_heartbeat();
		std::this_thread::sleep_for(std::chrono::seconds(that->HEARTBEAT_INTERVAL));
	}
}

//! The method running the save thread
//! \param that The zl_server instance
//! \note Saves log-book data to file at regular intervals.
//! \note Interval defined by SAVE_INTERVAL constant.
//!		
//! \note Runs in its own std::thread.
void zl_server::run_save(zl_server* that) {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(that->SAVE_INTERVAL));
		that->save_logbook_data(that->filename_);
	}
}

//! Send heartbeat to all attached clients
void zl_server::send_heartbeat() {
	zlip heartbeat;
	heartbeat.command = ZLIP_SERVER_HEARTBEAT;
	heartbeat.client_id = 0; // broadcast
	heartbeat.request["logbook_size"] = logbook_data_.size();
	request_handler_(this, heartbeat);
}

//! Send server shutdown to all attached clients
void zl_server::send_server_shutdown() {
	zlip shutdown;
	shutdown.command = ZLIP_SERVER_SHUTDOWN;
	shutdown.client_id = 0; // broadcast
	request_handler_(this, shutdown);
}

//! Set the server request handler
//! \param handler The request handler method
//! \note The request handler is called to process
//! server requests.
void zl_server::set_request_handler(zlip_client_method_t handler) {
	request_handler_ = handler;
}

//! ZLIP request handler
void zl_server::request_handler(void* inst, zlip& info) {
	zl_server* that = static_cast<zl_server*>(inst);
	that->handle_request(info);
}

//! Handle a client request
void zl_server::handle_request(zlip& info) {
	switch (info.command) {
	case ZLIP_ATTACH_CLIENT:
		handle_attach_client(info);
		break;
	case ZLIP_DETACH_CLIENT:
		handle_detach_client(info);
		break;
	case ZLIP_GET_QSO:
		handle_get_qso(info);
		break;
	case ZLIP_DELETE_QSO:
		handle_delete_qso(info);
		break;
	case ZLIP_EXTRACT_DATA:
		handle_extract_data(info);
		break;
	case ZLIP_CLEAR_EXTRACT:
		handle_clear_extract(info);
		break;
	case ZLIP_GET_WB4_STATUS:
		handle_get_wb4_status(info);
		break;
	case ZLIP_UPDATE_QSO:
		handle_update_qso(info);
		break;
	case ZLIP_INSERT_QSO:
		handle_insert_qso(info);
		break;
	case ZLIP_RESERVE_QSO:
		handle_reserve_qso_number(info);
		break;
	//case ZLIP_CHECK_QSO:
	//	handle_check_qso(info);
	//	break;
	//case ZLIP_FIND_DUPLICATES:
	//	handle_find_duplicates(info);
	//	break;
	default:
		// Unknown command
		break;
	}
}

//! Handle a client attach request
//! \param info The zlip request information
//! Allocates the next available client ID
//! and adds it to the attached clients list.
//! Returns the client ID in the response field.
void zl_server::handle_attach_client(zlip& info) {
	// Find the next available client ID
	uint8_t client_id = 1;
	while (attached_clients_.find(client_id) != attached_clients_.end()) {
		client_id++;
	}
	attached_clients_.insert(client_id);
	info.response["client_id"] = client_id;
}

//! Handle a client detach request
//! \param info The zlip request information
//! Removes the client ID from the attached clients list.
//! Frees the client ID for future use.
void zl_server::handle_detach_client(zlip& info) {
	uint8_t client_id = info.client_id;
	attached_clients_.erase(client_id);
}

//! Handle a get QSO request
//! \param info The zlip request information
//! Returns the QSO record numbered in the qso_number field
//! from the logbook data in the response field.
//! If the QSO number is invalid, returns a null object.
void zl_server::handle_get_qso(zlip& info) {
	size_t qso_number = get_true_qso_index(info.book_id, info.qso_number);
	if (qso_number < logbook_data_.size()) {
		info.response = logbook_data_[qso_number];
	}
	else {
		info.response = nullptr;
	}
}

//! Handle a delete QSO request
//! \param info The zlip request information
//! Deletes the QSO record numbered in the qso_number field
//! from the logbook data.
void zl_server::handle_delete_qso(zlip& info) {
	size_t qso_number = get_true_qso_index(info.book_id, info.qso_number);
	if (qso_number >= logbook_data_.size()) {
		// Invalid QSO number
		return;
	}
	logbook_mutex_.lock();
	auto qso_it = logbook_data_.begin() + qso_number;
	logbook_data_.erase(qso_it);
	// Remove QSO from all extracts
	remove_qso_from_extracts(qso_number);
	//for (auto& extract_pair : logbook_extracts_) {
	//	extract_t& extract = extract_pair.second;
	//	auto index_it = extract.qso_number_to_index.find(qso_number);
	//	if (index_it != extract.qso_number_to_index.end()) {
	//		size_t index = index_it->second;
	//		extract.qso_numbers.erase(extract.qso_numbers.begin() + index);
	//		extract.qso_number_to_index.erase(index_it);
	//		// Update indices
	//		for (size_t i = index; i < extract.qso_numbers.size(); i++) {
	//			size_t qso_num = extract.qso_numbers[i];
	//			extract.qso_number_to_index[qso_num] = i;
	//		}
	//	}
	//}
	remove_qso_from_wb4(qso_number);
	logbook_mutex_.unlock();
}

//! Handle an extract data request
//! \param info The zlip request information	
//! Creates a new logbook extraction
void zl_server::handle_extract_data(zlip& info) {
	// Create new extract
	extract_t extract;
	// TODO: Apply filter from info.request
	for (size_t i = 0; i < logbook_data_.size(); i++) {
		extract.qso_numbers.push_back(i);
		extract.qso_number_to_index[i] = i;
	}
	// /TODO
	// Find next available book ID
	uint8_t book_id = 1;
	while (logbook_extracts_.find(book_id) != logbook_extracts_.end()) {
		book_id++;
	}
	logbook_extracts_[book_id] = extract;
	// Return book ID and size
	info.response["book_id"] = book_id;
	info.response["size"] = extract.qso_numbers.size();
}

//! Handle a clear extract request
//! \param info The zlip request information
//! Clears the logbook extraction identified by book_id.
//! Frees the book ID for future use.
void zl_server::handle_clear_extract(zlip& info) {
	uint8_t book_id = info.book_id;
	logbook_extracts_.erase(book_id);
}

//! Handle a get worked-before status request
//! \param info The zlip request information
//! Returns the "worked before" status for the callsign
//! in the QSO record numbered by qso_number
void zl_server::handle_get_wb4_status(zlip& info) {
	size_t qso_number = get_true_qso_index(info.book_id, info.qso_number);
	if (qso_number >= logbook_data_.size()) {
		return;
	}
	// TODO : Implement worked-before logic
}

//! Handle a save QSO request
//! \param info The zlip request information
//! Updates the QSO record numbered in the qso_number field
//! with the data in the request field.
void zl_server::handle_update_qso(zlip& info) {
	size_t qso_number = get_true_qso_index(info.book_id, info.qso_number);
	if (qso_number >= logbook_data_.size()) {
		// Invalid QSO number
		return;
	}
	// Update QSO record merging data from request
	logbook_data_[qso_number].merge_patch(info.request);
	update_derived_fields(qso_number);
	dirty_qsos_.insert(qso_number);
}

//! Handle a insert QSO request
//! \param info The zlip request information
//! Inserts a new QSO record with the data in the request field
void zl_server::handle_insert_qso(zlip& info) {
	logbook_mutex_.lock();
	json new_qso = info.request;
	// Insert in chronological order
	size_t insert_index = get_closest_qso_before(new_qso);
	logbook_data_.insert(logbook_data_.begin() + insert_index, new_qso);
	dirty_qsos_.insert(insert_index);
	update_derived_fields(insert_index);
	update_extracts_after_insertion(insert_index);
	info.response["qso_number"] = insert_index;
	logbook_mutex_.unlock();
}

//! Handle a reserve QSO number request
//! \param info The zlip request information
//! Returns the next available QSO number
void zl_server::handle_reserve_qso_number(zlip& info) {
	logbook_mutex_.lock();
	info.response["qso_number"] = logbook_data_.size();
	logbook_data_.push_back(json()); // Placeholder
	logbook_mutex_.unlock();
}

////! Handle a check QSO request
////! \param info The zlip request information
////! Checks if a QSO record with the same
////! callsign and timestamp exists in the logbook data.
//void zl_server::handle_check_qso(zlip& info) {
//	json qso_to_check = info.request.at("qso_record");
//	zl_record::match_flags_t match_flags = info.request.at("match_flags");
//	zl_record::match_result_t match_result = zl_record::MT_NOMATCH;
//	size_t datum_pos = get_closest_qso_before(qso_to_check);
//	size_t test_record = 0;
//	bool had_swl_match = false;
//	bool found_match = false;
//	if (datum_pos < logbook_data_.size()) {
//		// Start N minutes before the time 
//		const std::chrono::seconds MINUTES_30(1800);
//		std::chrono::system_clock::time_point datum = zl_record::ctimestamp(qso_to_check);
//		size_t start_pos = datum_pos;
//		std::chrono::system_clock::time_point start = zl_record::ctimestamp(logbook_data_[start_pos]);
//		while (datum - start < MINUTES_30 && start_pos > 0) {
//			start_pos--;
//			start = zl_record::ctimestamp(logbook_data_[start_pos]);
//		}
//		size_t end_pos = datum_pos;
//		datum = zl_record::ctimestamp(qso_to_check, true);
//		std::chrono::system_clock::time_point end_pt = zl_record::ctimestamp(logbook_data_[end_pos], true);
//		while (end_pt - datum < MINUTES_30 && end_pos < logbook_data_.size() - 1) {
//			end_pos++;
//			end_pt = zl_record::ctimestamp(logbook_data_[end_pos], true);
//		}
//		for (test_record = start_pos; test_record <= end_pos && !found_match; test_record++) {
//			// If the test record is outwith the book skip the check
//			if (test_record < 0 || test_record >= logbook_data_.size()) continue;
//			// Get potential match QSO
//			json& test_qso = logbook_data_[test_record];
//			// Compare QSO records - Import record should have fewer fields
//			zl_record::match_result_t match_result = zl_record::match_records(qso_to_check, test_qso);
//			switch (match_result) {
//			case zl_record::MT_EXACT:
//			case zl_record::MT_LOC_MISMATCH:
//			case zl_record::MT_OVERLAP:
//				found_match = true;
//				break;
//			}
//		}
//		// Now look for near misses.
//		for (test_record = start_pos; test_record <= end_pos && !found_match && !had_swl_match; test_record++) {
//			// If the test record is outwith the book skip the check
//			if (test_record < 0 || test_record >= logbook_data_.size()) continue;
//			// Get potential match QSO
//			json& test_qso = logbook_data_[test_record];
//			// Compare QSO records - Import record should have fewer fields
//			zl_record::match_result_t match_result = zl_record::match_records(qso_to_check, test_qso);
//			switch (match_result) {
//			case zl_record::MT_2XSWL_MATCH:
//			case zl_record::MT_SWL_MATCH:
//				had_swl_match = true;
//				break;
//			case zl_record::MT_PROBABLE:
//			case zl_record::MT_POSSIBLE:
//				found_match = true;
//				break;
//			}
//		}
//	}
//	info.response["match_result"] = match_result;
//	info.response["qso_number"] = test_record;
//}
//
////! Handle a find duplicate QSO request
////! \param info The zlip request information
////! Finds duplicate QSO records in the logbook data
//void zl_server::handle_find_duplicates(zlip& info) {
//	// TODO: Copy method from book.cpp
//	// Set response to the two duplicate QSO numbers
//}

//! Return true if QSO \p rhs is later than QSO \p lhs
bool zl_server::is_qso_later(const json& lhs, const json& rhs) {
	// Compare by QSO date and time
	std::string lhs_date = lhs.value("QSO_DATE", "");
	std::string lhs_time = lhs.value("TIME_ON", "");
	std::string rhs_date = rhs.value("QSO_DATE", "");
	std::string rhs_time = rhs.value("TIME_ON", "");
	if (lhs_date < rhs_date) {
		return false;
	}
	else if (lhs_date > rhs_date) {
		return true;
	}
	else {
		// Same date, compare time
		return lhs_time < rhs_time ? false : true;
	}
}

//! Get the true QSO index from book ID and QSO number
size_t zl_server::get_true_qso_index(uint8_t book_id, size_t qso_number) {
	if (book_id == 0) {
		// Main logbook
		return qso_number;
	}
	else {
		// Extract logbook
		auto extract_it = logbook_extracts_.find(book_id);
		if (extract_it == logbook_extracts_.end()) {
			// Extract not found - use qso_number
			return qso_number;
		}
		extract_t& extract = extract_it->second;
		if (qso_number >= extract.qso_numbers.size()) {
			// Invalid QSO number
			return logbook_data_.size(); // Invalid index
		}
		return extract.qso_numbers[qso_number];
	}
}

//! Update worked-before status and add derived fields
void zl_server::update_derived_fields(size_t qso_number) {
	json& qso = logbook_data_[qso_number];
	// TODO: Copy from book.cpp
}

//! Remove QSO from extracts
void zl_server::remove_qso_from_extracts(size_t qso_number) {
	for (auto& extract_pair : logbook_extracts_) {
		extract_t& extract = extract_pair.second;
		auto index_it = extract.qso_number_to_index.find(qso_number);
		if (index_it != extract.qso_number_to_index.end()) {
			size_t index = index_it->second;
			extract.qso_numbers.erase(extract.qso_numbers.begin() + index);
			extract.qso_number_to_index.erase(index_it);
			// Update indices
			for (size_t i = index; i < extract.qso_numbers.size(); i++) {
				size_t qso_num = extract.qso_numbers[i];
				extract.qso_number_to_index[qso_num] = i;
			}
		}
	}
}

//! Update extracts after QSO insertion
void zl_server::update_extracts_after_insertion(size_t qso_number) {
	for (auto& extract_pair : logbook_extracts_) {
		extract_t& extract = extract_pair.second;
		// Find the position to insert
		size_t insert_index = extract.qso_numbers.size();
		// Binary search
		size_t left = 0;
		size_t right = extract.qso_numbers.size();
		while (left < right) {
			size_t mid = (left + right) / 2;
			if (extract.qso_numbers[mid] < qso_number) {
				left = mid + 1;
			}
			else {
				right = mid;
			}
		}
		insert_index = left;
		// Increment indices of existing QSOs after insertion point
		for (size_t i = insert_index; i < extract.qso_numbers.size(); i++) {
			size_t qso_num = extract.qso_numbers[i];
		}
		repair_extract_reverse_maps(extract_pair.first);
	}
}

//! Remove QSO from worked-before data
void zl_server::remove_qso_from_wb4(size_t qso_number) {
	// TODO: Copy from book.cpp
}

//! Repair reverse maps in extract \p book_id
void zl_server::repair_extract_reverse_maps(uint8_t book_id) {
	auto extract_it = logbook_extracts_.find(book_id);
	if (extract_it == logbook_extracts_.end()) {
		return;
	}
	extract_t& extract = extract_it->second;
	extract.qso_number_to_index.clear();
	for (size_t i = 0; i < extract.qso_numbers.size(); i++) {
		size_t qso_num = extract.qso_numbers[i];
		extract.qso_number_to_index[qso_num] = i;
	}
}

//! Get the number of the QSO closest to but not after \p qso
//! \param qso The QSO record to compare
size_t zl_server::get_closest_qso_before(const json& qso) {
	// Binary search
	size_t left = 0;
	size_t right = logbook_data_.size();
	while (left < right) {
		size_t mid = (left + right) / 2;
		if (is_qso_later(logbook_data_[mid], qso)) {
			right = mid;
		}
		else {
			left = mid + 1;
		}
	}
	return left;
}

	