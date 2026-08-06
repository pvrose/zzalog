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
#include "qso_buttons.h"

#include "book.h"
#include "zc_drawing.h"
#include "qso_data.h"
#include "spec_data.h"
#include "zc_fltk.h"
#include "zc_icons.h"

#include <algorithm>
#include <list>
#include <map>

#include <FL/Fl_Button.H>
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Widget.H>

extern void open_html(const char* topic);

// Map showing the buttons available in each qso_data logging_state
std::map<qso_data::logging_state_t, std::list<qso_buttons::button_type> > button_map_ =
{
	{ qso_data::QSO_INACTIVE, {qso_buttons::ACTIVATE, qso_buttons::START_QSO, qso_buttons::ADD_QSO, 
		qso_buttons::EDIT_QSO, qso_buttons::COPY_QSO, qso_buttons::CLONE_QSO, qso_buttons::DELETE_QSO,
		qso_buttons::START_NET, qso_buttons::EDIT_NET, qso_buttons::BROWSE, qso_buttons::VIEW_QSO, 
		qso_buttons::ENTER_QUERY } },
	{ qso_data::QSO_PENDING, { qso_buttons::START_QSO, qso_buttons::ADD_QSO, qso_buttons::EDIT_QSO, qso_buttons::COPY_QSO, 
		qso_buttons::CLONE_QSO, qso_buttons::CANCEL_QSO, qso_buttons::SAVE_QSO, 
		qso_buttons::DELETE_QSO, qso_buttons::START_NET, qso_buttons::BROWSE, qso_buttons::VIEW_QSO } },
	{ qso_data::QSO_STARTED, { qso_buttons::SAVE_QSO, qso_buttons::SAVE_VIEW, qso_buttons::SAVE_NEW,
		qso_buttons::SAVE_CONTINUE, qso_buttons::CANCEL_QSO, 
		qso_buttons::START_NET, qso_buttons::UPDATE_STATION, qso_buttons::QRZ_COM } },
	{ qso_data::QSO_ENTER, { qso_buttons::SAVE_NEW, qso_buttons::SAVE_EXIT, 
		qso_buttons::SAVE_CONTINUE, qso_buttons::UPDATE_STATION, qso_buttons::CANCEL_QSO } },
	{ qso_data::QSO_EDIT, { qso_buttons::SAVE_EDIT, qso_buttons::SAVE_EXIT, 
		qso_buttons::SAVE_VIEW, qso_buttons::CANCEL_QSO, 
		qso_buttons::EDIT_NET, qso_buttons::NAV_FIRST,
		qso_buttons::NAV_PREV, qso_buttons::NAV_NEXT, qso_buttons::NAV_LAST,
		qso_buttons::UPDATE_CAT, qso_buttons::REPLACE_CAT, qso_buttons::UPDATE_STATION, 
		qso_buttons::QRZ_COM, qso_buttons::PARSE_QSO } },
	{ qso_data::QSO_VIEW, { qso_buttons::EDIT_QSO, qso_buttons::CANCEL_VIEW, qso_buttons::ACTIVATE,
	    qso_buttons::START_QSO, qso_buttons::ADD_QSO,
		qso_buttons::NAV_FIRST, qso_buttons::NAV_PREV, qso_buttons::NAV_NEXT, qso_buttons::NAV_LAST ,
		qso_buttons::QRZ_COM, qso_buttons::LOOK_ALL_TXT,
		qso_buttons::EDIT_NET, qso_buttons::START_NET, qso_buttons::BROWSE } },
	{ qso_data::QSO_BROWSE, { qso_buttons::EDIT_QSO, qso_buttons::CANCEL_QSO, qso_buttons::VIEW_QSO, 
	    qso_buttons::NAV_FIRST,
		qso_buttons::NAV_PREV, qso_buttons::NAV_NEXT, qso_buttons::NAV_LAST, qso_buttons::QRZ_COM } },
	{ qso_data::QUERY_MATCH, { qso_buttons::ADD_QUERY, qso_buttons::REJECT_QUERY, qso_buttons::MERGE_QUERY,
		qso_buttons::NAV_PREV, qso_buttons::NAV_NEXT, qso_buttons::LOOK_ALL_TXT, qso_buttons::QRZ_COM }},
	{ qso_data::QUERY_NEW, { qso_buttons::ADD_QUERY, qso_buttons::REJECT_QUERY, qso_buttons::FIND_QSO, 
		qso_buttons::LOOK_ALL_TXT, qso_buttons::QRZ_COM }},
	{ qso_data::QUERY_WSJTX, { qso_buttons::ADD_QUERY, qso_buttons::REJECT_QUERY } },
	{ qso_data::QUERY_DUPE, { qso_buttons::KEEP_DUPE_1, qso_buttons::MERGE_DUPE, qso_buttons::KEEP_DUPE_2,
		qso_buttons::KEEP_BOTH_DUPES }},
	{ qso_data::QUERY_SWL, { qso_buttons::ADD_QUERY, qso_buttons::REJECT_QUERY } },
	{ qso_data::QRZ_MERGE, { qso_buttons::MERGE_DONE }},
	{ qso_data::QRZ_COPY, { qso_buttons::MERGE_DONE }},
	{ qso_data::NET_STARTED, {qso_buttons::SAVE_NET, qso_buttons::SAVE_QSO, qso_buttons::SAVE_CONTINUE,
		qso_buttons::CANCEL_QSO, qso_buttons::CANCEL_NET,
		qso_buttons::NAV_FIRST, qso_buttons::NAV_PREV, qso_buttons::NAV_NEXT, qso_buttons::NAV_LAST,
		qso_buttons::ADD_NET_QSO, qso_buttons::QRZ_COM }},
	{ qso_data::NET_EDIT, { qso_buttons::SAVE_NET, qso_buttons::CANCEL_QSO, qso_buttons::CANCEL_NET, 
		qso_buttons::NAV_FIRST, qso_buttons::NAV_PREV, qso_buttons::NAV_NEXT, qso_buttons::NAV_LAST,
	    qso_buttons::ADD_NET_QSO, qso_buttons::UPDATE_CAT, qso_buttons::QRZ_COM }},
	{ qso_data::MANUAL_ENTRY, { qso_buttons::EXEC_QUERY, qso_buttons::IMPORT_QUERY, qso_buttons::CANCEL_QUERY, qso_buttons::LOOK_ALL_TXT }},
	{ qso_data::QSO_WSJTX, { qso_buttons::CANCEL_QSO }},
	{ qso_data::QSO_FLDIGI, { qso_buttons::CANCEL_QSO }},
	{ qso_data::TEST_PENDING, { qso_buttons::START_QSO }},
	{ qso_data::TEST_ACTIVE, { qso_buttons::SAVE_QSO, qso_buttons::CANCEL_QSO, qso_buttons::SAVE_NEW, qso_buttons::RESTART }}
};

