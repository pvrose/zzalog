#pragma once
#include <cstdint>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum zlip_command : uint8_t;

//! ZLIP supports the transfer of logbook records between multiple clients
//! and a single server. It also supports snooper clients that monitor
//! the logbook activity without being able to modify it.
//! Each client calls a method that is supplied to it as
//! a function pointer.
//! The function is either a method in a port expander or a method in
//! in a socket server allowing the method call over a network.
//! The input method of the port expander can be connected to more than
//! one client or the server.
//! One output method of the port expander can be connected to the
//! server as a blocking method call.
//! Other output methods of the port expander can be connected to
//! snooper clients as non-blocking method calls.
struct zlip {
	//! The command to execute.
	zlip_command command = (zlip_command)0; 
	//! The client ID issuing the command.
	uint8_t client_id = 0;   
	//! The QSO number to operate on.
	size_t qso_number = 0;   
	//! The logbook extraction ID.
	//! 0 indicates the main logbook.
	//! x'FF' indicates a new extraction.
	uint8_t book_id = 0;     
	//! The request data
	json& request = json();      
	//! The response data
	json& response = json();    
};

enum zlip_command : uint8_t {
	//! No operation.
	ZLIP_NOP = 0,

	//! \brief Get the QSO numbered in the qso_number field from the
	//! logbook extract.

	//! Server response:
	//! Returns the QSO record as a JSON object in the response field.
	//! Snooper response:
	//! No operation.
	ZLIP_GET_QSO,

	//! \brief Delete the QSO numbered in the qso_number field from the
	//! logbook. Requests using a book_id other than 0 are translated
	//! to delete a QSO from the main logbook.

	//! Server response:
	//! Erases the QSO record from the logbook extract. No response data.
	//! Snooper response:
	//! Invalidates all QSO records.
	ZLIP_DELETE_QSO,

	//! \brief Update the QSO record supplied in the request field. 
	//! Requests using a book_id other than 0 are translated
	//! to update the QSO in the main logbook.
	//! Server response:
	//! Update the QSO record in the logbook extract. No response data.
	//! Snooper response:
	//! Invalidate the QSO record with the given QSO number.
	ZLIP_UPDATE_QSO,

	//! \brief Reserve a new QSO number.
	//! No QSO number is required in the qso_number field.
	//! No book_id is required.
	//! Server response:
	//! The next available QSO number is returned in the response field.
	//! Client and snooper response:
	//! Update the local QSO number counter.
	ZLIP_RESERVE_QSO,

	//! \brief Insert a new QSO record supplied in the request field.
	//! No QSO number is required in the qso_number field.
	//! No book_id is required.
	//! Server response:
	//! Inserts the new QSO record in the logbook according to
	//! the chronological order. The assigned QSO number is returned.
	//! Client and snooper response:
	//! Invalidate all QSO records.
	ZLIP_INSERT_QSO,

	//! \brief Check whether a QSO with given details matches 
	//! an existing QSO in the logbook.
	//! No QSO number is required in the qso_number field.
	//! No book_id is required.
	//! Server response:
	//! Checks whether the QSO record supplied in the request field
	//! matches an existing QSO record in the logbook. The possible
	//! matching QSO number is returned in the response field as well
	//! the match details.
	//! The request also includes match_flags to control the match operation.
	//! Client response:
	//! Refer the match response data to the user, and possibly
	//! make further requests.
	//! Snooper response:
	//! No operation.
//	ZLIP_CHECK_QSO,

	//! \brief Check for duplicate entries in the logbook.
	//! No QSO number is required in the qso_number field.
	//! No book_id is required.
	//! Server response:
	//! The logbook is scanned for duplicate entries. When a
	//! possible duplicate is found, the two QSO numbers and details
	//! are returned in the response field.
	//! Client response:
	//! Refer the duplicate response data to the user, and possibly
	//!	make further requests.
	//! Snooper response:
	//! No operation.
	//ZLIP_FIND_DUPLICATES,

