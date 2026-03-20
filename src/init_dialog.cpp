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
#include "init_dialog.h"

#include "stn_data.h"
#include "stn_dialog.h"

#include "zc_drawing.h"
#include "zc_utils.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Widget.H>

//! Constructor - sizes and labels itself
init_dialog::init_dialog(int X, int Y, int W, int H, const char* L) :
	Fl_Group(X, Y, W, H, L)
{
	create_form();
	load_values();
}


//! Destructor
init_dialog::~init_dialog() {
}

//! Instantiate the component widgets
void init_dialog::create_form() {
	int cx = x() + GAP + WLABEL;
	int cy = y() + GAP;
	const int cw = WLABEL + WSMEDIT;

	Fl_Group* g_radio = new Fl_Group(cx, cy, WBUTTON, 2 * HBUTTON);
	g_radio->box(FL_FLAT_BOX);

	bn_club_ = new Fl_Radio_Round_Button(cx, cy, HBUTTON, HBUTTON, "Club");
	bn_club_->align(FL_ALIGN_LEFT);
	bn_club_->callback(cb_type, (void*)(intptr_t)CLUB);
	bn_club_->tooltip("Select this if the log is for a club station");
	
	cy += HBUTTON;
	bn_indiv_ = new Fl_Radio_Round_Button(cx, cy, HBUTTON, HBUTTON, "Individual");
	bn_indiv_->align(FL_ALIGN_LEFT);
	bn_indiv_->callback(cb_type, (void*)(intptr_t)INDIVIDUAL);
	bn_indiv_->tooltip("Selecet this if teh log is for an individual licensee");

	g_radio->end();
	cy += HBUTTON;
	ip_call_ = new Fl_Input(cx, cy, WSMEDIT, HBUTTON, "Callsign");
	ip_call_->align(FL_ALIGN_LEFT);
	ip_call_->tooltip("Enter the station callsign");

	cy += HBUTTON;
	ip_club_ = new Fl_Input(cx, cy, WSMEDIT, HBUTTON, "Club name");
	ip_club_->align(FL_ALIGN_LEFT);
	ip_club_->tooltip("Input the club name");

	cy += HBUTTON + GAP;
	ip_location_ = new Fl_Input(cx, cy, WSMEDIT, HBUTTON, "Station ID");
	ip_location_->align(FL_ALIGN_LEFT);
	ip_location_->tooltip("Enter the default station identifier (eg \"Main Station\"");
	ip_location_->value("Main Station");

	cy += HBUTTON;
	ip_name_ = new Fl_Input(cx, cy, WSMEDIT, HBUTTON, "Operator");
	ip_name_->align(FL_ALIGN_LEFT);
	ip_name_->tooltip("Enter the default operator's name");

	cy += HBUTTON + GAP;
	cx = GAP + cw - WBUTTON;
	bn_accept_ = new Fl_Button(cx, cy, WBUTTON, HBUTTON, "Accept");
	bn_accept_->callback(cb_accept, nullptr);
	bn_accept_->tooltip("Click to accept the entered data");

	int ch = cy + HBUTTON + GAP;
	resizable(nullptr);
	size(GAP + cw + GAP, ch);

	end();
	show();
}

//! Copy from station_defauts_

void init_dialog::load_values() {
	stn_default defaults = stn_data_->defaults();
	if (defaults.type == NOT_USED) {
		bn_club_->value(false);
		bn_indiv_->value(false);
		ip_club_->value("");
		ip_call_->value("");
		ip_location_->value("Main station");
		ip_name_->value("");
	}
	else {
		switch (defaults.type) {
		case CLUB:
			bn_club_->value(true);
			bn_indiv_->value(false);
			break;
		case INDIVIDUAL:
			bn_club_->value(false);
			bn_indiv_->value(true);
			break;
		default:
			break;
		}
		ip_call_->value(defaults.callsign.c_str());
		ip_location_->value(defaults.location.c_str());
		ip_name_->value(defaults.name.c_str());
		ip_club_->value(defaults.club_name.c_str());
	}
}


