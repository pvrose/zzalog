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

*/#pragma once

#include "cty_data.h"

#include <istream>
#include <map>
#include <string>

//! This class reads the CSV iso.csv file containing a mapping of 
//! ISO Country Codes to DXCC entities.


class cty4_reader
{
public:
	//! Constructor.
	cty4_reader();
	//! Destructor.
	~cty4_reader();

	//! \brief Load data from incoming stream to database.
	//! \param data Internal database.
	//! \param in input stream.
	//! \param version Returns any version information in the file.
	//! \return true if successful, false if not.
	bool load_data(cty_data* data, std::istream& in, std::string& version);

protected:

	//! Internal structure
	struct iso_record {
		std::string iso_cc;             //!< ISO Country Code
		std::string sovereign_state;    //!< Sovereign State
	};
	//! Map of entity name to ISO record.
	std::map< std::string, iso_record > iso_map_;



};
	