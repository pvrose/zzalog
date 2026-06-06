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
#include "user_dialog.h"

#include "book.h"
#include "log_table.h"
#include <page_dialog.h>
#include "report_tree.h"
#include "spec_tree.h"
#include "tabbed_forms.h"

#include "zc_banner.h"
#include "zc_callback.h"
#include "zc_drawing.h"
#include "zc_settings.h"
#include "zc_status.h"
#include "zc_tabs_nonav.h"
#include "zc_fltk.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Widget.H>

extern void open_html(const char* filename);

// constructor
user_dialog::user_dialog(int X, int Y, int W, int H, const char* label) :
	page_dialog(X, Y, W, H, label) 
{
	// Set defaults
	log_font_ = 0;
	log_size_ = FL_NORMAL_SIZE;
	tip_duration_ = Fl_Tooltip::delay();
	tip_font_ = Fl_Tooltip::font();
	tip_size_ = Fl_Tooltip::size();
	session_elapse_ = 30.0;
	tree_size_ = FL_NORMAL_SIZE;
	tree_font_ = 0;
	if (status_) {
		banner_size_ = status_->get_banner()->fontsize();
		banner_font_ = status_->get_banner()->font();
	} else {
		banner_size_ = FL_NORMAL_SIZE;
		banner_font_ = FL_COURIER;
	}

	do_creation(X, Y);
}

// Destructor
user_dialog::~user_dialog() {}

// Handle
int user_dialog::handle(int event) {
	int result = page_dialog::handle(event);
	// Now handle F1 regardless
	switch (event) {
	case FL_FOCUS:
		return true;
	case FL_UNFOCUS:
		// Acknowledge focus events to get the keyboard event
		return true;
	case FL_PUSH:
		if (!result) take_focus();
		return true;
	case FL_KEYBOARD:
		switch (Fl::event_key()) {
		case FL_F + 1:
			open_html("user_dialog.html");
			return true;
		}
		break;
	}
	return result;
}


// Load values from settings
void user_dialog::load_values() {
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	// Log table
	zc_settings log_settings(&view_settings, "Log Table");
	log_settings.get("Font Name", log_font_, log_font_);
	log_settings.get("Font Size", log_size_, log_size_);
	log_settings.get("Session Gap", session_elapse_, session_elapse_);
	// Tooltip
	zc_settings tip_settings(&view_settings, "Tooltip");
	tip_settings.get("Duration", tip_duration_, Fl_Tooltip::delay());
	tip_settings.get("Font Name", tip_font_, Fl_Tooltip::font());
	tip_settings.get("Font Size", tip_size_, Fl_Tooltip::size());
	// Tree views
	zc_settings tree_settings(&view_settings, "Log Table");
	tree_settings.get("Font Name", tree_font_, (Fl_Font)0);
	tree_settings.get("Font Size", tree_size_, FL_NORMAL_SIZE);
	// Banner
	zc_settings banner_settings(&view_settings, "Banner");
	banner_settings.get("Font Name", banner_font_, banner_font_);
	banner_settings.get("Font Size", banner_size_, banner_size_);
}