// Map describing all the parameters for each button
std::map<qso_buttons::button_type, qso_buttons::button_action> action_map_ =
{
	{ qso_buttons::ACTIVATE, { "Activate", zc_icon_t::ICON_NONE, "Pre-load QSO fields based on logging mode", qso_buttons::cb_activate, 0, FL_ALT + 'z'}},
	{ qso_buttons::START_QSO, { "Start QSO", zc_icon_t::ICON_NONE, "Start a QSO in real-time", qso_buttons::cb_start, (void*)qso_data::QSO_ON_AIR, FL_ALT + 's'} },
	{ qso_buttons::EDIT_QSO, { "Edit QSO", zc_icon_t::ICON_NONE, "Edit the selected QSO", qso_buttons::cb_edit, 0, FL_ALT + 'e'}},
	{ qso_buttons::VIEW_QSO, { "View QSO", zc_icon_t::ICON_NONE, "View the selected QSO in entry view", qso_buttons::cb_bn_view_qso, 0 } },
	{ qso_buttons::ADD_QSO, { "Add QSO", zc_icon_t::ICON_NONE, "Create a new record (no initialisation)", qso_buttons::cb_start, (void*)qso_data::QSO_NONE, FL_ALT + 'a'}},
	{ qso_buttons::COPY_QSO, { "Copy QSO", zc_icon_t::ICON_NONE, "Create a new record (copy call and conditions)", qso_buttons::cb_start, (void*)qso_data::QSO_COPY_CALL }},
	{ qso_buttons::CLONE_QSO, { "Clone QSO", zc_icon_t::ICON_NONE, "Create a new record (copy conditions)", qso_buttons::cb_start, (void*)qso_data::QSO_COPY_CONDX }},
	{ qso_buttons::BROWSE, { "Browse Log", zc_icon_t::ICON_NONE, "Browse the log without editing", qso_buttons::cb_bn_browse, 0}} ,
	{ qso_buttons::SAVE_QSO, { "Log", zc_icon_t::ICON_NONE, "Log the QSO (set start time if not set) and quit", qso_buttons::cb_save, (void*)qso_buttons::SAVE_QSO, FL_ALT + 'l'}},
	{ qso_buttons::CANCEL_QSO, { "Quit QSO", zc_icon_t::ICON_NONE, "Cancel the current QSO entry", qso_buttons::cb_cancel, 0, FL_ALT + 'x'}},
	{ qso_buttons::DELETE_QSO, { "Delete QSO", zc_icon_t::ICON_NONE, "Delete the selected QSO", qso_buttons::cb_bn_delete_qso, 0, FL_ALT + FL_Delete } },
	{ qso_buttons::SAVE_EDIT, { "Log", zc_icon_t::ICON_NONE, "Copy changed record back to book", qso_buttons::cb_save, (void*)qso_buttons::SAVE_EDIT}},
	{ qso_buttons::SAVE_CONTINUE, { "Log && Edit", zc_icon_t::ICON_NONE, "Set TIME_OFF and allow continued edit", qso_buttons::cb_save, (void*)qso_buttons::SAVE_CONTINUE}},
	{ qso_buttons::SAVE_EXIT, { "Log && Exit", zc_icon_t::ICON_NONE, "Copy changed record and return to previous activity", qso_buttons::cb_save, (void*)qso_buttons::SAVE_EXIT }},
	{ qso_buttons::SAVE_VIEW, { "Log && View", zc_icon_t::ICON_NONE, "Copy changed record and allow view", qso_buttons::cb_save, (void*)qso_buttons::SAVE_VIEW }},
	{ qso_buttons::SAVE_NEW, { "Log && New", zc_icon_t::ICON_NONE, "Save QSO and start new QSO", qso_buttons::cb_save, (void*)qso_buttons::SAVE_NEW }},
	{ qso_buttons::CANCEL_VIEW, { "Cancel", zc_icon_t::ICON_NONE, "Cancel the current QSO view", qso_buttons::cb_cancel, 0 } },
    { qso_buttons::NAV_FIRST, { nullptr, zc_icon_t::ICON_FIRST, "Select first record in net or book", qso_buttons::cb_bn_navigate, (void*)NV_FIRST, FL_ALT + FL_SHIFT + FL_Left } },
	{ qso_buttons::NAV_PREV, { nullptr, zc_icon_t::ICON_PREVIOUS, "Select previous record in net or book", qso_buttons::cb_bn_navigate, (void*)NV_PREV, FL_ALT + FL_Left } },
	{ qso_buttons::NAV_NEXT, { nullptr, zc_icon_t::ICON_NEXT, "Select next record in net or book", qso_buttons::cb_bn_navigate, (void*)NV_NEXT, FL_ALT + FL_Right } },
	{ qso_buttons::NAV_LAST, { nullptr, zc_icon_t::ICON_LAST, "Select last record in net or book", qso_buttons::cb_bn_navigate, (void*)NV_LAST, FL_ALT + FL_SHIFT + FL_Right } },
	{ qso_buttons::ADD_QUERY, { "Add QSO", zc_icon_t::ICON_NONE, "Add queried QSO to log", qso_buttons::cb_bn_add_query, 0 }},
	{ qso_buttons::REJECT_QUERY, {"Reject QSO", zc_icon_t::ICON_NONE, "Do not add queried QSO to log", qso_buttons::cb_bn_reject_query, 0} },
	{ qso_buttons::MERGE_QUERY, {"Merge QSO", zc_icon_t::ICON_NONE, "Merge query with logged QSO", qso_buttons::cb_bn_merge_query, 0 } },
	{ qso_buttons::FIND_QSO, { nullptr, zc_icon_t::ICON_SEARCH, "Display possible match", qso_buttons::cb_bn_find_match, 0}},
	{ qso_buttons::KEEP_DUPE_1, { "Keep 1", zc_icon_t::ICON_NONE, "Keep first QSO and delete second", qso_buttons::cb_bn_dupe, (void*)qso_data::DF_1}},
	{ qso_buttons::KEEP_DUPE_2, { "Keep 2", zc_icon_t::ICON_NONE, "Keep second QSO and delete first", qso_buttons::cb_bn_dupe, (void*)qso_data::DF_2}},
	{ qso_buttons::MERGE_DUPE, { "Merge", zc_icon_t::ICON_NONE, "Merge the two records", qso_buttons::cb_bn_dupe, (void*)qso_data::DF_MERGE}},
	{ qso_buttons::KEEP_BOTH_DUPES, { "Keep 1 && 2", zc_icon_t::ICON_NONE, "Keep both records", qso_buttons::cb_bn_dupe, (void*)qso_data::DF_BOTH}},
	{ qso_buttons::MERGE_DONE, { "Done", zc_icon_t::ICON_NONE, "Save changes", qso_buttons::cb_bn_save_merge, 0} },
	{ qso_buttons::LOOK_ALL_TXT, { "ALL.TXT", zc_icon_t::ICON_SEARCH, "Look in WSJT-X ALL.TXT file for possible contact", qso_buttons::cb_bn_all_txt, 0 } },
	{ qso_buttons::START_NET, { "Start Net", zc_icon_t::ICON_NONE, "Start a QSO with more than one other station", qso_buttons::cb_bn_start_net, 0, FL_ALT + FL_SHIFT + 's'}},
	{ qso_buttons::EDIT_NET, { "Edit Net", zc_icon_t::ICON_NONE, "Open all calls that overlap", qso_buttons::cb_bn_add_net, 0}},
	{ qso_buttons::SAVE_NET, { "Log Net", zc_icon_t::ICON_NONE, "Log all the QSOs and quit", qso_buttons::cb_bn_save_all, 0, FL_ALT + FL_SHIFT + 'l'}},
	{ qso_buttons::CANCEL_NET, { "Quit Net", zc_icon_t::ICON_NONE, "Cancel all QSOs", qso_buttons::cb_bn_cancel_all, 0, FL_ALT + FL_SHIFT + 'x'}},
	{ qso_buttons::ADD_NET_QSO, { "Add Call", zc_icon_t::ICON_NONE, "Add a QSO with this call to the net", qso_buttons::cb_bn_add_net, 0, FL_ALT + FL_SHIFT + 'a'}},
	{ qso_buttons::ENTER_QUERY, { "Query", zc_icon_t::ICON_NONE, "Enter QSO details for search query", qso_buttons::cb_bn_query_entry, 0 }},
	{ qso_buttons::EXEC_QUERY, { "Check", zc_icon_t::ICON_NONE, "Execute query", qso_buttons::cb_bn_execute_query, 0 }},
	{ qso_buttons::CANCEL_QUERY, { "Cancel Query", zc_icon_t::ICON_NONE, "Cancel query", qso_buttons::cb_bn_cancel_query, 0 }},
	{ qso_buttons::IMPORT_QUERY, { "Test Import", zc_icon_t::ICON_NONE, "Test import query", qso_buttons::cb_bn_import_query, 0 }},
	{ qso_buttons::QRZ_COM, { "QRZ.com", zc_icon_t::ICON_SEARCH, "Display details in QRZ.com", qso_buttons::cb_bn_qrz_com, 0}},
	{ qso_buttons::UPDATE_CAT, { "Update CAT", zc_icon_t::ICON_NONE, "Use CAT info where current QSO has no value", qso_buttons::cb_bn_update_cat, (void*)false }},
	{ qso_buttons::REPLACE_CAT, { "Replace CAT", zc_icon_t::ICON_NONE, "Use current CAT info", qso_buttons::cb_bn_update_cat, (void*)true }},
	{ qso_buttons::RESTART, { "Restart", zc_icon_t::ICON_NONE, "Ditch current QSO and start anew", qso_buttons::cb_bn_restart, 0 }},
	{ qso_buttons::PARSE_QSO, { "Parse QSO", zc_icon_t::ICON_NONE, "Add DXCC, CQ, etc details to QSO", qso_buttons::cb_bn_parse_qso, 0 }},
	{ qso_buttons::UPDATE_STATION, { "U/d Station", zc_icon_t::ICON_NONE, "Add QTH, Operator and station callsigns to QSO", qso_buttons::cb_bn_update_station, 0 }},
};

