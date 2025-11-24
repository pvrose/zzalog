#include "zlip.h"


// //! Serialize zlip structure to JSON
void to_json(json& j, const zlip& z)
{
	j = json{
		{"command", z.command},
		{"client_id", z.client_id},
		{"qso_number", z.qso_number},
		{"book_id", z.book_id},
		{"request", z.request},
		{"response", z.response}
	};
}

// //! Deserialize zlip structure from JSON	
void from_json(const json& j, zlip& z)
{
	j.at("command").get_to(z.command);
	j.at("client_id").get_to(z.client_id);
	j.at("qso_number").get_to(z.qso_number);
	j.at("book_id").get_to(z.book_id);
	j.at("request").get_to(z.request);
	j.at("response").get_to(z.response);
}