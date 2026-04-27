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
#include "contest_dialog.h"

#include "contest_algo.h"
#include "contest_data.h"
#include "field_choice.h"
#include "page_dialog.h"

#include "zc_calendar.h"
#include "zc_calendar_input.h"
#include "zc_debug.h"
#include "zc_drawing.h"
#include "zc_filename_input.h"
#include "zc_file_holder.h"
#include "zc_file_viewer.h"
#include "zc_fltk.h"
#include "zc_settings.h"
#include "zc_status.h"
#include "zc_utils.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <set>
#include <string>
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Input_.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Widget.H>

extern void open_html(const char* topic);
extern debug_flag DEBUG_DEVELOPMENT;

contest_dialog::contest_dialog(int X, int Y, int W, int H, const char* L) :
	page_dialog(X, Y, W, H, L)
	, contest_(nullptr)
	, contest_id_("")
	, contest_index_("")
{
	load_values();
	create_form(x(), y());
	populate_ct_index();
	populate_algorithm();
	update_contest();
	update_timeframe();
	update_algorithm();
	enable_widgets();

	// Create help window
}

contest_dialog::~contest_dialog() {
}

// Handle
int contest_dialog::handle(int event) {
	int result = page_dialog::handle(event);
	// Now handle F1 regardless
	switch (event) {
	case FL_FOCUS:
		return true;
	case FL_UNFOCUS:
		// Acknowledge focus events to get the keyboard event
		return true;
	//case FL_PUSH:
	//	take_focus();
	//	return true;
	case FL_KEYBOARD:
		switch (Fl::event_key()) {
		case FL_F + 1:
			open_html("contest_dialog.html");
			return true;
		}
		break;
	}
	return result;
}

// inherited methods

// Load values from zc_settings
void contest_dialog::load_values() {
	zc_settings top_settings;
	zc_settings behav_settings(&top_settings, "Behaviour");
	zc_settings contest_settings( &behav_settings, "Contest");
	contest_settings.get<std::string>("Current ID", contest_id_, "");
	contest_settings.get<std::string>("Current Index", contest_index_, "");
}

// Used to create the form
void contest_dialog::create_form(int X, int Y) {
	int curr_x = x() + WLLABEL;
	int curr_y = y() + GAP;

	w_contest_id_ = new field_input(curr_x, curr_y, WSMEDIT, HBUTTON, "Contest ID");
	w_contest_id_->align(FL_ALIGN_LEFT);
	w_contest_id_->callback(cb_id, &contest_id_);
	w_contest_id_->tooltip("Please select the contest ID (per ADIF) to edit");
	w_contest_id_->field_name("CONTEST_ID");

	curr_x += w_contest_id_->w() + WLABEL;
	w_contest_ix_ = new Fl_Input_Choice(curr_x, curr_y, WBUTTON, HBUTTON, "Index");
	w_contest_ix_->align(FL_ALIGN_LEFT);
	w_contest_ix_->callback(cb_index, &contest_index_);
//	w_contest_ix_->input()->when(FL_WHEN_ENTER_KEY_ALWAYS);
	w_contest_ix_->tooltip("Please select the index (eg year) identifying specific contest");

	curr_x = x() + WLLABEL;
	curr_y += GAP + HBUTTON;

	w_algorithm_ = new Fl_Input_Choice(curr_x, curr_y, WSMEDIT, HBUTTON, "Algorithm");
	w_algorithm_->align(FL_ALIGN_LEFT);
	w_algorithm_->tooltip("Please select the algorithm used for scoring and exchange");
	w_algorithm_->callback(cb_algorithm);

	curr_y += HBUTTON;

	w_check_algorithm_file_ = new Fl_Button(curr_x, curr_y, WBUTTON, HBUTTON, "Check");
	w_check_algorithm_file_->callback(cb_check_algorithm_file);
	w_check_algorithm_file_->tooltip("Check the algorithm file exists and is valid");

	curr_x += WBUTTON;
	w_edit_algorithm_file_ = new Fl_Button(curr_x, curr_y, WBUTTON, HBUTTON, "Edit");
	w_edit_algorithm_file_->callback(cb_edit_algorithm_file);
	w_edit_algorithm_file_->tooltip("Open the algorithm file in the default editor");

	if (zc_app::debug(DEBUG_DEVELOPMENT)) {
		curr_x += WBUTTON;
		w_release_algorithm_file_ = new Fl_Button(curr_x, curr_y, WBUTTON, HBUTTON, "Release");
		w_release_algorithm_file_->callback(cb_release_algorithm_file);
		w_release_algorithm_file_->tooltip("Release the algorithm file back to git (development only)");
	}

	curr_x = x() + WLLABEL;


	curr_y += GAP + HBUTTON;

	w_start_date_ = new zc_calendar_input(curr_x, curr_y, WSMEDIT + HBUTTON, HBUTTON, "Contest start");
	w_start_date_->align(FL_ALIGN_LEFT);
	w_start_date_->tooltip("Please specify the start date (UTC) of the contest");
	w_start_date_->callback(cb_timeframe, this);

	curr_x += w_start_date_->w();
	w_start_time_ = new Fl_Int_Input(curr_x, curr_y, WBUTTON, HBUTTON);
	w_start_time_->tooltip("Please specify the start time (UTC) of the contest");
	w_start_time_->callback(cb_timeframe, this);
	curr_y += HBUTTON;
	curr_x = x() + WLLABEL;

	w_finish_date_ = new zc_calendar_input(curr_x, curr_y, WSMEDIT + HBUTTON, HBUTTON, "Contest finish");
	w_finish_date_->align(FL_ALIGN_LEFT);
	w_finish_date_->tooltip("Please specify the finish date (UTC) of the contest");

	curr_x += w_start_date_->w();
	w_finish_time_ = new Fl_Int_Input(curr_x, curr_y, WBUTTON, HBUTTON);
	w_finish_time_->tooltip("Please specify the finish time (UTC) of the contest");

	curr_y += HBUTTON + GAP;
	curr_x = x() + WLLABEL;

	end();
	show();
}

