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
#include "cty_element.h"

#include <fstream>
#include <string>



//! This class reads data from cty.dat from country-files.com.
class cty2_reader
{
public:
	//! Constructor.
	cty2_reader();
	//! Destructor.
	~cty2_reader();

	//! Load data

	//! \param data Internal database.
	//! \param in input stream.
	//! \param version Returns any version information in the file.
	//! \return true if successful, false if not.
	bool load_data(cty_data* data, std::istream& in, std::string& version);

protected:
	//! Load an entity item.
	
	//! \param entity Entity item to load.
	//! \param in Input stream.
	//! \param dxcc DXCC entity identifier: will be updated.
	//! \return true if succussful, false if not.
	bool load_entity(cty_entity* entity, std::istream& in, int& dxcc);

	//! Takes each pattern from the record and generates an element from it.
	
	//! \param value individual pattern.
	//! \param match updated with calsign if it is an exception.
	//! \param exception returns true if an exception pattern, otherwise false.
	//! \return either a cty_prefix* or a cty_exception* depending on the pattern.
	cty_element* load_pattern(std::string value, std::string& match, bool& exception);
	//! The internal inport database,
	cty_data* data_;

	

};