// Constructor
qso_buttons::qso_buttons(int X, int Y, int W, int H, const char* L) :
	Fl_Group(X, Y, W, H, L),
	qso_data_((qso_data*)parent())
{
	tooltip("Control buttons");
	create_form(X, Y);
	enable_widgets();
}

// Destructor
qso_buttons::~qso_buttons() {
}

// Handle
int qso_buttons::handle(int event) {
	int result = Fl_Group::handle(event);
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
			open_html("qso_buttons.html");
			return true;
		}
		break;
	}
	return result;
}

// Handle shortcut
bool qso_buttons::handle_shortcut(int shortcut) {
	if (shortcut_map_.find(shortcut) != shortcut_map_.end()) {
		button_type bn = shortcut_map_.at(shortcut);
		if (action_map_.find(bn) != action_map_.end()) {
			const button_action& action = action_map_.at(bn);
			if (action.callback != nullptr) {
				action.callback(this, action.userdata);
				return true;
			}
		}
	}
	return false;
}

// Create all the buttons
void qso_buttons::create_form(int X, int Y) {
	int curr_x = X;
	int curr_y = Y;

	label("Controls");
	labelfont(FL_BOLD);
	labelsize(FL_NORMAL_SIZE + 2);
	align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
	box(FL_BORDER_BOX);

	curr_x += GAP;
	curr_y += HTEXT;
	int max_x = curr_x;

	const int NUMBER_PER_ROW = 10;
	// Create the maximum number of buttons (MAX_ACTIONS) in rows of (NUMBER_PER_ROW)
	for (int ix = 0; ix < MAX_ACTIONS; ix++) {
		bn_action_[ix] = new Fl_Button(curr_x, curr_y, WBUTTON, HBUTTON, "");
		if ((ix + 1) % NUMBER_PER_ROW == 0 && ix < MAX_ACTIONS) {
			curr_x += WBUTTON;
			max_x = std::max<int>(max_x, curr_x);
			curr_x = X + GAP;
			curr_y += HBUTTON;
		}
		else {
			curr_x += WBUTTON;
			max_x = std::max<int>(max_x, curr_x);
		}
	}
	if (MAX_ACTIONS % NUMBER_PER_ROW != 0) {
		curr_y += HBUTTON;
	}

	max_x += GAP;
	curr_y += GAP;
	resizable(nullptr);
	size(max_x - X, curr_y - Y);
	end();
}

