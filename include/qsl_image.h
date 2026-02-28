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

#include "qsl_data.h"

class Fl_RGB_Image;
class record;

//! This class is a container for the static method image.
class qsl_image
{
public:
	//! Create the image from the QSO record \p qso for the specified QSL \p type.
	
	//! It draws the image on a temporary surface then returns the captured image.
	static Fl_RGB_Image* image(record* qso, qsl_data::qsl_type type);

};

