#pragma once

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <list>
#include <set>
#include <string>
#include <thread>
#include <vector>

//! Forward declarations
class zlip;

using json = nlohmann::json;

//! This class implements the log-book server functionality.

//! The server services requests from multiple clients to access
//! and update log-book entries. The server maintains the
//! log-book data and ensures coherency between clients.
//! The server periodically saves the log-book data to file.
//! The server periodically sends heartbeat packets to
//! indicate it is alive.
//! Clients must attach to the server before making requests.
//! Clients must detach from the server when they no longer
//! need to access the log-book data.
//! /TODO: Basic implementation - further work required to
//! add locking between threads and ensure data integrity.
class zl_server {

public:
	//! Constructor
	zl_server(std::string filename);

	//! Destructor
	~zl_server();

	//! Start the server
	void start_server();

	//! Request the packet.
	
	//! \param packet The packet to request.
	static void request(zl_server* that, zlip& packet);

	//! Set callback to responsd to incoming packets
	void set_callback(void (*response)(void*, zlip&));

protected:

	//! Load log-book data from file
	bool load_logbook_data(std::string filename);

	//! Save log-book data to file
	bool save_logbook_data(std::string filename);

	//! Process fetch request
	void process_fetch(zlip& packet);

	//! Process fetch match request
	void process_fetch_match(zlip& packet);

	//! Process fetch next request
	void process_fetch_next(zlip& packet);

	//! Process update request
	void process_update(zlip& packet);

	//! Process create request
	void process_create(zlip& packet);

	//! Process delete request
	void process_delete(zlip& packet);

	//! Send response packet
	void send_response(zlip& packet);

	//! Send ack packet
	void send_ack(zlip& packet);

	//! Send nack packet
	void send_nack(zlip& packet);

	//! Send delete ack packet
	void send_delete_ack(zlip& packet);

	//! Process attach request
	void process_attach(zlip& packet);

	//! Process detach request
	void process_detach(zlip& packet);

	//! Send heartbeat packet
	void send_heartbeat();

	//! Send wakeup packet
	void send_wakeup();

	//! Send shutdown packet
	void send_shutdown();

	//! Invoke callbacks
	void invoke_callbacks(zlip& packet);

	//! The thread sending heartbeats
	std::thread* heartbeat_thread_;

	//! The method running the heartbeat thread
	static void run_heartbeat(zl_server* that);

	//! The heartbeat interval in seconds
	const int HEARTBEAT_INTERVAL = 30;

	//! The thread saving the log-book data
	std::thread* save_thread_;

	//! The method running the save thread
	static void run_save(zl_server* that);

	//! The save interval in seconds
	const int SAVE_INTERVAL = 300;

	//! The list of callbacks
	struct callback_entry {
		//! The callback function
		void (*callback_)(void*, zlip&) = nullptr;
		//! The callback object
		void* callback_object_ = nullptr;
	};
	std::list<callback_entry> responses_;

	//! Log-book data - indexed by entry ID. 
	std::vector<json> logbook_data_;

	//! The log-book filename
	std::string filename_;

	//! Attached clients
	std::set<uint8_t> attached_clients_;

	
};
	