// Configure the buttons according to the logging_state
void qso_buttons::enable_widgets() {
	int ix = 0;
	shortcut_map_.clear();
	if (button_map_.find(qso_data_->logging_state()) != button_map_.end()) {
		// If we have a button map for the state use it - else deactivate all buttons
		const std::list<button_type>& buttons = button_map_.at(qso_data_->logging_state());
		// Activate the buttons we need and set their parameters
		for (auto bn = buttons.begin(); bn != buttons.end() && ix < MAX_ACTIONS; bn++, ix++) {
			const button_action& action = action_map_.at(*bn);
			bn_action_[ix]->label(action.label);
			zc_add_icon_to_widget(bn_action_[ix], action.icon);
			bn_action_[ix]->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
			bn_action_[ix]->tooltip(action.tooltip);
			//bn_action_[ix]->color(action.colour);
			//bn_action_[ix]->labelcolor(fl_contrast(FL_FOREGROUND_COLOR, action.colour));
			bn_action_[ix]->callback(action.callback, action.userdata);
			switch(*bn) {
				case NAV_FIRST:
				case NAV_LAST:
				case NAV_NEXT:
				case NAV_PREV:
				{
					if (qso_data_->can_navigate((navigate_t)(intptr_t)action.userdata)) {
						bn_action_[ix]->activate();
					} else {
						bn_action_[ix]->deactivate();
					}
					break;
				}
				default:
					bn_action_[ix]->activate();
			}
			// TODO is this a good thing to do?
//			bn_action_[ix]->shortcut(action.shortcut);
			if (action.shortcut != 0) {
				shortcut_map_[action.shortcut] = *bn;
			}
		}
	}
	// Deactivate any remaining buttons
	for (; ix < MAX_ACTIONS; ix++) {
		bn_action_[ix]->label("");
		bn_action_[ix]->tooltip("");
		bn_action_[ix]->color(FL_BACKGROUND_COLOR);
		bn_action_[ix]->callback((Fl_Callback*)nullptr);
		bn_action_[ix]->deactivate();
	}

}