// Used to create the form
void user_dialog::create_form(int X, int Y) {
	begin();
	int cx = X + GAP;
	int cy = Y + GAP;

	zc_tabs_nonav* tabs = new zc_tabs_nonav(cx, cy, w() - (2 * GAP), h() - (2 * GAP));
	tabs->callback(cb_tab);
	tabs->box(FL_FLAT_BOX);

	int rx = 0;
	int ry = 0;
	int rw = 0;
	int rh = 0;
	tabs->client_area(rx, ry, rw, rh, 0);

	// Tab 1 - Log table
	Fl_Group* g1 = new Fl_Group(rx, ry, rw, rh, "Log Table");
	g1->box(FL_FLAT_BOX);
	int pos_x = rx + GAP;
	int pos_y = ry + HTEXT;
	Fl_Hold_Browser* br1 = new Fl_Hold_Browser(pos_x, pos_y, WEDIT, HMLIN, "Font");
	br1->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br1->tooltip("Please select the font used in the cells in all log table views");
	populate_font(br1, &log_font_);
	pos_x += br1->w();
	Fl_Hold_Browser* br2 = new Fl_Hold_Browser(pos_x, pos_y, WBUTTON, HMLIN, "Size");
	br2->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br2->callback(cb_br_size, &log_size_);
	br2->tooltip("Please select the font size used in the cells in all log table views");
	br1->callback(cb_br_logfont, br2);
	populate_size(br2, &log_font_, &log_size_);
	pos_y = br2->y() + br2->h() + HTEXT;
	pos_x = br1->x();
	Fl_Counter* val0 = new Fl_Counter(pos_x, pos_y, WEDIT, HBUTTON, "Session elapse time (minutes)");
	val0->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	val0->type(FL_SIMPLE_COUNTER);
	val0->step(10.0);
	val0->range(10.0, 120.0);
	val0->value(session_elapse_);
	val0->callback(zc::cb_value<Fl_Counter, float>, &session_elapse_);
	g1->end();

	// Tab 2 - Tooltips
	Fl_Group* g2 = new Fl_Group(rx, ry, rw, rh, "Tooltips");
	g2->box(FL_FLAT_BOX);
	pos_x = rx + GAP;
	pos_y = ry + HTEXT;
	Fl_Hold_Browser* br3 = new Fl_Hold_Browser(pos_x, pos_y, WEDIT, HMLIN, "Font");
	br3->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br3->tooltip("Please select the font used in the cells in all tooltips");
	populate_font(br3, &tip_font_);
	pos_x += br3->w();
	Fl_Hold_Browser* br4 = new Fl_Hold_Browser(pos_x, pos_y, WBUTTON, HMLIN, "Size");
	br4->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br4->callback(cb_br_size, &tip_size_);
	br4->tooltip("Please select the font size used in the cells in all tooltips");
	br3->callback(cb_br_tipfont, br4);
	populate_size(br4, &tip_font_, &tip_size_);
	pos_y = br4->y() + br4->h() + HTEXT;
	pos_x = br3->x();
	Fl_Counter* val1 = new Fl_Counter(pos_x, pos_y, br3->w(), HBUTTON, "Duration (s)");
	val1->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	val1->type(FL_SIMPLE_COUNTER);
	val1->step(0.5);
	val1->range(1.0, 15.0);
	val1->value(tip_duration_);
	val1->callback(zc::cb_value<Fl_Counter, float>, &tip_duration_);
	val1->tooltip("Please select the time (in seconds) that a tooltip will display");
	g2->end();

	// Tab 3 - Tree views
	Fl_Group* g4 = new Fl_Group(rx, ry, rw, rh, "Tree views");
	g4->box(FL_FLAT_BOX);
	pos_x = rx + GAP;
	pos_y = ry + HTEXT;
	Fl_Hold_Browser* br7 = new Fl_Hold_Browser(pos_x, pos_y, WEDIT, HMLIN, "Font");
	br7->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br7->tooltip("Please select the font used in the cells in all tree views");
	populate_font(br7, &tree_font_);
	pos_x += br7->w();
	Fl_Hold_Browser* br8 = new Fl_Hold_Browser(pos_x, pos_y, WBUTTON, HMLIN, "Size");
	br8->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br8->callback(cb_br_size, &tree_size_);
	br8->tooltip("Please select the font size used in the cells in all tree views");
	br7->callback(cb_br_treefont, br8);
	populate_size(br8, &tree_font_, &tree_size_);
	g4->end();

	// Tab 4 - Status
	Fl_Group* g5 = new Fl_Group(rx, ry, rw, rh, "Status");
	g5->box(FL_FLAT_BOX);
	pos_x = rx + GAP;
	pos_y = ry + HTEXT;
	Fl_Hold_Browser* br9 = new Fl_Hold_Browser(pos_x, pos_y, WEDIT, HMLIN, "Font");
	br9->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br9->tooltip("Please select the font used in the status banner");
	populate_font(br9, &banner_font_);
	pos_x += br9->w();
	Fl_Hold_Browser* br10 = new Fl_Hold_Browser(pos_x, pos_y, WBUTTON, HMLIN, "Size");
	br10->align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
	br10->callback(cb_br_size, &banner_size_);
	br10->tooltip("Please select the font size used in the status banner");
	br9->callback(cb_br_bannerfont, br10);
	populate_size(br10, &banner_font_, &banner_size_);
	g5->end();

	tabs->end();



	end();
	show();
}

// Used to write settings back
void user_dialog::save_values() {
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	// Log table
	zc_settings log_settings(&view_settings, "Log Table");
	log_settings.set("Font Name", log_font_);
	log_settings.set("Font Size", log_size_);
	log_settings.set("Session Gap", session_elapse_);
	// Tell the log views
	log_table::set_font(log_font_, log_size_);
	// Tooltip
	zc_settings tip_settings(&view_settings, "Tooltip");
	tip_settings.set("Duration", tip_duration_);
	tip_settings.set("Font Name", tip_font_);
	tip_settings.set("Font Size", tip_size_);
	// Tell the tooltips
	Fl_Tooltip::delay(tip_duration_);
	Fl_Tooltip::font(tip_font_);
	Fl_Tooltip::size(tip_size_);
	// Tree view settings
	zc_settings tree_settings(&view_settings, "Log Table");
	tree_settings.set("Font Name", tree_font_);
	tree_settings.set("Font Size", tree_size_);
	//((pfx_tree*)tabbed_forms_->get_view(OT_PREFIX))->set_font(tree_font_, tree_size_);
	((report_tree*)tabbed_forms_->get_view(OT_REPORT))->set_font(tree_font_, tree_size_);
	((spec_tree*)tabbed_forms_->get_view(OT_ADIF))->set_font(tree_font_, tree_size_);
	// Banner
	zc_settings banner_settings(&view_settings, "Banner");
	banner_settings.set("Font Name", banner_font_);
	banner_settings.set("Font Size", banner_size_);
	status_->get_banner()->font(banner_font_, banner_size_);

	// Now tell all views to update formats
	book_->selection(-1, HT_FORMAT);
}

