#pragma once
#include <cstdint>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

//! ZLIP (ZZALOG Interface Protocol) commands.

//! The clients and servers will be separate programs running
//! within the same local network and communicating through a single
//! multicast port.
//! The protocol ensures that coherency between the record data
//! seen in each client matches that maintained in the client.
//! All clients snoop the multicast port and act on the data appropriately. 
//! The behaviour for requesting client, non-requesting client (snooper) and
//! server is detailed for each command.
//! \todo Work still needs to be done to confirm the attach/detach and heartbeat/wakeup/shutdown
//! detailed requirements. 
//! - In the event of sudden server failure the clients may have
//! the latest copy of entries that the server had not written through to file-store.
//! - Two clients may try to
//! attach with the same request_id. How to handle that?
enum zlip_command : uint8_t {
	//! No operation
	
	//! Sent by client or server
	//! - client_id = 0 - ignored
	//! - request_id = 0 - ignored
	//! - record_id = 0 - ignored
	//! - payload = NULL - ignored
	ZLIP_NOP = 0x00,

	//! Fetch specific log entry
	
	//! Sent by client
	//! - client_id = N - set by the requesting client.
	//! - request_id = N - incremented from previous request_id by the requestor.
	//! - record_id = N - the id of the requested log entry.
	//! - payload = NULL
	//! 
	//! Server responds:
	//! - ZLIP_RESPONSE if the entry with record_id exists.
	//! - ZLIP_NACK if the entry does not exist.
	//! 
	//! Snooper ignores.
	ZLIP_FETCH = 0x01,

	//! Fetch matching log entry.
	
	//! Sent by client
	//! - client_id = N - set by the requesting client
	//! - request_id = N - incremented from previous request_id by the requestor.
	//! - record_id = N - number of entry at which to start the search.
	//! - payload - contains fields that the srever tries to match against entries.
	//! 
	//! Server responds:
	//! - ZLIP_RESPONSE if an entry that matches the payload exists.
	//!   - record_id in the response is set to that of the matching entry.
	//! - ZLIP_NACK if no entry matches the payload.
	//! 
	//! Snooper ignores.
	ZLIP_FETCH_MATCH = 0x02,

	//! Fetch next existing log entry
	
	//! Sent by client
	//! - client_id = N - set by the requesting client
	//! - request_id = N - incremented from previous request_id by the requestor.
	//! - record_id = N - number of entry at which to start the search.
	//! - payload = NULL
	//! 
	//! Server responds:
	//! - ZLIP_RESPONSE if an entry with the supplied or higher record_id exists
	//!   - record_id in the response is set to that of the returned entry.
	//! - ZLIP_NACK if no entry exists at this or subsequent record_id.
	//! 
	//! Snooper ignores.
	ZLIP_FETCH_NEXT = 0x03,

	//! Update log entry

	//! Sent by client
	//! - client_id = N - set by the requesting client
	//! - request_id = N - incremented from previous request_id by the requestor.
	//! - record_id = N - number of entry to update
	//! - payload = data to update: note only the data fields to be updated need
	//!   be supplied in the payload. All other fields are left unchanged.
	//! 
	//! Server responds:
	//! - ZLIP_ACK if the update is successful.
	//! - ZLIP_NACK if no entry exists at this or subsequent record_id.
	//! 
	//! Snooper ignores.
	ZLIP_UPDATE = 0x04,

	//! Create new log entry

	//! Sent by client
	//! - client_id = N - set by the requesting client
	//! - request_id = N - incremented from previous request_id by the requestor.
	//! - record_id = 0 - new entry being requested.
	//! - payload = data to use to create new entry.
	//! 
	//! Server responds:
	//! - ZLIP_ACK if the creation is successful. No reason not to be.
	//! 
	//! Snooper ignores.
	ZLIP_CREATE = 0x05,

	//! Delete log entry

	//! Sent by client
	//! - client_id = N - set by the requesting client
	//! - request_id = N - incremented from previous request_id by the requestor.
	//! - record_id = N - number of entry to be deleted.
	//! - payload = data to use to create new entry.
	//! 
	//! Server responds:
	//! - ZLIP_DELETE_ACK if the creation is successful.
	//! - ZLIP_NACK if no entry exists to delete with that record_id.
	//! 
	//! Snooper ignores.
	ZLIP_DELETE = 0x06,

	//! Returned log entry

	//! Sent by server.
	//! - client_id = N - returned by server indicates the requesting client.
	//! - request_id = N - returned by server indicates the request.
	//! - record_id = N - set by server to the record id of the returned entry.
	//! - payload = complete data for the requested log entry.
	//! 
	//! Requestor updates its local copy of the log entry.
	//! 
	//! Snooper checks record_id to see if it is interested. It either updates or invalidates
	//! its local copy of the log entry as appropriate.
	ZLIP_RESPONSE = 0x07,

	//! Server accepts update/create

	//! Sent by server.
	//! - client_id = N - returned by server indicates the requesting client.
	//! - request_id = N - returned by server indicates the request.
	//! - record_id = N - set by server to the record id of the returned entry.
	//! - payload = complete data for the updated or created log entry.
	//! 
	//! Requestor ignores - update/create already done.
	//! 
	//! Snooper checks record_id to see if it is interested. It either updates or invalidates
	//! its local copy of the log entry as appropriate.
	ZLIP_ACK = 0x0F,

	//! No log entry exists with that record_id

	//! Sent by server.
	//! - client_id = N - returned by server indicates the requesting client.
	//! - request_id = N - returned by server indicates the request.
	//! - record_id = N - returned by server indicates the requested record id.
	//! - payload = NULL
	//! 
	//! Requestor invalidates its local copy of the log entry.
	//! 
	//! Snooper checks record_id to see if it is interested. It invalidates
	//! its local copy of the log entry.
	ZLIP_NACK = 0x10,

