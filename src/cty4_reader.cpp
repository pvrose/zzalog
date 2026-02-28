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
#include "cty4_reader.h"

#include "cty_data.h"
#include "cty_element.h"
#include "objects.h"
#include "zc_status.h"
#include "zc_utils.h"

#include <istream>
#include <string>

// Constructor
cty4_reader::cty4_reader()
{
}

// Destructor
cty4_reader::~cty4_reader()
{
}

// Load data from specified file into and add each record to the map
bool cty4_reader::load_data(cty_data* data, std::istream& in, std::string& version)
{
	std::string line;
	int line_num = 0;
	std::string now = zc::now(false, "%Y%m%d");
	version = "";
	// Initialsie the progress
	status_->progress(
		2,
		OT_PREFIX,
		"Loading ISO country codes from iso.csv",
		"steps"
	);

	// Read header line
	if (!std::getline(in, line)) {
		return false;
	}
	// Process each line
	while (std::getline(in, line)) {
		line_num++;
		std::vector<std::string> fields;
		zc::split_line(line, fields, ',');
		if (fields.size() != 3) {
			status_->misc_status(
				ST_ERROR, "CTY_DATA: Error reading iso.csv - invalid number of fields in line %d",
				line_num
			);
			return false;
		}
		// Fields: ISO-2, Sovereign State, Entity Name
		std::string entity = cty_element::expand_name(fields[2]);
		iso_record rec = { fields[0], fields[1] };
		if (iso_map_.find(entity) != iso_map_.end()) {
			status_->misc_status(
				ST_WARNING,
				"CTY_DATA: Duplicate entity '%s' in line %d",
				entity.c_str(),
				line_num
			);
		}
		else {
			iso_map_[entity] = rec;
		}
	}

	status_->progress(1, OT_PREFIX);

	// Now update entities in database
	for (auto& ent_pair : data->data()->entities) {
		cty_entity* ent = ent_pair.second;
		std::string ent_name = cty_element::expand_name(ent->name_);
		if (ent->dxcc_id_ > 0 &&
			!ent->deleted_ &&
			ent->time_contains(now)) {
			if (iso_map_.find(ent_name) != iso_map_.end()) {
				ent->iso_cc_ = iso_map_[ent_name].iso_cc;
				ent->sovereign_state_ = iso_map_[ent_name].sovereign_state;
			}
			else {
				status_->misc_status(
					ST_WARNING,
					"CTY_DATA: No ISO code found for entity '%s'",
					ent->name_.c_str()
				);
			}
		}
	}

	status_->progress(2, OT_PREFIX);

	return true;
}