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
#pragma once

#include "cty_data.h"

#include <istream>
#include <map>
#include <string>

	//! This class reads the CSV iso.csv file containing a mapping of 
	//! ISO Country Codes to DXCC entities.


class cty5_reader
{
public:
	//! Constructor.
	cty5_reader();
	//! Destructor.
	~cty5_reader();

	//! \brief Load data from incoming stream to database.
	//! \param data Internal database.
	//! \param in input stream.
	//! \param version Returns any version information in the file.
	//! \return true if successful, false if not.
	bool load_data(cty_data* data, std::istream& in, std::string& version);

protected:

	//! Map of entity name to Unicode flag emoji. This is used to add the flag emoji to the entity data.
	std::map< std::string, std::string> flag_map_;



};
