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
#include "win_dialog.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

// Constructor - derived classes will extend this to build the dialog
win_dialog::win_dialog(int W, int H, const char * label) :
	Fl_Window(W, H, label)
	, button_(BN_CANCEL)
{
}

// Destructor
win_dialog::~win_dialog()
{
	//clear();
}

// Show the dialog and wait for OK or Cancel (or any bespoke buttons) to be clicked
button_t win_dialog::display() {
	show();
	// Default to CANCEL otherwise if another event hides the dialog something nasty happens
	button_ = BN_CANCEL;
	// now wait for OK or cancel to be clicked - using the FLTK scheduler ensures
	// other tasks get a look-in
	while (shown()) { Fl::check(); }
	return button_;
}

// This stops the wait and allows the wanted response to be sent
void win_dialog::do_button(button_t button) {
	button_ = button;
	hide();
}

