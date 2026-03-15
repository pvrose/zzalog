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
#include "cty2_reader.h"

#include "cty_data.h"
#include <cty_element.h>
#include "objects.h"
#include "zc_status.h"

#include <zc_utils.h>

#include <string>
#include <vector>
#include <istream>

cty2_reader::cty2_reader() {
	data_ = nullptr;
}

cty2_reader::~cty2_reader() {
}

// Load data from specified file into and add each record to the map
bool cty2_reader::load_data(cty_data* data, std::istream& in, std::string& version) {
	data_ = data;
	// calculate the file size and initialise the progress bar
	std::streampos startpos = in.tellg();
	in.seekg(0, std::ios::end);
	std::streampos endpos = in.tellg();
	long file_size = (long)(endpos - startpos);
	// reposition back to beginning
	in.seekg(0, std::ios::beg);
	// Initialsie the progress
	status_->progress(file_size, OT_PREFIX, "Loading country data from country-files.com", "bytes");

	while (in.good()) {
		cty_entity* entry = new cty_entity;
		int dxcc;
		if (load_entity(entry, in, dxcc)) {
			data_->add_entity(entry);
		}
		else {
			delete entry;
		}
	}
	if (in.eof()) {
		return true;
	}
	else {
		return false;
	}
}

bool cty2_reader::load_entity(cty_entity* entry, std::istream& in, int& dxcc) {
	std::string line;
	getline(in, line);
	if (in.good()) {
		std::vector<std::string> items;
		zc::split_line(line, items, ',');
		if (items[0][0] == '*') return false;
		entry->nickname_ = items[0];
		entry->name_ = items[1];
		dxcc = std::stoi(items[2]);
		entry->dxcc_id_ = dxcc;
		entry->continent_ = items[3];
		entry->cq_zone_ = std::stoi(items[4]);
		entry->itu_zone_ = std::stoi(items[5]);
		// The longitude appears to be positive for West in this file
		entry->coordinates_ = { std::stod(items[6]), -std::stod(items[7]) };
		//entry->timezone = std::stod(items[8]);
		// Now parse patterns
		std::vector<std::string> patts;
		zc::split_line(items[9], patts, ' ');
		for (auto& it : patts) {
			std::string match;
			bool exception;
			cty_element* entry = load_pattern(it, match, exception);
			entry->dxcc_id_ = dxcc;
			if (exception) {
				data_->add_exception(match, (cty_exception*)entry);
			}
			else {
				data_->add_prefix(match, (cty_prefix*)entry);
			}
		}
		// Report progress 
		int bytes = (int)in.tellg();
		status_->progress(bytes, OT_PREFIX);
		return true;
	}
	else {
		if (in.eof()) return true;
		else return false;
	}
}

cty_element* cty2_reader::load_pattern(std::string patt, std::string& match, bool& exception) {
	size_t pos = 0;
	size_t spos = 0;
	match = "";
	bool zone = false;
	cty_element* result;
	if (patt[pos] == '=') {
		result = new cty_exception;
		exception = true;
		pos++;
	}
	else {
		result = new cty_prefix;
		exception = false;
	}
	while (pos < patt.length()) {
		switch (patt[pos]) {
		case '(':
			spos = pos + 1;
			zone = true;
			break;
		case ')':
			result->cq_zone_ = std::stoi(patt.substr(spos, pos - spos));
			zone = false;
			break;
		case '[':
			spos = pos + 1;
			zone = true;
			break;
		case ']':
			result->itu_zone_ = std::stoi(patt.substr(spos, pos - spos));
			zone = false;
			break;
		case ';':
		case '\r':
		case '\n':
			// ignore 
			break;
		default:
			if (!zone) {
				match += patt[pos];
			}
			break;
		}
		pos++;
	}
	return result;
}