// Used to write zc_settings back
void contest_dialog::save_values() {
	if (!contest_) contest_ = contest_data_->get_contest(contest_id_, contest_index_, true);
	if (contest_) {
		contest_->algorithm = w_algorithm_->value();
		std::string start_date = w_start_date_->value();
		std::string start_time = w_start_time_->value();
		tm* start = new tm;
		zc::string_to_tm(start_date + start_time, *start, "%Y%m%d%H%M%S");
		std::string finish_date = w_finish_date_->value();
		std::string finish_time = w_finish_time_->value();
		tm* finish = new tm;
		zc::string_to_tm(finish_date + finish_time, *finish, "%Y%m%d%H%M%S");
#ifdef _WIN32
		contest_->date.start = std::chrono::system_clock::from_time_t(_mkgmtime(start));
		contest_->date.finish = std::chrono::system_clock::from_time_t(_mkgmtime(finish));
#else
		contest_->date.start = std::chrono::system_clock::from_time_t(timegm(start));
		contest_->date.finish = std::chrono::system_clock::from_time_t(timegm(finish));
#endif
		// Algorithm file
		contest_data_->add_algorithm(contest_->algorithm);
	}
	contest_data_->save_data();
}

// Used to enable/disable specific widget - any widgets enabled must be attributes
void contest_dialog::enable_widgets() {
}

// Update contest values
void contest_dialog::update_contest() {
	contest_ = contest_data_->get_contest(contest_id_, contest_index_);
}

// Update logging fields
void contest_dialog::update_algorithm() {
	if (contest_) {
		w_algorithm_->value(contest_->algorithm.c_str());
	}
	else if (contest_data_->get_algorithms()->size()) {
		w_algorithm_->value(0);
	}
}

// Update timeframe
void contest_dialog::update_timeframe() {
	time_t start;
	time_t finish;
	if (contest_) {
		ct_date_t* timeframe = &contest_->date;
		start = std::chrono::system_clock::to_time_t(timeframe->start);
		finish = std::chrono::system_clock::to_time_t(timeframe->finish);
	}
	else {
		start = time(nullptr);
		finish = time(nullptr);
	}
	tm* stm = gmtime(&start);
	char temp[32];
	strftime(temp, sizeof(temp), "%Y%m%d", stm);
	w_start_date_->value(temp);
	strftime(temp, sizeof(temp), "%H%M%S", stm);
	w_start_time_->value(temp);
	tm* ftm = gmtime(&finish);
	strftime(temp, sizeof(temp), "%Y%m%d", ftm);
	w_finish_date_->value(temp);
	strftime(temp, sizeof(temp), "%H%M%S", ftm);
	w_finish_time_->value(temp);
}

