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

#include "contest_algorithm.h"
#include <record.h>

#include <set>
#include <string>

//! Provides the individual contest algorithms, allowing them to have simple names.
namespace contests {

    //! Implementation of contest_algorithm for a basic context exchange.

    //! The receive and transmit exchanges are RS(T) and Serial Number.
    //! Scoring is 1 point per unique contact in a different DXCC entity with 
    //! a multiplier of 1 per DXCC entity per band
    class basic :
        public contest_algorithm
    {
    public:

        //! Contructor.
        
        //! Identifies the receive exchange items as RST_RCVD and SRX.
        //! Identifies the transmit exchange items as RST_SENT and STX.
        //! Adds itself to the global attribute algorithms_.
        basic();

        //! Algorithm specific method to split text into a number of fields.
        
        //! The supplied \a text is parsed as "[RST_RCVD] [SRX]" and
        //! written into \a qso fields of these names.
        virtual void parse_exchange(record* qso, std::string text);

        //! Algorithm specific method to generate text from a number of fields.
        
        //! The default RS(T) is written into the RST_SENT field of \a qso.
        //! The fields RST_SENT and STX are concatenated and returned from
        //! the method.
        virtual std::string generate_exchange(record* qso);

        //! Algorithm specific method to score an individual QSO.
        
        //! The fields DXCC and BAND are concatenated to create a 
        //! multiplier. If this multiplier is not present in \a multipliers
        //! it is added and the value 1 is set in the multiplier attribute
        //! of the return value.
        //! If the DXCC entity of \a qso is not the same as the user's
        //! DXCC entity a value of 1 is returned in the qso_points 
        //! attribute of the return value.
        virtual score_result score_qso(record* qso, std::set<std::string>& multipliers);

        //! The algorithm uses serial numbers.
        
        //! \return true.
        virtual bool uses_serno() { return true; }

    };

}

