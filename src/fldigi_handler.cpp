#include "fldigi_handler.h"

#include "adi_reader.h"
#include "main.h"
#include "menu_bar.h"
#include "qso_apps.h"
#include "qso_data.h"
#include "qso_manager.h"
#include "record.h"
#include "socket_server.h"
#include "status.h"

#include <sstream>

// Constructor
fldigi_handler::fldigi_handler() {
	server_ = nullptr;
	run_server();
	connected_ = false;
}

fldigi_handler::~fldigi_handler() {
	close_server();
}

//! Returns true if server has been started
bool fldigi_handler::has_server() {
	return server_ != nullptr && server_->has_server();
}

//! Start server
void fldigi_handler::run_server() {
	if (!server_) {
		std::string address = qso_manager_->apps()->network_address(FLDIGI);
		int udp_port = qso_manager_->apps()->network_port(FLDIGI);
		if (address.length()) {
			server_ = new socket_server(socket_server::UDP, address, udp_port);
			server_->callback(this, rcv_request);
		}
		else return;
	}
	if (!server_->has_server()) {
		status_->misc_status(ST_NOTE, "FLDIGI: Starting socket");
		server_->run_server();
	}
	menu_bar_->update_items();

}

//! Close servver
void fldigi_handler::close_server() {
	if (server_) {
		status_->misc_status(ST_NOTE, "FLDIGI: Closing server");
		server_->close_server(true);
		delete server_;
		server_ = nullptr;
	}
}

//! Callback from server thread: Receive a datagram from FLDIGI
int fldigi_handler::rcv_request(void* instance, std::stringstream& os) {
	return ((fldigi_handler*)instance)->rcv_dgram(os);
}

//! Receive a datagram from FLDIGI
int fldigi_handler::rcv_dgram(std::stringstream& os) {
	// Set connected
	if (!connected_) {
		connected_ = true;
		qso_manager_->enable_widgets();
	}
	// Get ADIF payload
	adi_reader* reader = new adi_reader();
	adi_reader::load_result_t dummy;
	record* dummy_qso = new record;
	reader->load_record(dummy_qso, os, dummy);
	record* qso = qso_manager_->start_modem_qso(dummy_qso->item("CALL"), qso_data::QSO_COPY_FLDIGI);
	for(auto& it : *dummy_qso) {
		if (qso->item_exists(it.first)) {
			if (qso->item(it.first) != it.second) {
				char msg[128];
				snprintf(msg, sizeof(msg), "FLDIGI: Updating QSO Field %s: was %s now %s",
			        it.first.c_str(), qso->item(it.first).c_str(), it.second.c_str());
				status_->misc_status(ST_WARNING, msg);
			}
	 	} 
		qso->item(it.first, it.second);
	}
	qso_manager_->update_modem_qso(true);
	status_->misc_status(ST_NOTE, "FLDIGI: Logged QSO");
	return 1;
}

// Received data
bool fldigi_handler::has_data() const {
	return connected_;
}
