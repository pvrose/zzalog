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
#include "qsl_image.h"

#include "qsl_data.h"
#include "qsl_dataset.h"
#include "qsl_display.h"
#include "record.h"

#include <FL/Fl_Device.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Image_Surface.H>

Fl_RGB_Image* qsl_image::image(record* qso, qsl_data::qsl_type type) {

	// Get the QSL card design data
	qsl_display* qsl = new qsl_display(0, 0);
	qsl_data* data = qsl_dataset_->get_card(qso->item("STATION_CALLSIGN"), type, false);
	qsl->set_card(data);
	qsl->set_qsos(&qso, 1);

	int w, h;
	qsl->get_size(w, h);

	// Create the drawing surface to the size of the card
	Fl_Image_Surface* surface = new Fl_Image_Surface(w, h);
	// direct all further graphics requests to the image
	Fl_Surface_Device::push_current(surface);

	// Draw the design
	qsl->draw();

	Fl_RGB_Image* image = surface->image();

	// Restore previous drawing surface
	Fl_Surface_Device::pop_current();

	delete surface;
	delete qsl;

	return image;
}