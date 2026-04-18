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
#include "page_dialog.h"
#include "zc_callback.h"

#include "zc_fltk.h"

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Widget.H>



// Default constructor
page_dialog::page_dialog(int X, int Y, int W, int H, const char* label) :
	Fl_Group(X, Y, W, H, label)
{
	callback(cb_bn_ok);
}

// Provides a reference creation process
void page_dialog::do_creation(int X, int Y) { 
	// Load initial configuration - typically from settings
	this->load_values();
	// Construct the dialog from its widgets
	this->create_form(X, Y);
	// Enable or disable widgets depending on values of various attributes
	this->enable_widgets();
}

// Default destructor
page_dialog::~page_dialog()
{
	//clear();
}

// Default OK button - save confiuguation in settings
void page_dialog::cb_bn_ok(Fl_Widget* w, void* v) {
	page_dialog* that = (page_dialog*)w;
	that->save_values();
}

// Default enable button 
void page_dialog::cb_ch_enable(Fl_Widget* w, void* v) {
	page_dialog* that = zc::ancestor_view<page_dialog>(w);
	zc::cb_value<Fl_Check_Button, bool>(w, v);
	that->enable_widgets();
}