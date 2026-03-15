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
#pragma once

#include <FL/Fl_Group.H>

#include <string>
#include <vector>

class Fl_Box;
class Fl_Button;
class Fl_Check_Button;
class Fl_Group;
class Fl_Input;
class Fl_Input_Choice;
class Fl_Multiline_Input;
class zc_tabs_nonav;
class Fl_Widget;

class init_dialog;
class record;
class stn_call_dlg;
class stn_oper_dlg;
class stn_qth_dlg;
struct qth_info_t;
struct oper_info_t;


 
//! This class provides a dialog for manging station locations, operators and callsigns
class stn_dialog :
    public Fl_Group
{
public:
	//! Constructor.

	//! \param X horizontal position within host window
	//! \param Y vertical position with hosr window
	//! \param W width 
	//! \param H height
	//! \param L label
	stn_dialog(int X, int Y, int W, int H, const char* L = nullptr);
	//! Destructor.
    ~stn_dialog();

	//! Inheritedfrom page_dialog allowing keyboard F1 to open userguide.
	virtual int handle(int event);

	// inherited methods
	// Standard methods - need to be written for each
	//! Load values from stn_data.
	virtual void load_values();
	//! Instantiate component widgwts
	virtual void create_form(int X, int Y);
	//! Save values and update other widgets with changes.
	virtual void save_values();
	//! Configure component widgets after data changes,
	virtual void enable_widgets();

	//! Callback from changing tabs.
	static void cb_tab(Fl_Widget* w, void* v);
	//! Callback to close window from Done button
	static void cb_done(Fl_Widget* w, void* v);

	//! type for the single_tab 
	enum tab_type : char {
		DEFAULTS,      //!< Initial defaults dialog
		QTH,           //!< Location data editor.
		OPERATOR,      //!< Operator data editor.
		CALLSIGN       //!< Callsign data editor.
	};

	//! Set the active tab \p t and set its default value \p id.
	void set_tab(tab_type t, std::string id, std::string message);

	//! Update 
	void update();

protected:

	//! Current QSO record.
	record* qso_;

	Fl_Box* message_;           //!< Message 
	Fl_Button* bn_done_ = nullptr;   //!< Done button
	zc_tabs_nonav* tabs_;             //!< Tabs:
	init_dialog* g_defs_;       //!< Tab: Defaults dislog 
	stn_qth_dlg* g_qth_;        //!< Tab: Location editing
	stn_oper_dlg* g_oper_;      //!< Tab: Operator editing.
	stn_call_dlg* g_call_;      //!< Tab: Callsign editing.
};

#include <FL/Fl_Double_Window.H>

//! Window to hold stn_dialog
class stn_window : public Fl_Double_Window {

	//! Internal dialog
	stn_dialog* dlg_;

	//! Close callback
	static void cb_hide(Fl_Widget* w, void* v) {
		((stn_window*)w)->dlg_->update();
		default_callback((stn_window*)w, v);
	}

public:
	
	//! Constructor - sets own dimensions and label
	stn_window() :
		Fl_Double_Window(600, 400, "Station details") {
		dlg_ = new stn_dialog(0, 0, w(), h());
		show();
	}

	//! Destructor
	~stn_window() {
		delete dlg_;
	}

	//! Set the active tab \p t and set its default value \p id.
	void set_tab(stn_dialog::tab_type t, std::string id, std::string msg) {
		dlg_->set_tab(t, id, msg);
		show();
	}


};

extern stn_window* stn_window_;
