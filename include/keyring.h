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

#include <cstdint>
#include <map>
#include <string>

//! This class provides access to the API keys for non-user specific data.
class keyring {

public:
	//! Constructor.
	//! Opens the keyring file "keys.json", craeting it if necessary and reading all the keys.
	keyring();
	//! Destructor
	~keyring();
	//! Load data
	bool load_data();
	//! Save data
	bool save_data();
	//! Return the key for specified \p item.
	const std::string key(const std::string& item);
	//! Set key \p item to \p value.
	void key(const std::string& item, const std::string& value);

protected:
	//! The bunch of keys
	std::map<std::string, std::string> keys_;
	//! Seed used
	uint32_t seed_ = 0;
};

extern keyring* keyring_;