//! Callback when "Accept" button clicked
void init_dialog::cb_accept(Fl_Widget* w, void* v) {
	init_dialog* that = zc::ancestor_view<init_dialog>(w);
	stn_default defaults = stn_data_->defaults();
	defaults.callsign = zc::to_upper(that->ip_call_->value());
	defaults.location = that->ip_location_->value();
	defaults.name = that->ip_name_->value();
	defaults.club_name = that->ip_club_->value();
	std::string message;
	switch(defaults.type) {
	case CLUB:
        if (defaults.callsign.length() &&
		    defaults.club_name.length() &&
		    defaults.location.length())
		    that->valid_data_ = true;
		else {
			that->valid_data_ = false;
			message = "Please supply club callsign, club name and location identifier";
		}
		break;
	case INDIVIDUAL:
		if (defaults.callsign.length() &&
			defaults.name.length() &&
		    defaults.location.length())
		    that->valid_data_ = true;
		else {
			that->valid_data_ = false;
			message = "Please supply callsign, operator name and location identifier";
		}
		break;
	}
	if (that->valid_data_) {
		stn_data_->set_defaults(defaults);
		stn_dialog* dlg = zc::ancestor_view<stn_dialog>(that);
		dlg->enable_widgets();
		dlg->set_tab(stn_dialog::QTH, defaults.location, "Set initial values.");
	} else {
		stn_data_->set_defaults(defaults);
		stn_dialog* dlg = zc::ancestor_view<stn_dialog>(that);
		dlg->enable_widgets();
		dlg->set_tab(stn_dialog::DEFAULTS, "", message);
	}
}

//! Callback when radio button is selected

//! \param w: widget clicked
//! \param v: object of type stn_type indicates the new station type
void init_dialog::cb_type(Fl_Widget* w, void* v) {
	init_dialog* that = zc::ancestor_view<init_dialog>(w);
	stn_data_->set_type((stn_type)(intptr_t)v);
	that->enable_widgets();
}

//! SAvevalues for club use
void init_dialog::enable_widgets() {
	stn_default defaults = stn_data_->defaults();
	// Set default station ID.
	if (defaults.location.length() == 0) defaults.location = "Main station";
	switch (defaults.type) {
	case NOT_USED:
		bn_club_->value(false);
		bn_indiv_->value(false);
		bn_club_->labelcolor(FL_FOREGROUND_COLOR);
		bn_indiv_->labelcolor(FL_FOREGROUND_COLOR);
		ip_club_->deactivate();
		ip_club_->value("");
		ip_call_->deactivate();
		ip_call_->value("");
		ip_name_->deactivate();
		ip_name_->value("");
		ip_location_->deactivate();
		ip_location_->value("");
		break;
	case CLUB:
		bn_club_->value(true);
		bn_indiv_->value(false);
		ip_club_->activate();
		ip_club_->value(defaults.club_name.c_str());
		if (defaults.club_name.length() == 0) {
			ip_club_->labelcolor(FL_RED);
		} else {
			ip_club_->labelcolor(FL_FOREGROUND_COLOR);
		}
		ip_call_->activate();
		ip_call_->value(defaults.callsign.c_str());
		if (defaults.callsign.length() == 0) {
			ip_call_->labelcolor(FL_RED);
		} else {
			ip_call_->labelcolor(FL_FOREGROUND_COLOR);
		}
		ip_name_->deactivate();
		ip_name_->value("");
		ip_location_->activate();
		ip_location_->value(defaults.location.c_str());
		if (defaults.location.length() == 0) {
			ip_location_->labelcolor(FL_RED);
		} else {
			ip_location_->labelcolor(FL_FOREGROUND_COLOR);
		}
		break;
	case INDIVIDUAL:
		bn_club_->value(false);
		bn_indiv_->value(true);
		ip_club_->deactivate();
		ip_club_->value("");
		ip_call_->activate();
		ip_call_->value(defaults.callsign.c_str());
		if (defaults.callsign.length() == 0) {
			ip_call_->labelcolor(FL_RED);
		} else {
			ip_call_->labelcolor(FL_FOREGROUND_COLOR);
		}
		ip_name_->activate();
		ip_name_->value(defaults.name.c_str());
		if (defaults.name.length() == 0) {
			ip_name_->labelcolor(FL_RED);
		} else {
			ip_name_->labelcolor(FL_FOREGROUND_COLOR);
		}
		ip_location_->activate();
		ip_location_->value(defaults.location.c_str());
		if (defaults.location.length() == 0) {
			ip_location_->labelcolor(FL_RED);
		} else {
			ip_location_->labelcolor(FL_FOREGROUND_COLOR);
		}
		break;
	}
}

bool init_dialog::valid_data() {
	return valid_data_;
}