	//! \brief Start or continue a logbook extraction.
	//! Mo QSO number is required in the qso_number field.
	//! The book_id field is used to identify the logbook extraction.
	//! A book_id of 0'FF' indicates to create a new logbook extraction
	//! otherwise the existing logbook extraction with the given book_id
	//! will be augmented.
	//! Request: A serach criteria JSON object is supplied in the
	//! request field to define which QSOs to include in the extraction.
	//! Server response:
	//! The server creates a new logbook extraction or continues 
	//! the existing logbook extraction with the given book_id and 
	//! responds with the book_id and number of QSOs extracted
	//! in the response field.
	//! Snooper response:
	//! No operation.
	ZLIP_EXTRACT_DATA,

	//! \brief Clear a logbook extraction.
	//! No QSO number is required in the qso_number field.
	//! The book_id field is used to identify the logbook extraction.
	//! Server response:
	//! Clears the logbook extraction with the given book_id.
	//! Snooper response:
	//! No operation.
	ZLIP_CLEAR_EXTRACT,

	//! \brief Request for a client to attach itself to the server.
	//! Client ID is assigned by the server.
	//! Server response:
	//! Suply the first available client ID in the response field.
	//! Snooper response:
	//! No operation.
	ZLIP_ATTACH_CLIENT,

	//! \brief Request for a client to detach itself from the server.
	//! Client ID is supplied in the client_id field.
	//! Server response:
	//! Removes the client_id from the active client list
	//! and makes it available for new clients.
	//! Snooper response:
	//! No operation.
	ZLIP_DETACH_CLIENT,

	//! \brief Server is shutting down.
	//! Client ID is 0 indicating a server broadcast.
	//! Snooper clients receive the command and reset their state.
	ZLIP_SERVER_SHUTDOWN,

	//! \brief Server sends a heartbeat to all clients.
	//! Client ID is 0 indicating a server broadcast.
	//! Server sends the command periodically to all clients.
	//! Server sends the logbook status in the request field.
	ZLIP_SERVER_HEARTBEAT,

	//! \brief Get "Worked before" status for a given callsign.
	//! The callsign is obtained form the QSO record supplied
	//! by qso_number and the book_id.
	//! Server response:
	//! The "Worked before" status is returned in the response field.
	ZLIP_GET_WB4_STATUS,
};

//! ZLIP client method type.
//! The method is called with a zlip structure
//! to process the command.
//! \param inst Pointer to the server or snooper processing the request
//! \param info The zlip structure containing the command
using zlip_client_method_t = void (*)(void* inst, zlip& info);

NLOHMANN_JSON_SERIALIZE_ENUM (zlip_command, {
	{ZLIP_NOP, "NOP"},
	{ZLIP_GET_QSO, "GET_QSO"},
	{ZLIP_DELETE_QSO, "DELETE_QSO"},
	{ZLIP_UPDATE_QSO, "UPDATE_QSO"},
	{ZLIP_RESERVE_QSO, "RESERVE_QSO"},
	{ZLIP_INSERT_QSO, "INSERT_QSO"},
//	{ZLIP_CHECK_QSO, "CHECK_QSO"},
//	{ZLIP_FIND_DUPLICATES, "FIND_DUPLICATES"},
	{ZLIP_EXTRACT_DATA, "EXTRACT_DATA"},
	{ZLIP_CLEAR_EXTRACT, "CLEAR_EXTRACT"},
	{ZLIP_ATTACH_CLIENT, "ATTACH_CLIENT"},
	{ZLIP_DETACH_CLIENT, "DETACH_CLIENT"},
	{ZLIP_SERVER_SHUTDOWN, "SERVER_SHUTDOWN"},
	{ZLIP_SERVER_HEARTBEAT, "SERVER_HEARTBEAT"},
	{ZLIP_GET_WB4_STATUS, "GET_WB4_STATUS"},
	}
)

//! Serialize zlip structure to JSON
void to_json(json& j, const zlip& p);
//! Deserialize zlip structure from JSON
void from_json(const json& j, zlip& p);