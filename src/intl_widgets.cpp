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
#include "intl_widgets.h"

#include "intl_dialog.h"
#include "main.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>

// Constructor
intl_editor::intl_editor(int X, int Y, int W, int H, const char* label) :
	Fl_Text_Editor(X, Y, W, H, label),
	insert_mode_(true)
{
	// Define standard cursor
	insert_mode(insert_mode_);
	if (insert_mode_) {
		cursor_style(Fl_Text_Display::NORMAL_CURSOR);
	}
	else {
		cursor_style(Fl_Text_Display::BLOCK_CURSOR);
	}
};

intl_editor::~intl_editor() {
	if (intl_dialog_) {
		intl_dialog_->editor(nullptr);
	}
}

// Event handler - handle event as normal then set the cursor depending on current insert mode
int intl_editor::handle(int event) {
	// Tell international character dialog to paste to this widget as this is the most recent one to get focus
	switch (event) {
	case FL_FOCUS:
		// Something has tried to give the editor the focus: accept it.
		if (intl_dialog_) {
			intl_dialog_->editor(this);
		}
		return true;
	default:
		// Default handling of all events
		int result = Fl_Text_Editor::handle(event);
		// Change cursor depending on insert_mode()
		insert_mode_ = insert_mode();
		if (insert_mode_) {
			// INS
			cursor_style(Fl_Text_Display::NORMAL_CURSOR);
		}
		else {
			// OVR
			cursor_style(Fl_Text_Display::BLOCK_CURSOR);
		}
		return result;
	}
}

// Version of Fl_Input - constructor
intl_input::intl_input(int X, int Y, int W, int H, const char* label) :
	Fl_Input(X, Y, W, H, label) {};

intl_input::~intl_input() {
	if (intl_dialog_ && intl_dialog_->visible()) {
		intl_dialog_->editor(nullptr);
	}
}

// Event handler
int intl_input::handle(int event) {
	// Tell international character dialog to paste to this widget
	switch (event) {
	case FL_FOCUS:
		if (intl_dialog_) {
			intl_dialog_->editor(this);
			return true;
		}
		break;
	case FL_UNFOCUS:
		if (intl_dialog_) {
			return true;
		}
		break;
	}
	return Fl_Input::handle(event);
}
