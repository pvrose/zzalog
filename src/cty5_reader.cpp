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
#include "zc_file_holder.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <FL/Fl_SVG_Image.H>

#include <fstream>
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
	// Pre-populate the flag map using sovereign state flags.
	for (auto ent_pair : data->data()->entities) {
		cty_entity* ent = ent_pair.second;
		if (ent->iso_cc_.empty()) {
			ent->flag_filename_ = "";
		} else {
			ent->flag_filename_ = zc::to_lower(ent->iso_cc_) + ".png";
		}
	}
	// Override exceptions with the flag images from the flags.json file.
	json jall;
	in >> jall;

	if (jall.find("Flags") == jall.end()) {
		status_->misc_status(
			ST_ERROR,
			"CTY DATA: No 'Flags' section found in JSON data"
		);
		return false;
	}
	json jflags = jall["Flags"];
	for (auto it = jflags.begin(); it != jflags.end(); ++it) {
		int dxcc_id = std::stoi(it.key());
		cty_entity* ent = data->data()->entities[dxcc_id];
		ent->flag_filename_ = zc::to_lower(it.value());
	}

	status_->progress(2, OT_PREFIX);

	return true;
}

