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

#include "main.h"
#include "spec_data.h"

#include <string>
#include <set>
#include <map> 
#include <cmath>


//! \brief Structure to provide a special means of sorting bands known by their
//! wavelength to be sorted on their frequency.
struct band_lt {
    //! Special &lt; operator.
    
    //! \param l LHS of operation. A string containing wavelength as eg. 10M (28~29.7 MHz).
    //! \param r RHS of operation. A string containing wavelength.
    //! \return frequency of LHS is less than frequency of RHS.
    bool operator() (std::string l, std::string r) const {
        // If the supplied string is not a real band name - return assume 0.0Hz
        double lv = spec_data_->freq_for_band(l);
        double rv = spec_data_->freq_for_band(r);
        if (std::isnan(lv)) lv = 0.0;
        if (std::isnan(rv)) rv = 0.0;
        return lv < rv;
    }
};

//! A version of set<string> sorting on the frequency represented by the string value.
class band_set : public std::set<std::string, band_lt>{};

//! A version of map<string, T> sorting on the frequency represented by the string value.
template <class T>
class band_map : public std::map<std::string, T, band_lt>{
};
