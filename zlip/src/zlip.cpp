#include "zlip.h"

//! Convert zlip struct to json
void to_json(json& j, const zlip& s) {
	j["Client ID"] = s.client_id;
	j["Request ID"] = s.request_id;
	j["Record ID"] = s.record_id;
	j["Command"] = s.command;
	j["Payload"] = s.payload;
}

//! Convert json to zlip struct
void from_json(const json& j, zlip& s) {
	j.at("Client ID").get_to(s.client_id);
	j.at("Request ID").get_to(s.request_id);
	j.at("Record ID").get_to(s.record_id);
	j.at("Command").get_to(s.command);
	j.at("Payload").get_to(s.payload);
}