// Deactivate all buttons to prevent double clicking
void qso_buttons::disable_widgets() {
	for (int ix = 0; ix < MAX_ACTIONS; ix++) {
		bn_action_[ix]->deactivate();
	}
}

// Activate- Go from qso_data::QSO_INACTIVE to qso_data::QSO_PENDING
// v is not used
void qso_buttons::cb_activate(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_VIEW:
		that->qso_data_->action_cancel_edit();
		[[fallthrough]];
		// Drop through
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_activate(qso_data::QSO_ON_AIR);
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Start QSO - transition from qso_data::QSO_INACTIVE->qso_data::QSO_PENDING->qso_data::QSO_STARTED
// v is not used
void qso_buttons::cb_start(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	qso_data::qso_init_t mode = (qso_data::qso_init_t)(intptr_t)v;
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_activate(mode);
		[[fallthrough]];
		// Fall into next state
	case qso_data::TEST_PENDING:
	case qso_data::QSO_PENDING:
		that->qso_data_->action_start(mode);
		break;
	case qso_data::QSO_VIEW:
		that->qso_data_->action_cancel_edit();
		that->qso_data_->action_activate(mode);
		that->qso_data_->action_start(mode);
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Save QSO - transition through qso_data::QSO_PENDING->qso_data::QSO_STARTED->qso_data::QSO_INACTIVE saving QSO
// v is used in QSO_EDIT state to indicate type of save.
void qso_buttons::cb_save(Fl_Widget* w, void* v) {
	qso_data* data = zc::ancestor_view<qso_data>(w);
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	button_type edit_button = (button_type)(intptr_t)v;
	qso_num_t current = data->current_number();
	if (that) that->disable_widgets();
	switch (data->logging_state()) {
	case qso_data::QSO_PENDING:
		// If in pending then we can assume it's started
		data->action_start(qso_data::QSO_AS_WAS);
		// Two routes - QSO entry
		[[fallthrough]];
	case qso_data::QSO_STARTED:
	case qso_data::TEST_ACTIVE:
		// Realtime entry - do not start another
		if (!data->action_save(edit_button == SAVE_CONTINUE)) break;
		switch (edit_button) {
			case SAVE_QSO: {
				data->action_activate(qso_data::QSO_AS_WAS);
				break;
			}
			case SAVE_NEW: {
				data->action_activate(qso_data::QSO_AS_WAS);
				data->action_start(qso_data::QSO_AS_WAS);
				break;
			}
			case SAVE_VIEW: {
				data->action_activate(qso_data::QSO_AS_WAS);
				data->action_view(current);
				break;
			}
			case SAVE_CONTINUE: {
				data->action_activate(qso_data::QSO_AS_WAS);
				data->action_view(current);
				data->action_edit();
				break;
			}
			default:
				break;
		}
		break;
	case qso_data::QSO_ENTER:
		// Batch entry - start another entry
		if(!data->action_save(edit_button == SAVE_CONTINUE)) break;
		switch(edit_button) {
			case SAVE_NEW: {
				data->action_activate(qso_data::QSO_NONE);
				data->action_start(qso_data::QSO_NONE);
				break;
			}
			case SAVE_EXIT: {
				data->action_deactivate();
				break;
			}
			default:
				break;
		}
		break;
		// QSO editing
	case qso_data::QSO_EDIT:
		data->action_save_edit();
		switch (edit_button) {
		case SAVE_EXIT:
			switch (data->edit_return_state_) {
			case qso_data::QSO_INACTIVE:
				break;
			case qso_data::QSO_PENDING:
				data->action_activate(qso_data::QSO_AS_WAS);
				break;
			case qso_data::MANUAL_ENTRY:
				data->action_query_entry();
				break;
			default:
				break;
			}
			break;
		case SAVE_EDIT:
			data->action_edit();
			break;
		case SAVE_VIEW:
			data->action_view();
			break;
		default:
			break;
		}
		break;
	case qso_data::NET_STARTED:
		data->action_save(edit_button == SAVE_CONTINUE);
		break;
	default:
		break;
	}
	if (that) that->enable_widgets();
}

// Cancel QSO - delete QSO; clear fields
// v is not used
void qso_buttons::cb_cancel(Fl_Widget* w, void* v) {
	qso_data* data = zc::ancestor_view<qso_data>(w);
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	if (that) that->disable_widgets();
	switch (data->logging_state()) {
	case qso_data::QSO_PENDING:
		data->action_deactivate();
		break;
	case qso_data::QSO_STARTED:
	case qso_data::QSO_ENTER:
	case qso_data::TEST_ACTIVE:
		data->action_cancel();
		data->action_activate(qso_data::QSO_AS_WAS);
		break;
	case qso_data::QSO_EDIT:
	case qso_data::QSO_VIEW:
		data->action_cancel_edit();
		break;
	case qso_data::QSO_BROWSE:
		data->action_cancel_browse();
		break;
	case qso_data::NET_STARTED:
		data->action_cancel();
		break;
	case qso_data::NET_EDIT:
		data->action_cancel_net_edit();
		break;
	case qso_data::QSO_WSJTX:
	case qso_data::QSO_FLDIGI:
		data->action_cancel_modem();
		break;
	default:
		break;
	}
	if (that) that->enable_widgets();
}

// Edit QSO - transition to qso_data::QSO_EDIT
// v is not used
void qso_buttons::cb_edit(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	that->qso_data_->action_remember_state();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_edit();
		break;
	case qso_data::QSO_PENDING:
		that->qso_data_->action_deactivate();
		that->qso_data_->action_edit();
		break;
	case qso_data::QSO_BROWSE:
		that->qso_data_->action_cancel_browse();
		that->qso_data_->action_edit();
		break;
	case qso_data::QSO_VIEW:
		that->qso_data_->action_edit();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// View QSO - transition to qso_data::QSO_VIEW
// v is not used
void qso_buttons::cb_bn_view_qso(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	that->qso_data_->action_remember_state();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_view();
		break;
	case qso_data::QSO_PENDING:
		that->qso_data_->action_deactivate();
		that->qso_data_->action_view();
		break;
	case qso_data::QSO_BROWSE:
		that->qso_data_->action_cancel_browse();
		that->qso_data_->action_view();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// CAllback - navigate buttons
// v - direction
void qso_buttons::cb_bn_navigate(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	navigate_t target = (navigate_t)(intptr_t)v;
	that->qso_data_->action_navigate(target);
	that->enable_widgets();
}

// Callback - browse
// v is not used
void qso_buttons::cb_bn_browse(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_PENDING:
	case qso_data::QSO_VIEW:
		that->qso_data_->action_deactivate();
		// Drop through
		[[fallthrough]];
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_browse();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback - add query record
// v is not used
void qso_buttons::cb_bn_add_query(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QUERY_MATCH:
	case qso_data::QUERY_NEW:
	case qso_data::QUERY_WSJTX:
	case qso_data::QUERY_SWL:
		that->qso_data_->action_add_query();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback - add query record
// v is not used
void qso_buttons::cb_bn_reject_query(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QUERY_MATCH:
	case qso_data::QUERY_NEW:
	case qso_data::QUERY_WSJTX:
	case qso_data::QUERY_SWL:
		that->qso_data_->action_reject_query();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback - add query record
// v is not used
void qso_buttons::cb_bn_merge_query(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QUERY_MATCH:
	case qso_data::QUERY_NEW:
		that->qso_data_->action_merge_query();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback - add query record
// v is not used
void qso_buttons::cb_bn_find_match(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QUERY_NEW:
		that->qso_data_->action_find_match();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback - dupe action
// v is not used
void qso_buttons::cb_bn_dupe(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	qso_data::dupe_flags action = (qso_data::dupe_flags)(intptr_t)v;
	switch (that->qso_data_->logging_state()) {
	case qso_data::QUERY_DUPE:
		that->qso_data_->action_handle_dupe(action);
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback QRZ merge action
// v is not used
void qso_buttons::cb_bn_save_merge(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QRZ_MERGE:
	case qso_data::QRZ_COPY:
		that->qso_data_->action_save_merge();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Callback Find QSO in WSJT-X
// v is not used
void qso_buttons::cb_bn_all_txt(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QUERY_NEW:
	case qso_data::QUERY_MATCH:
	case qso_data::QSO_VIEW:
	case qso_data::MANUAL_ENTRY:
		that->qso_data_->action_look_all_txt();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Save all qsos
// v is not used
void qso_buttons::cb_bn_save_all(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::NET_STARTED:
	case qso_data::NET_EDIT:
		that->qso_data_->action_save_net_all();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Cancel all QSOs
// v is not used
void qso_buttons::cb_bn_cancel_all(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::NET_STARTED:
	case qso_data::NET_EDIT:
		that->qso_data_->action_cancel_net_all();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Add a QSO to the net
// v is not used
void qso_buttons::cb_bn_add_net(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_VIEW:
		that->qso_data_->action_cancel_edit();
		[[fallthrough]];
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_edit();
		[[fallthrough]];
		// NB state may now be QSO_EDIT _or_ QSO_STARTED
	case qso_data::QSO_EDIT:
		that->qso_data_->action_create_net();
		break;
	case qso_data::NET_STARTED:
	case qso_data::NET_EDIT:
		that->qso_data_->action_add_net_qso();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Start a net
// v is not used
void qso_buttons::cb_bn_start_net(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_VIEW:
		that->qso_data_->action_cancel_edit();
		[[fallthrough]];
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_activate(qso_data::QSO_ON_AIR);
		[[fallthrough]];
	case qso_data::QSO_PENDING:
		that->qso_data_->action_start(qso_data::QSO_ON_AIR);
		[[fallthrough]];
	case qso_data::QSO_STARTED:
		that->qso_data_->action_create_net();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Delete the current QSO
// v is not used
void qso_buttons::cb_bn_delete_qso(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_INACTIVE:
	case qso_data::QSO_PENDING:
		that->qso_data_->action_delete_qso();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Open a qso_query to define search criteria
// v is not used
void qso_buttons::cb_bn_query_entry(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_INACTIVE:
		that->qso_data_->action_query_entry();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Open a qso_query to define search criteria
// v is not used
void qso_buttons::cb_bn_execute_query(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	that->qso_data_->action_remember_state();
	switch (that->qso_data_->logging_state()) {
	case qso_data::MANUAL_ENTRY:
		that->qso_data_->action_exec_query();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Open a qso_query to define search criteria
// v is not used
void qso_buttons::cb_bn_cancel_query(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::MANUAL_ENTRY:
		that->qso_data_->action_cancel_query();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Open a qso_query to define search criteria
// v is not used
void qso_buttons::cb_bn_import_query(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::MANUAL_ENTRY:
		that->qso_data_->action_import_query();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Open a browser with QRZ.com
// v is not used
void qso_buttons::cb_bn_qrz_com(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::QSO_PENDING:
	case qso_data::TEST_PENDING:
	case qso_data::QSO_STARTED:
	case qso_data::QSO_EDIT:
	case qso_data::QSO_VIEW:
	case qso_data::QSO_BROWSE:
	case qso_data::NET_EDIT:
	case qso_data::NET_STARTED:
	case qso_data::QUERY_MATCH:
	case qso_data::QUERY_NEW:
		that->qso_data_->action_qrz_com();
		break;
	default:
		break;
	}
	that->enable_widgets();
}

// Update QSO with CAT data
// v is not used
void qso_buttons::cb_bn_update_cat(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	bool clear = (bool)(intptr_t)v;
	switch(that->qso_data_->logging_state()) {
		case qso_data::QSO_EDIT:
		case qso_data::NET_EDIT: {
			that->qso_data_->action_update_cat(clear);
			break;
		default:
			break;
		}
	}
	that->enable_widgets();
}

// Cancel current QSO and restart
void qso_buttons::cb_bn_restart(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch (that->qso_data_->logging_state()) {
	case qso_data::TEST_ACTIVE: {
		that->qso_data_->action_cancel();
		that->qso_data_->action_activate(qso_data::QSO_ON_AIR);
		that->qso_data_->action_start(qso_data::QSO_ON_AIR);
		break;
	}
	default:
		break;
	}
	that->enable_widgets();
}

// Parse the QSO
void qso_buttons::cb_bn_parse_qso(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	that->disable_widgets();
	switch(that->qso_data_->logging_state()) {
	case qso_data::QSO_EDIT: {
		that->qso_data_->action_parse_qso();
		break;
	}
	default:
		break;
	}
	that->enable_widgets();
}

// Update station details
void qso_buttons::cb_bn_update_station(Fl_Widget* w, void* v) {
	qso_buttons* that = zc::ancestor_view<qso_buttons>(w);
	switch(that->qso_data_->logging_state()) {
	case qso_data::QSO_STARTED:
	case qso_data::QSO_EDIT:
	case qso_data::QSO_ENTER:
		that->qso_data_->update_station_fields();
		break;
	default:
		break;
	}
	that->enable_widgets();
}