// Used to enable/disable specific widget - any widgets enabled must be attributes
void user_dialog::enable_widgets() {
	// Standard tab formats
	// value() returns the selected widget. We need to test which widget it is.
	zc_tabs_nonav* tabs = (zc_tabs_nonav*)child(0);
	Fl_Widget* tab = tabs->value();
	for (int ix = 0; ix < tabs->children(); ix++) {
		Fl_Widget* wx = tabs->child(ix);
		if (wx == tab) {
			wx->labelfont((wx->labelfont() | FL_BOLD) & (~FL_ITALIC));
			wx->labelcolor(FL_FOREGROUND_COLOR);
			wx->activate();
		}
		else {
			wx->labelfont((wx->labelfont() & (~FL_BOLD)) | FL_ITALIC);
			wx->labelcolor(FL_FOREGROUND_COLOR);
			wx->deactivate();
		}
	}

}

// Callback for log_font browser
// v is unused
void user_dialog::cb_br_logfont(Fl_Widget* w, void* v) {
	user_dialog* that = zc::ancestor_view<user_dialog>(w);
	Fl_Hold_Browser* font_br = (Fl_Hold_Browser*)w;
	that->log_font_ = (Fl_Font)font_br->value() - 1;
	that->populate_size((Fl_Hold_Browser*)v, &that->log_font_, &that->log_size_);
}

// Callback for all size browsers
// v is a pointer to the size variable
void user_dialog::cb_br_size(Fl_Widget* w, void* v) {
	Fl_Hold_Browser* size_br = (Fl_Hold_Browser*)w;
	int line = size_br->value();
	if (isdigit(size_br->text(line)[0])) {
		*(Fl_Fontsize*)v = std::stoi(size_br->text(line));
	}
}

// Callback for tooltip font browser
// v is unused
void user_dialog::cb_br_tipfont(Fl_Widget* w, void* v) {
	user_dialog* that = zc::ancestor_view<user_dialog>(w);
	Fl_Hold_Browser* font_br = (Fl_Hold_Browser*)w;
	that->tip_font_ = (Fl_Font)font_br->value() - 1;
	that->populate_size((Fl_Hold_Browser*)v, &that->tip_font_, &that->tip_size_);
}

// Callback for tree view font browser
// v is unused
void user_dialog::cb_br_treefont(Fl_Widget* w, void* v) {
	user_dialog* that = zc::ancestor_view<user_dialog>(w);
	Fl_Hold_Browser* font_br = (Fl_Hold_Browser*)w;
	that->tree_font_ = (Fl_Font)font_br->value() - 1;
	that->populate_size((Fl_Hold_Browser*)v, &that->tree_font_, &that->tree_size_);
}

// Callback for banner font browser
// v is unused
void user_dialog::cb_br_bannerfont(Fl_Widget* w, void* v) {
	user_dialog* that = zc::ancestor_view<user_dialog>(w);
	Fl_Hold_Browser* font_br = (Fl_Hold_Browser*)w;
	that->banner_font_ = (Fl_Font)font_br->value() - 1;
	that->populate_size((Fl_Hold_Browser*)v, &that->banner_font_, &that->banner_size_);
}

// Callback on changing tab
void user_dialog::cb_tab(Fl_Widget* w, void* v) {
	user_dialog* that = zc::ancestor_view<user_dialog>(w);
	that->enable_widgets();
}

// Populate the font browser
void user_dialog::populate_font(Fl_Hold_Browser* br, const Fl_Font* font) {
	br->clear();
	// Only get FLTK default fonts
	for (int i = 0; i < FL_FREE_FONT; i++) {
		// Contains any combination of FL_BOLD and FL_ITALIC
		const char* name = Fl::get_font_name(Fl_Font(i), nullptr);
		char buffer[128];
		// display in the named font
		sprintf(buffer, "@F%d@.%s", i, name);
		br->add(buffer);
	}
	br->value(*font + 1);
}

// Populate the size browser
void user_dialog::populate_size(Fl_Hold_Browser* br, const Fl_Font* font, const Fl_Fontsize* size) {
	br->clear();
	// To receive the array of sizes
	int* sizes;
	int num_sizes = Fl::get_font_sizes(*font, sizes);
	if (num_sizes) {
		// We have some sizes
		if (sizes[0] == 0) {
			// Scaleable font - so any size available 
			for (int i = 1; i < std::max<int>(64, sizes[num_sizes - 1]); i++) {
				char buff[20];
				sprintf(buff, "%d", i);
				br->add(buff);
			}
			br->value(*size);
		}
		else {
			// Only list available sizes
			int select = 0;
			for (int i = 0; i < num_sizes; i++) {
				// while the current value is less than required up the selected value
				if (sizes[i] < *size) {
					select = i;
				}
				char buff[20];
				sprintf(buff, "%d", sizes[i]);
				br->add(buff);
			}
			br->value(select);
		}
	}
}

