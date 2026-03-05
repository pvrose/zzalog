#include "bumr_dialog.h"

#include "book.h"
#include "page_dialog.h"

#include "zc_drawing.h"
#include "zc_filename_input.h"
#include "zc_settings.h"

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Output.H>


// Constructor
bumr_dialog::bumr_dialog(int X, int Y, int W, int H, const char* L) :
	page_dialog(X, Y, W, H, L)
{
	// use page_dialog default
	do_creation(X, Y);
	enable_widgets();
}

// Destructor
bumr_dialog::~bumr_dialog()
{
}

// Handle F1 key to open user guide
int bumr_dialog::handle(int event) {
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
			open_html("bumr_dialog.html");
			return true;
		}
		break;
	}
	return result;
}

// Load data from settings
void bumr_dialog::load_values() {
	zc_settings settings;
	zc_settings behaviour_settings(&settings, "Behaviour");
	mirror_settings_ = new zc_settings(&behaviour_settings, "Mirror");
	backup_settings_ = new zc_settings(&behaviour_settings, "Backup");
}

// Create the form
void bumr_dialog::create_form(int X, int Y) {
	int cx = X + GAP;
	int cy = Y + GAP;
	const int WFILE = WBUTTON * 5;
	const int WGRP = GAP + WLABEL + WFILE + GAP;
	char text[128];
	std::string s_value;
	bool b_value;

	// Mirror use
	op_mirror_use_ = new Fl_Output(cx + GAP + WLABEL, cy, WFILE, HBUTTON);
	op_mirror_use_->align(FL_ALIGN_LEFT);
	op_mirror_use_->value(book_->get_mirror_use().c_str());	
	op_mirror_use_->box(FL_FLAT_BOX);
	op_mirror_use_->tooltip("Whether the mirror file is being used as the working data.");
	op_mirror_use_->color(FL_BACKGROUND_COLOR);
	cy += op_mirror_use_->h();
	// Log filename
	op_logname_ = new Fl_Output(cx + GAP + WLABEL, cy, WFILE, HBUTTON, "Working file");
	op_logname_->align(FL_ALIGN_LEFT);
	op_logname_->value(book_->get_filename().c_str());
	op_logname_->box(FL_FLAT_BOX);
	op_logname_->tooltip("The current log file as being used.");
	op_logname_->color(FL_BACKGROUND_COLOR);
	cy += op_logname_->h();
	// Real filename
	op_realname_ = new Fl_Output(cx + GAP + WLABEL, cy, WFILE, HBUTTON, "Real file");
	op_realname_->align(FL_ALIGN_LEFT);
	op_realname_->value(book_->get_real_filename().c_str());
	op_realname_->box(FL_FLAT_BOX);
	op_realname_->tooltip("The real log file of which the working file is a copy.");
	op_realname_->color(FL_BACKGROUND_COLOR);
	cy += op_realname_->h() + GAP;
	// Backup configuration
	const int HBUG = HTEXT + HBUTTON * 2 + GAP;
	grp_backup_ = new Fl_Group(cx, cy, WGRP, HBUG, "Backup configuration");
	grp_backup_->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	grp_backup_->box(FL_BORDER_BOX);
	cy += HTEXT;
	cx += GAP + WLABEL;
	// Backup path
	ip_bu_path_ = new zc_filename_input(cx, cy, WFILE, HBUTTON, "Path");
	ip_bu_path_->align(FL_ALIGN_LEFT);
	backup_settings_->get("Path", s_value, std::string(""));
	ip_bu_path_->value(s_value.c_str());
	ip_bu_path_->tooltip("Directory in which backup files will be stored");
	cy += ip_bu_path_->h();
	// Backup depth
	ip_bu_depth_ = new Fl_Int_Input(cx, cy, WBUTTON, HBUTTON, "Depth");
	ip_bu_depth_->align(FL_ALIGN_LEFT);
	int i_value;
	backup_settings_->get("Depth", i_value, 0);
	snprintf(text, sizeof(text), "%d", i_value);
	ip_bu_depth_->value(text);
	ip_bu_depth_->tooltip("Number of backup files to maintain");
	grp_backup_->end();
	
	// Mirror configuration
	cx = X + GAP;
	cy = grp_backup_->y() + grp_backup_->h() + GAP;
	const int HMRG = HTEXT + HBUTTON * 3 + GAP;
	grp_mirror_ = new Fl_Group(cx, cy, WGRP, HMRG, "Mirror configuration");
	grp_mirror_->box(FL_BORDER_BOX);
	grp_mirror_->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	cx += GAP + WLABEL;
	cy += HTEXT;
	// Mirror enable
	bn_mr_enable_ = new Fl_Check_Button(cx, cy, HBUTTON, HBUTTON, "Enable");
	bn_mr_enable_->align(FL_ALIGN_LEFT);
	mirror_settings_->get("Enabled", b_value, false);
	bn_mr_enable_->value(b_value);
	bn_mr_enable_->tooltip("Enable or disable mirroring");
	// Mirror path
	cy += bn_mr_enable_->h();
	ip_mr_path_ = new zc_filename_input(cx, cy, WFILE, HBUTTON, "Path");
	ip_mr_path_->align(FL_ALIGN_LEFT);
	mirror_settings_->get("Path", s_value, std::string(""));
	ip_mr_path_->value(s_value.c_str());
	ip_mr_path_->tooltip("Directory in which mirror files will be stored");
	// Using mirror copy as woorking data.
	cy += ip_mr_path_->h();
	bn_mr_working_ = new Fl_Check_Button(cx, cy, HBUTTON, HBUTTON, "Use as\nworking");
	bn_mr_working_->align(FL_ALIGN_LEFT);
	mirror_settings_->get("Writeback", b_value, false);
	bn_mr_working_->value(b_value);
	bn_mr_working_->tooltip("Use the mirror copy as the working data (changes will be written back to the real file at close");
	grp_mirror_->end();

	end();
	show();
}

