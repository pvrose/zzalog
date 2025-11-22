#include "zl_server.h"

#include "zlip.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <thread>

//! Constructor
zl_server::zl_server(std::string filename) {
	filename_ = filename;
	load_logbook_data(filename_);
	start_server();
}

//! Destructor
zl_server::~zl_server() {
	save_logbook_data(filename_);
	send_shutdown();
}

//! Start the server
//! Start 2 threads: 
//! one to send heartbeat packets, 
//! and one to update log-book data to file periodically.
void zl_server::start_server() {
	heartbeat_thread_ = new std::thread(run_heartbeat, this);
	save_thread_ = new std::thread(run_save, this);
}

//! Request the packet.

//! \param packet The packet being requested.
void zl_server::request(zlip& packet) {
	switch (packet.command) {
	case ZLIP_FETCH:
		process_fetch(packet);
		break;
	case ZLIP_FETCH_MATCH:
		process_fetch_match(packet);
		break;
	case ZLIP_FETCH_NEXT:
		process_fetch_next(packet);
		break;
	case ZLIP_UPDATE:
		process_update(packet);
		break;
	case ZLIP_CREATE:
		process_create(packet);
		break;
	case ZLIP_DELETE:
		process_delete(packet);
		break;
	case ZLIP_ATTACH:
		process_attach(packet);
		break;
	case ZLIP_DETACH:
		process_detach(packet);
		break;
	default:
		// Unknown command - ignore
		break;
	}
}

//! Set callback to responsd to incoming packets
void zl_server::set_callback(void (*response)(void*, zlip&)) {
	callback_entry entry;
	entry.callback_ = response;
	entry.callback_object_ = nullptr;
	responses_.push_back(entry);
}

//! Load log-book data from file
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
		logbook_data_.push_back(item);
	}
	return true;
}

//! Save log-book data to file
bool zl_server::save_logbook_data(std::string filename) {
	std::ofstream out(filename.c_str());
	if (!out.is_open()) {
		return false;
	}
	json j = json::array();
	for (const auto& item : logbook_data_) {
		j.push_back(item);
	}
	out << j.dump(4);
	return true;
}

//! Process fetch request
void zl_server::process_fetch(zlip& packet) {
	// Find the record with the specified record_id
	uint32_t record_id = packet.record_id;
	if (record_id < logbook_data_.size()) {
		// Record found
		packet.payload = logbook_data_[record_id];
		if (packet.payload.is_null()) {
			// Record not found
			send_nack(packet);
			return;
		}
		send_response(packet);
	} else {
		// Record not found
		send_nack(packet);
	}
}

//! Process fetch match request
void zl_server::process_fetch_match(zlip& packet) {
	// Find the first record that matches the payload starting from record_id
	uint32_t start_id = packet.record_id;
	for (uint32_t i = start_id; i < logbook_data_.size(); i++) {
		bool match = true;
		for (auto& [key, value] : packet.payload.items()) {
			if (logbook_data_[i].contains(key)) {
				if (logbook_data_[i][key] != value) {
					match = false;
					break;
				}
			} else {
				match = false;
				break;
			}
		}
		if (match) {
			// Match found
			packet.record_id = i;
			packet.payload = logbook_data_[i];
			send_response(packet);
			return;
		}
	}
	// No match found
	send_nack(packet);
}

//! Process fetch next request
void zl_server::process_fetch_next(zlip& packet) {
	// Find the next existing record starting from record_id
	uint32_t start_id = packet.record_id;
	for (uint32_t i = start_id; i < logbook_data_.size(); i++) {
		if (!logbook_data_[i].is_null()) {
			// Record found
			packet.record_id = i;
			packet.payload = logbook_data_[i];
			send_response(packet);
			return;
		}
	}
	// No record found
	send_nack(packet);
}

//! Process update request
void zl_server::process_update(zlip& packet) {
	// Update the record with the specified record_id
	uint32_t record_id = packet.record_id;
	if (record_id < logbook_data_.size()) {
		// Record found - update fields
		for (auto& [key, value] : packet.payload.items()) {
			logbook_data_[record_id][key] = value;
		}
		send_ack(packet);
	} else {
		// Record not found
		send_nack(packet);
	}
}

//! Process create request
void zl_server::process_create(zlip& packet) {
	// Create a new record
	json new_record = packet.payload;
	logbook_data_.push_back(new_record);
	packet.record_id = logbook_data_.size() - 1;
	send_ack(packet);
}

//! Process delete request
void zl_server::process_delete(zlip& packet) {
	// Delete the record with the specified record_id
	uint32_t record_id = packet.record_id;
	if (record_id < logbook_data_.size()) {
		// Record found - delete it
		if (record_id == logbook_data_.size() - 1) {
			// If it's the last record, just pop it
			logbook_data_.pop_back();
		}
		else {
			// Mark as deleted
			logbook_data_[record_id] = json(); // Mark as deleted
		}
		send_delete_ack(packet);
	} else {
		// Record not found
		send_nack(packet);
	}
}

//! Send response packet
void zl_server::send_response(zlip& packet) {
	packet.command = ZLIP_RESPONSE;
	invoke_callbacks(packet);
}

//! Send ack packet
void zl_server::send_ack(zlip& packet) {
	packet.command = ZLIP_ACK;
	invoke_callbacks(packet);
}

//! Send nack packet
void zl_server::send_nack(zlip& packet) {
	packet.command = ZLIP_NACK;
	packet.payload = json();
	invoke_callbacks(packet);
}

//! Send delete ack packet
void zl_server::send_delete_ack(zlip& packet) {
	packet.command = ZLIP_DELETE_ACK;
	packet.payload = json();
	invoke_callbacks(packet);
}

//! Process attach request
void zl_server::process_attach(zlip& packet) {
	// Add client to attached clients list
	for (uint8_t id = 0; id < 255 ; id++) {
		if (attached_clients_.find(id) == attached_clients_.end()) {
			attached_clients_.insert(id);
			packet.client_id = id;
			// Send attach accept response
			packet.command = ZLIP_ATTACH_ACCEPT;
			invoke_callbacks(packet);
			return;
		}
	}
}

//! Process detach request
void zl_server::process_detach(zlip& packet) {
	// Remove client from attached clients list
	uint8_t client_id = packet.client_id;
	attached_clients_.erase(client_id);
}

//! Send heartbeat packet
void zl_server::send_heartbeat() {
	zlip packet;
	packet.client_id = 0;
	packet.request_id = 0;
	packet.command = ZLIP_HEARTBEAT;
	packet.record_id = logbook_data_.size();
	packet.payload = json();
	invoke_callbacks(packet);
}

//! Send wakeup packet
void zl_server::send_wakeup() {
	zlip packet;
	packet.client_id = 0;
	packet.request_id = 0;
	packet.command = ZLIP_WAKEUP;
	packet.record_id = 0;
	packet.payload = json();
	invoke_callbacks(packet);
}

//! Send shutdown packet
void zl_server::send_shutdown() {
	zlip packet;
	packet.client_id = 0;
	packet.request_id = 0;
	packet.command = ZLIP_SHUTDOWN;
	packet.record_id = 0;
	packet.payload = json();
	invoke_callbacks(packet);
}

//! Invoke callbacks
void zl_server::invoke_callbacks(zlip& packet) {
	for (auto& entry : responses_) {
		entry.callback_(entry.callback_object_, packet);
	}
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
