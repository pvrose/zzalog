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
#include "main_window.h"

#include "import_data.h"
#include "menu_bar.h"

#include <sstream>
#include <string>

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

main_window* main_window_ = nullptr;

// Create the window
main_window::main_window(int W, int H, const char* label) :
	Fl_Double_Window(W, H, label)
{
}

main_window::~main_window() {}

// Handle FL_HIDE and FL_SHOW to get menu to update itself
// Handle FL_PASTE to import QSOs from the clipboard
int main_window::handle(int event) {

	switch (event) {
	case FL_HIDE:
	case FL_SHOW:
		// Get menu to update Windows controls
		if(menu_bar_) menu_bar_->update_windows_items();
		break;
	case FL_PASTE:
		// Get data from paste
		std::string data = Fl::event_text();
		std::stringstream adif;
		adif.str(data);
		// Stop any extant update and wait for it to complete
		import_data_->stop_update(false);
		while (!import_data_->update_complete()) Fl::check();
		import_data_->load_stream(adif, import_data::update_mode_t::CLIPBOARD);
		// Wait for the import to finish
		while (import_data_->size()) Fl::check();
		return true;
	}

	return Fl_Double_Window::handle(event);
}