// Save values back to settings
void bumr_dialog::save_values() {
	// Save backup settings - no need to inform anyone.
	backup_settings_->set("Depth", atoi(ip_bu_depth_->value()));
	backup_settings_->set("Path", ip_bu_path_->value());
	// Save mirror settings - we need to inform the book as this may change how it operates.
	// See if any of the settings have changed.
	// If not we can avoid the overhead of flushing the book data to the mirror file.
	bool changed = false;
	bool orig_enabled;
	mirror_settings_->get("Enabled", orig_enabled, false);
	if (orig_enabled != (bool)bn_mr_enable_->value()) {
		changed = true;
	}
	bool orig_writeback;
	mirror_settings_->get("Writeback", orig_writeback, false);
	if (orig_writeback != (bool)bn_mr_working_->value()) {
		changed = true;
	}
	std::string orig_path;
	mirror_settings_->get("Path", orig_path, std::string(""));
	if (orig_path != ip_mr_path_->value()) {
		changed = true;
	}
	// Now save the settings
	mirror_settings_->set("Enabled", bn_mr_enable_->value());
	mirror_settings_->set("Path", std::string(ip_mr_path_->value()));
	mirror_settings_->set("Writeback", bn_mr_working_->value());
	// If any of the settings have changed then flush the book and update the mirror settings in the book.
	if (changed) {
		book_->flush_data();
		book_->load_mirror_settings();
	}
}

// Enable widgets - deactivate widgets when group is disabled
void bumr_dialog::enable_widgets() {
	// Mirror widgets
	bool mr_enabled = bn_mr_enable_->value();
	if (mr_enabled) {
		ip_mr_path_->activate();
		bn_mr_working_->activate();
	}
	else {
		ip_mr_path_->deactivate();
		bn_mr_working_->deactivate();
	}
}