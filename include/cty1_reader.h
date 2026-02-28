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

#include <list>
#include <string>
#include<istream>



	//! This class reads the XML cty.xml file obtained from Clublog.org
	
	/** XML structure
	\code

	<clublog>
		:
		:
		<exceptions>
			<exception record = '6002'>
				<call>VE1ST/NA14</call>
				<entity>CANADA</entity>
				< adif>1 </adif >
				< cqz>4 </cqz >
				<cont>NA</cont>
				<long>-97.14 </long >
				< lat>49.90 </lat >
			</exception>
			:
			:
		</exceptions>
		:
		:
		<invalid_operations>
			<invalid record = '489'>
				<call>T88A</call>
				< start>1995-05-01T00:00:00+00:00 </start >
				< end>1995-12-31T23:59:59+00:00 </end >
			</invalid>
			:
			:
		</invalid_operations>
		:
		:
		<zone_exceptions>
			<zone_exception record = '59'>
				<call>KD6WW/VY0</call>
				< zone>1 </zone >
				< start>2003-07-30T00:00:00+00:00 </start >
				< end>2003-07-31T23:59:59+00:00 </end >
			</zone_exception>
			:
			:
		</zone_exceptions>
		:
		:
	</clublog>
	\endcode
	*/

	class cty1_reader
	{
	public:
		//! Constructor.
		cty1_reader();
		//! Destructor.
		~cty1_reader();

		//! Load data
		
		//! \param data Internal database.
		//! \param in input stream.
		//! \param version Returns any version information in the file.
		//! \return true if successful, false if not.
		bool load_data(cty_data* data, std::istream& in, std::string& version);
		// Protected methods
	protected:

		//! Converts data in standard XML format to "YYYYMMDD" format.
		std::string xmldt2date(std::string xml_data);

	protected:
		//! The internal database being loaded.
		cty_data* data_;
		//! Value of element
		std::string value_;
		////! Input stream from file.
		//std::istream* file_;
		//! Number of elements read
		int number_read_;


	};


