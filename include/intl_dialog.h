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
#ifndef __INTL_DIALOG__
#define __INTL_DIALOG__

#include "win_dialog.h"

#include <string>
#include <set>



class Fl_Widget;
class Fl_Group;

	//! \brief This class provides a window that can be used to paste non-ASCII 
	//! characters into the specified editor widget.
	
	//! The editor widget tells this dialog when it opens.
	//! It comprises a number of buttons each of which copies a character to clipboard and pastes it into
	//! the current input or editor widget.
	//! The available characters for pasting can be added to.
	class intl_dialog : public win_dialog
	{
	public:
		//! Constructor.
		intl_dialog();
		//! Destructor.
		virtual ~intl_dialog();

		//! Override win_dialog::handle().
		
		//! Handles HIDE and SHOW events to allow menu to control its visibility.
		//! TAkes focus to allow keyboard F1 to open userguide.
		virtual int handle(int event);

		//! Callback from "Save" button - calls save_data().
		static void cb_bn_save(Fl_Widget* w, void* v);
		//! Callback from "Reload" button - clears existing data and calls load_data().
		static void cb_bn_restore(Fl_Widget* w, void* v);
		//! Callback from "Add" button - adds the characters in the "New" input widget.
		static void cb_bn_add(Fl_Widget* w, void* v);
		//! Callback from the data buttons - copies character to clipboard then pastes to target widget.
		static void cb_bn_use(Fl_Widget* w, void* v);

	protected:
		//! Load settings
		void load_settings();
		//! Save settings
		void save_settings();

		//! Load data from \p filename_.
		bool load_data();
		//! Save data to \p filename_.
		bool save_data();
		//! Instantiate all component widgets.
		void create_form();
		//! Add all the "use character" button group.
		void add_buttons(int width);
		//! Transfer UTF-8 characters from text input to list of Unicode symbols.
		void add_symbols(std::string text);
		//! The set of Unicode symbols to draw buttons for.
		std::set<unsigned int> symbols_;
		//! New character.
		std::string new_char_;
		//! The button group.
		Fl_Group* buttons_;

	};

	extern intl_dialog* intl_dialog_;

#endif
