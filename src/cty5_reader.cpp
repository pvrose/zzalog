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
#include "cty5_reader.h"

#include "cty_data.h"
#include "cty_element.h"
#include "objects.h"
#include "zc_status.h"
#include "zc_fltk.h"

#include <istream>
#include <string>

// Constructor
cty5_reader::cty5_reader()
{
}

// Destructor
cty5_reader::~cty5_reader()
{
}

// Load data from specified file into and add each record to the map
bool cty5_reader::load_data(cty_data* data, std::istream& in, std::string& version)
{
	std::string line;
	int line_num = 0;
	std::string now = zc::now(false, "%Y%m%d");
	version = "";
	// Initialsie the progress
	status_->progress(
		2,
		OT_PREFIX,
		"Loading Flag emojis from flags.csv",
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
		if (fields.size() != 5) {
			status_->misc_status(
				ST_ERROR, "CTY DATA: Error reading flags.csv - invalid number of fields in line %d",
				line_num
			);
			return false;
		}
		// Fields: Entity Number, Prefix, Name , Flag Emoji, Continent
		int dxcc_id = std::stoi(fields[0]);
		std::string entity = cty_element::expand_name(fields[2]);
		if (flag_map_.find(entity) != flag_map_.end()) {
			status_->misc_status(
				ST_WARNING,
				"CTY DATA: Duplicate entity '%d' in line %d",
				dxcc_id,
				line_num
			);
		}
		else {
			flag_map_[entity] = fields[3];
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
			if (flag_map_.find(ent_name) != flag_map_.end()) {
				ent->flag_emoji_ = flag_map_[ent_name];
			}
			else {
				// Look for the sovereign state for the entity and use that flag if found
				if (ent->sovereign_state_.length()) {
					std::string state_name = cty_element::expand_name(ent->sovereign_state_);
					if (flag_map_.find(state_name) != flag_map_.end()) {
						ent->flag_emoji_ = flag_map_[state_name];
					}
					else {
						status_->misc_status(
							ST_WARNING,
							"CTY DATA: No flag emoji found for entity '%s'",
							ent_name.c_str()
						);
					}
				}
				else {
					status_->misc_status(
						ST_WARNING,
						"CTY DATA: No sovereign state found for entity '%s'",
						ent_name.c_str()
					);
				}
			}
		}
	}

	status_->progress(2, OT_PREFIX);

	return true;
}