// Callbacks
// Contest ID field_input
void contest_dialog::cb_id(Fl_Widget* w, void* v) {
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	that->contest_id_ = ((field_input*)w)->value();
	that->contest_index_ = "";
	that->populate_ct_index();
	that->w_contest_ix_->value("");
	that->update_contest();
	that->update_algorithm();
	that->update_timeframe();
}

// Contest index
void contest_dialog::cb_index(Fl_Widget* w, void* v) {
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	that->contest_index_ = ((Fl_Input_Choice*)w)->value();
	that->update_contest();
	that->update_algorithm();
	that->update_timeframe();
}

// Algorithm choice
void contest_dialog::cb_algorithm(Fl_Widget* w, void* v) {
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	if (that->contest_) {
		std::string& algorithm = that->contest_->algorithm;
		algorithm = ((Fl_Input_Choice*)w)->value();
		contest_data_->add_algorithm(algorithm);
	}
}

// Algorithm file check
void contest_dialog::cb_check_algorithm_file(Fl_Widget* w, void* v) {
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	std::string algorithm = that->w_algorithm_->value();
	contest_algo* algo = new contest_algo(algorithm);
	if (algo->valid()) {
		status_->misc_status(ST_OK, "Algorithm file is valid");
	} else {
		status_->misc_status(ST_ERROR, "Algorithm file is not valid");
	}
}

// Algorithm file edit
void contest_dialog::cb_edit_algorithm_file(Fl_Widget* w, void* v) {
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	std::string file = contest_data_->get_algorithm_file(that->w_algorithm_->value());
	if (file.empty()) {
		status_->misc_status(ST_ERROR, "No algorithm file specified");
		return;
	}
	zc_file_viewer* viewer = new zc_file_viewer(300, 200, file.c_str());
	viewer->type(zc_file_viewer::VT_FILE);
	viewer->load_file(file);
}

// Algorithm file release (development only)
void contest_dialog::cb_release_algorithm_file(Fl_Widget* w, void* v) {
	if (!zc_app::debug(DEBUG_DEVELOPMENT)) return;
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	file_types file = contest_data_->algorithm_map_[that->w_algorithm_->value()];
	if (file >= FILE_CONTESTS) {
		file_holder_->copy_working_to_source(file);
		file_holder_->copy_source_to_git(file);
	}
	// Update the algorithm file to reflect any changes that may have been made in the working copy.
	contest_data_->save_data();
	// And copy back to source and git
	file_holder_->copy_working_to_source(FILE_CONTESTS);
	file_holder_->copy_source_to_git(FILE_CONTESTS);
}

// Timeframe change
void contest_dialog::cb_timeframe(Fl_Widget* w, void* v) {
	contest_dialog* that = zc::ancestor_view<contest_dialog>(w);
	// Get the start date and time values and update the end date and time to start + 1 day.
	std::string start_date = that->w_start_date_->value();
	std::string start_time = that->w_start_time_->value();
	tm* start = new tm;
	zc::string_to_tm(start_date + start_time, *start, "%Y%m%d%H%M%S");
	// Update the end date and time to start + 1 day.
#ifdef _WIN32
	time_t start_time_t = _mkgmtime(start);
#else
	time_t start_time_t = timegm(start);
#endif
	time_t finish_time_t = start_time_t + 24 * 3600;
	tm* finish = gmtime(&finish_time_t);
	char temp[32];
	strftime(temp, sizeof(temp), "%Y%m%d", finish);
	that->w_finish_date_->value(temp);
	strftime(temp, sizeof(temp), "%H%M%S", finish);
	that->w_finish_time_->value(temp);
}

// Populate contest index choice
void contest_dialog::populate_ct_index() {
	std::set<std::string>* indices = contest_data_->get_contest_indices(contest_id_);
	w_contest_ix_->clear();
	w_contest_ix_->add("");
	if (indices) {
		for (auto& it : *indices) {
			w_contest_ix_->add(it.c_str());
		}
	}
	w_contest_ix_->value("");
}

// Populate logged fields choice
void contest_dialog::populate_algorithm() {
	w_algorithm_->clear();
	if (contest_data_->get_algorithms()->size()) {
		for (auto& it : *contest_data_->get_algorithms()) {
			w_algorithm_->add(it.c_str());
		}
	}
}

