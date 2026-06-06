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
#include "intl_dialog.h"

#include "file_types.h"
#include "menu_bar.h"
#include "win_dialog.h"

#include "zc_callback.h"
#include "zc_drawing.h"
#include "zc_file_holder.h"
#include "zc_settings.h"
#include "zc_status.h"
#include "zc_fltk.h"

#include <cmath>
#include <fstream>
#include <string>

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/fl_utf8.h>
#include <FL/Fl_Widget.H>

// Majo
std::string DEFAULT_INTL = "";
extern std::string CONTACT;
extern std::string COPYRIGHT;
intl_dialog* intl_dialog_ = nullptr;
extern void open_html(const char* topic);
extern Fl_Widget* paste_target_;

// Constructs a window 
intl_dialog::intl_dialog() :
	win_dialog(640, 480, "International character set")
{
	// Load the data
	if (load_data()) {
		// create the dialog
		create_form();
		load_settings();
	} 
	end();
}

// Create the form
void intl_dialog::create_form() {

	// Delete the existing form and re-build it
	int curr_x = GAP;
	int curr_y = GAP;
	clear();
	begin();
	buttons_ = new Fl_Group(curr_x, curr_y, 10, 10);
	end();
	resizable(nullptr);
	add_buttons();
}

// Load settings
void intl_dialog::load_settings() {
	zc_settings settings;
	zc_settings views(&settings, "Views");
	zc_settings intl_dialog_settings(&views, "International Characters");
	bool open;
	intl_dialog_settings.get<bool>("Open Automatically", open, false);
	int top, left;
	intl_dialog_settings.get<int>("Top", top, 100);
	intl_dialog_settings.get<int>("Left", left, 100);
	position(left, top);
	if (open) {
		show();
	}
}

// Save settings
void intl_dialog::save_settings() {
}


// Add buttons with all the wanted characters
void intl_dialog::add_buttons() {
	// Remove existing buttons and start adding again
	buttons_->clear();
	buttons_->begin();
	// Get size of array
	int button_width = HBUTTON * 5 / 4;
	int num_cols = std::ceil(std::sqrt(symbols_.size()));
	int width = (button_width * num_cols) + GAP * 2;
	int num_rows;
	// Have to do it this way as mixed signed/unsigned arithmetic
	if (symbols_.size()) {
		num_rows = ((symbols_.size() - 1) / num_cols) + 1;
	}
	else {
		num_rows = 0;
	}
	// Max 4 bytes per UTF character plus terminal \0
	char utf8[5];
	int len;
	// position of first button
	int curr_x = buttons_->x();
	int curr_y = buttons_->y();
	auto ucs = symbols_.begin();
	// For each row and column
	for (int R = 0; R < num_rows; R++) {
		for (int C = 0; C < num_cols && ucs != symbols_.end(); C++) {
			// Get the next character from the list 
			len = fl_utf8encode(ucs->first, utf8);
			utf8[len] = 0;
			// Button - copy and paste the label on the button to the current editor
			Fl_Button* bn = new Fl_Button(curr_x, curr_y, button_width, button_width);
			// Fit the label font to approx 70%
			bn->labelsize(button_width * 14 / 20);
			bn->copy_label(utf8);
			bn->callback(cb_bn_use);
			bn->copy_tooltip(ucs->second.c_str());
			curr_x += bn->w();
			ucs++;
		}
		curr_x = GAP;
		curr_y += button_width;
	}
	int height = curr_y + button_width;

	buttons_->end();
	// Adjust the size of the buttons group
	buttons_->resizable(nullptr);
	buttons_->size(width, height);
	// Adjust the size of  the window to fit
	Fl_Box* b_cr = new Fl_Box(x(), buttons_->y() + buttons_->h(), width + GAP, FOOT_HEIGHT * 2);
	b_cr->copy_label(std::string(COPYRIGHT + "     \n" + CONTACT + "     ").c_str());
	b_cr->labelsize(FL_NORMAL_SIZE - 1);
	b_cr->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

	size(buttons_->x() + buttons_->w() + GAP, buttons_->y() + buttons_->h() + b_cr->h());
}

// Desctructor
intl_dialog::~intl_dialog()
{
	clear();
	save_settings();
}

// Handle FL_HIDE and FL_SHOW to get menu to update otself
int intl_dialog::handle(int event) {
	int result;
	switch (event) {
	case FL_HIDE:
	case FL_SHOW:
		// Get menu to update Windows controls
		menu_bar_->update_windows_items();
		break;
	case FL_FOCUS:
		return true;
	case FL_UNFOCUS:
		// Acknowledge focus events to get the keyboard event
		return true;
	case FL_PUSH:
		// Allow normal click behaviour before changing focus
		result = win_dialog::handle(event);
		if (!result) take_focus();
		return result;
	case FL_KEYBOARD:
		switch (Fl::event_key()) {
		case FL_F + 1:
			open_html("intl_dialog.html");
			return true;
		}
		break;
	}

	return win_dialog::handle(event);
}

// Paste the buttons label into the editor widget
void intl_dialog::cb_bn_use(Fl_Widget* w, void* v) {
	const char* utf8 = ((Fl_Button*)w)->label();
	intl_dialog* that = zc::ancestor_view<intl_dialog>(w);
	if (paste_target_) {
		int len = strlen(utf8);
		// Copy to clipboard
		Fl::copy(utf8, len);
		// Paste to currently open editor
		Fl::paste(*paste_target_);
		// Set focus to that widget
		Fl::focus(paste_target_);
		// Get the paste target to accept focus again
		paste_target_->take_focus();
	}
}

// Load the initial character data
bool intl_dialog::load_data() {
	symbols_.clear();
	std::string filename;
	std::ifstream is;
	;
	std::string line;
	if (!file_holder_->get_file(FILE_INTLCHARS, is, filename)) {
		symbols_.clear();
		return true;
	}
	else {
		while (is.good()) {
			getline(is, line);
			if (is.good()) {
				// Convert the UTF-8 bytes to a UCS code
				const char* p = line.c_str();
				int len;
				unsigned int ucs = fl_utf8decode(p, nullptr, &len);
				len += 3; // Skip any description separator and whitespace
				symbols_[ucs] = line.substr(len);
			}
		}
		if (is.eof()) {
			return true;
		}
		else {
			return false;
		}
	}
}
