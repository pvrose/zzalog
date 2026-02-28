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



class Fl_Output;
class Fl_Button;
class Fl_Check_Button;

//! This class displays the current state of the log.

//! It indicates whether it is dirty and provides basic controls.
class qso_log_info :
	public Fl_Group
{


public:
	//! Constructor.

	//! \param X horizontal position within host window
	//! \param Y vertical position with hosr window
	//! \param W width 
	//! \param H height
	//! \param L label
	qso_log_info(int X, int Y, int W, int H, const char* L = nullptr);
	//! Destructor.
	~qso_log_info();

	//! Inherited from Fl_Group::handle() to accept forecase so that keyboard F1 opens the userguide.
	virtual int handle(int event);

	//! Instantiate component widgets.
	void create_form(int X, int Y);
	//! Configure component widgets after data change.
	void enable_widgets();
	//! Configer component widgets after a ticker callback.
	void enable_timer_widgets();

	//! Callback every 100 ms.
	static void cb_ticker(void* v);

	//! Callback from "Save/QSO" button. 
	
	//! If value of button is true, then allows each change to a QSO record to be saved.
	static void cb_bn_enable(Fl_Widget* w, void* v);
	//! Callback from "Save!" button: forces the book_ to be saved.
	static void cb_bn_save(Fl_Widget* w, void* v);

protected:
	//! Output: textual description of status.
	Fl_Output* op_status_;
	//! Button: indicates status through colour.
	Fl_Button* bn_loadsave_;
	//! Check button: indicates and controls whether to save every QSO record cahnge.
	Fl_Check_Button* bn_save_enable_;
	//! Button: Force save.
	Fl_Button* bn_save_;

};