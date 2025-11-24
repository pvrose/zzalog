#pragma once

#include "zlip.h"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <list>
#include <mutex>
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

	//! Set the server request generater
	void set_request_handler(zlip_client_method_t handler);

	//! Request handler
	static void request_handler(void* inst, zlip& info);

protected:
	//! Handle a client request
	void handle_request(zlip& info);

	//! Handle a client attach request
	void handle_attach_client(zlip& info);

	//! Handle a client detach request
	void handle_detach_client(zlip& info);

	//! Handle a get QSO request
	void handle_get_qso(zlip& info);

	//! Handle a delete QSO request
	void handle_delete_qso(zlip& info);

	//! Handle an extract data request
	void handle_extract_data(zlip& info);

	//! Handle a clear extract request
	void handle_clear_extract(zlip& info);

	//! Handle a get worked-before status request
	void handle_get_wb4_status(zlip& info);

	//! Handle a save QSO request
	void handle_update_qso(zlip& info);

	//! Handle a insert QSO request
	void handle_insert_qso(zlip& info);

	//! Handle a reserve QSO number request
	void handle_reserve_qso_number(zlip& info);

	// //! Handle a check QSO request
	// void handle_check_qso(zlip& info);

	// //! Handle a find duplicate QSO request
	// void handle_find_duplicates(zlip& info);

	//! Load log-book data from file
	bool load_logbook_data(std::string filename);

	//! Save log-book data to file
	bool save_logbook_data(std::string filename);

	//! Run heartbeat thread
	static void run_heartbeat(zl_server* that);

	//! Run save thread
	static void run_save(zl_server* that);

	//! Send heartbeat to all attached clients
	void send_heartbeat();

	//! Send server shutdown to all attached clients
	void send_server_shutdown();

	//! Get the true QSO index from book ID and QSO number
	size_t get_true_qso_index(uint8_t book_id, size_t qso_number);

	//! Update worked-before status and add derived fields
	void update_derived_fields(size_t qso_number);

	//! Remove QSO from extracts
	void remove_qso_from_extracts(size_t qso_number);

	//! Update extracts after QSO insertion
	void update_extracts_after_insertion(size_t qso_number);

	//! Remove QSO from worked-before data
	void remove_qso_from_wb4(size_t qso_number);

	//! Return true if QSO \p rhs is later than QSO \p lhs
	bool is_qso_later(const json& lhs, const json& rhs);

	//! Repair reverse maps in extract \p book_id
	void repair_extract_reverse_maps(uint8_t book_id);

	//! Get the number of the QSO closest to but not after \p qso
	size_t get_closest_qso_before(const json& qso);

	//! Heartbeat interval in seconds
	const size_t HEARTBEAT_INTERVAL = 30;

	//! Save interval in seconds
	const size_t SAVE_INTERVAL = 300;

	//! Heartbeat thread
	std::thread* heartbeat_thread_ = nullptr;

	//! Save thread
	std::thread* save_thread_ = nullptr;

	//! Log-book data - indexed by entry ID. 
	std::vector<json> logbook_data_;

	//! The log-book filename
	std::string filename_;

	//! Attached clients
	std::set<uint8_t> attached_clients_;

	zlip_client_method_t request_handler_ = nullptr;

	//! Mutex for log-book data	
	//! Stops trying to change the shape of the log-book data
	//! while writing or reading it.
	std::mutex logbook_mutex_;

	//! Extract data
	struct extract_t {
		//! Extracted QSO numbers
		std::vector<size_t> qso_numbers;
		//! Reverse map of QSO number to extract index
		std::map<size_t, size_t> qso_number_to_index;
	};

	//! Log-book extracts - indexed by book ID
	std::map<uint8_t, extract_t> logbook_extracts_;

	//! Set of dirty QSO record (by numner)
	std::set<size_t> dirty_qsos_;
		
};
	