	//! Server acknowledgeds delete

	//! Sent by server.
	//! - client_id = N - returned by server indicates the requesting client.
	//! - request_id = N - returned by server indicates the request.
	//! - record_id = N - returned by server indicates the deleted record id.
	//! - payload = NULL
	//! 
	//! Requestor ignores - delete already done.
	//! 
	//! Snooper checks record_id to see if it is interested. It invalidates
	//! its local copy of the log entry as appropriate.
	ZLIP_DELETE_ACK = 0x11,

	//! Attach client to server
	
	//! Sent by client to request attachment to server. This is the first command
	//! sent by a client and must be accepted or rejected
	//! by the server before any other commands.
	//! - client_id = 0 - indicates request for client_id N.
	//! - request_id = N - randomly chosen by the client for this request.
	//! - record_id = 0 - ignored
	//! - payload = NULL - ignored
	//! 
	//! Server responds:
	//! - ZLIP_ATTACH_ACCEPT to accept the client attachment. It returns
	//! the allocated client_id.
	//! 
	//! Snooper ignores.
	ZLIP_ATTACH = 0x08,

	//! Accept client attachment

	//! Sent by the server to accept attachment of a client.
	//! - client_id = N - allocated client id.
	//! - request_id = N - echoed from the attach request.
	//! - record_id = 0 - ignored
	//! - payload = NULL - ignored
	//! 
	//! Requestor notes that it is now attached to the server and
	//! uses the client_id for subsequent requests.
	//! 
	//! Snooper ignores.
	ZLIP_ATTACH_ACCEPT = 0x09,

	//! Detach client from server
	
	//! Sent by client to detach from server.
	//! - client_id = N - the id of the detaching client.
	//! - request_id = 0 - ignored
	//! - record_id = 0 - ignored
	//! - payload = NULL - ignored
	//! 
	//! Server notes that the client is no longer attached.
	//! The client_id becomes available for reuse.
	//! 
	//! Snooper ignores.
	ZLIP_DETACH = 0x0B,

	//! Server heartbeat
	
	//! Sent by server periodically to indicate it is alive.
	//! - client_id = 0 - ignored
	//! - request_id = 0 - ignored
	//! - record_id = N: number of records in the log-book. 
	//! - payload = NULL - ignored
	//! 
	//! All clients must monitor heartbeats to ensure server is alive. 
	//! If the heartbeat is not seen for a configured period the client
	//! report the server as offline and must re-attach when the server
	//! wakes up.
	ZLIP_HEARTBEAT = 0x0C,

	//! Server wakeup notification
	
	//! Sent by server on startup to indicate it has started.
	//! 
	//! /TODO: Confirm behaviour - is heartbeat sufficient?
	//! All clients must re-attach to the server after receiving
	//! the wakeup notification. All clients must discard any
	//! local log data as it is now invalid.
	ZLIP_WAKEUP = 0x0D,

	//! Server shutdown notification
	
	//! Sent by server on shutdown to indicate it is going offline.
	//! 
	//! All clients must treat the server as offline after
	//!	receiving the shutdown notification. All clients must
	//! report the server as offline and report any local log data
	//! as not having been saved.
	ZLIP_SHUTDOWN = 0x0E,

};

//! ZLIP (ZZALOG Interface Protocol) packet structure.

//! This structure defines the packet format for ZLIP ZZALOG interface communication.
//! This interface supports logbook record requests between a number of clients and
//! a single server. See zlip_command for details of each command and its use within 
//! the protocol.
struct zlip { 
	//! Client ID

	//! Unique identifier for the client initiating the request.
	//! 0 = initiated by server
	//! 1 to` 255 = initiated by client
	uint8_t client_id = 0;

	//! Request ID

	//! Unique identifier for the request per client.
	//! Allocated by client, echoed by server.
	uint16_t request_id = 0;

	//! Command

	//! Specifies the command to be executed.
	zlip_command command = ZLIP_NOP;

	//! Payload

	//! JSON formatted payload containing command-specific data.
	json payload = nlohmann::json();

	//! Record ID

	//! Unique identifier for the log record.
	//! Allocated by server, echoed by client.
	//! Unique for lifetime of log book.
	uint32_t record_id;
};

NLOHMANN_JSON_SERIALIZE_ENUM(zlip_command, {
	{ ZLIP_NOP, "NOP" },
	{ ZLIP_FETCH, "FETCH" },
	{ ZLIP_FETCH_MATCH, "FETCH_MATCH" },
	{ ZLIP_FETCH_NEXT, "FETCH_NEXT" },
	{ ZLIP_UPDATE, "UPDATE" },
	{ ZLIP_CREATE, "CREATE" },
	{ ZLIP_DELETE, "DELETE" },
	{ ZLIP_RESPONSE, "RESPONSE" },
	{ ZLIP_ACK, "ACK" },
	{ ZLIP_NACK, "NACK" },
	{ ZLIP_DELETE_ACK, "DELETE_ACK" },
	{ ZLIP_ATTACH, "ATTACH" },
	{ ZLIP_ATTACH_ACCEPT, "ATTACH_ACCEPT" },
	{ ZLIP_DETACH, "DETACH" },
	{ ZLIP_HEARTBEAT, "HEARTBEAT" },
	{ ZLIP_WAKEUP, "WAKEUP" },
	{ ZLIP_SHUTDOWN, "SHUTDOWN" }
	}
);

//! Convert zlip struct to json
void to_json(json& j, const zlip& s);
//! Convert json to zlip struct
void from_json(const json& j, zlip& s);

