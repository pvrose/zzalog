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

#include "zc_rpc_data_item.h"
#include "zc_rpc_handler.h"

#include <sstream>

class record;
class zc_socket_server;

class fldigi_handler {

public:
	//! Constructor.
	fldigi_handler();
	//! Destructor.
	~fldigi_handler();

	//! Start the server.
	void run_server();
	//! Close the server.
	void close_server();
	//! Returns the server state.
	bool has_server();
	//! Returns true if a request has been received.
	bool has_data() const;

protected:
	//! Fetch the first record that matches callsign request.

	//! \param v Pointer to this instance.
	//! \param params Request.
	//! \param response Returned response.
	//! \return true if request fails, otherwise false.
	static int get_record(void* v, zc_rpc_data_item::rpc_list& params, zc_rpc_data_item& response);
	//! Check duplicate - replies true (exact match), possible (callsign matches), false (not a match.

	//! \param v Pointer to this instance.
	//! \param params Request.
	//! \param response Returned response.
	//! \return true if request fails, otherwise false.
	static int check_dup(void* v, zc_rpc_data_item::rpc_list& params, zc_rpc_data_item& response);
	//! Add new record

	//! \param v Pointer to this instance.
	//! \param params Request.
	//! \param response Returned response.
	//! \return true if request fails, otherwise false.
	static int add_record(void* v, zc_rpc_data_item::rpc_list& params, zc_rpc_data_item& response);
	//! Update fileds in current selection

	//! \param v Pointer to this instance.
	//! \param params Request.
	//! \param response Returned response.
	//! \return true if request fails, otherwise false.
	static int update_record(void* v, zc_rpc_data_item::rpc_list& params, zc_rpc_data_item& response);
	//! List methods - std::string responds with a std::list of methods suppported

	//! \param v Pointer to this instance.
	//! \param params Request.
	//! \param response Returned response.
	//! \return true if request fails, otherwise false.
	static int list_methods(void* v, zc_rpc_data_item::rpc_list& params, zc_rpc_data_item& response);

	//! Generate error response

	//! \param code Error code.
	//! \param message Error message
	//! \param response Formatted response for sending to client.
	void generate_error(int code, std::string message, zc_rpc_data_item& response);

	//! Check FlDigi isconnected
	void check_connected();

	//! The currently selected record.
	record* current_qso_;
	//! The record possibly being created.
	record* putative_qso_;
	//! XMK-RPC handler.
	zc_rpc_handler* zc_rpc_handler_;
	//! The std::list of methods supported by the RPC interface
	std::list<zc_rpc_handler::method_entry> method_list_;

	//! Connected
	bool connected_;



};

// #endif
