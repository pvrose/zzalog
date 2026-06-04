/*
	Copyright 2026, Philip Rose, GM3ZZA

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

#include "page_dialog.h"

// FLTK classes
class Fl_Check_Button;
class Fl_Group;
class Fl_Int_Input;
class Fl_Output;
// ZZACOMMON classes
class zc_filename_input;
class zc_settings;

//! \brief Class to handle user changes to the backup and mirror configuration.
//! This will be used a a tab in the config window.
class bumr_dialog :
	public page_dialog
{
public:

	//! \brief Constructor.
	//! \param X horizontal position within host window
	//! \param Y vertical position with hosr window
	//! \param W width 
	//! \param H height
	//! \param L label
	bumr_dialog(int X, int Y, int W, int H, const char* L = nullptr);

	//! Destructor
	virtual ~bumr_dialog();

	//! Inherited from page_dialog allows keyboard F1 to open userguide
	virtual int handle(int event);

	// inherited methods
	// Standard methods - need to be written for each
	// 
	//! Load values from settings and internal databases loaded from files
	virtual void load_values();

	//! Instantiate component widgets.
	virtual void create_form(int X, int Y);

	//! Save values back to settings.
	virtual void save_values();

	//! Configure component widgets after data change.
	virtual void enable_widgets();

	//! Callback for changes to mirror enabled setting.
	static void cb_mr_enable(Fl_Widget* w, void* v);

	Fl_Output* op_mirror_use_;         //!< Displays mirror use.
	Fl_Output* op_logname_;            //!< Displays current log filename.
	Fl_Output* op_targetname_;           //!< Displays target (target) log filename.
	// widgets - backup configuration
	Fl_Group* grp_backup_;             //!< Widgets controlling backup behaviour.
	zc_filename_input* ip_bu_path_;    //!< Backup directory path.
	Fl_Int_Input* ip_bu_depth_;        //!< Number of backups maintained.

	// widgets - mirror configuration
	Fl_Group* grp_mirror_;             //!< Widgets controlling mirror behaviour.
	Fl_Check_Button* bn_mr_enable_;    //!< Mirroring enabled.
	zc_filename_input* ip_mr_path_;    //!< Mirror directory path.
	Fl_Check_Button* bn_mr_working_;   //!< Use the mirror file as working data.


};