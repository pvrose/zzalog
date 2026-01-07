#pragma once

#include <sstream>

class socket_server;

class fldigi_handler {

public:
	//! Constructor.
	fldigi_handler();
	//! Destructor.
	~fldigi_handler();

	//! Returns true if server has been started
	bool has_server();
	//! Start server
	void run_server();
	//! Close servver
	void close_server();

	//! Is there data
	bool has_data () const;

	//! Callback from server thread: Receive a datagram from FLDIGI
	static int rcv_request(void* instance, std::stringstream& os);
	//! Receive a datagram from FLDIGI
	int rcv_dgram(std::stringstream& os);

	//! Socket server
	socket_server* server_;

	//! Server is connected
	bool connected_;

};
