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

#include <map>
#include <string>

#include "zc_tabs_nonav.h"



class rig_if;

//! This class presents a set of tabs indicating the usable rigs.

//! Initially only rigs actively connectable are shown, but if a QSO refers to another
//! that will tehn get shown.
class qso_tabbed_rigs :
    public zc_tabs_nonav
{
public:
	//! Constructor.

	//! \param X horizontal position within host window
	//! \param Y vertical position with hosr window
	//! \param W width 
	//! \param H height
	//! \param L label
	qso_tabbed_rigs(int X, int Y, int W, int H, const char* L);
	//! Destructor.
	~qso_tabbed_rigs();

	//! Get active rigs and previous values from settings.
	void load_values();
	//! Instatntiate component widgets. 
	void create_form(int X, int Y);
	//! Configure component widgets after data change.
	void enable_widgets();
	//! Save configuration back to settings.
	void save_values();
	//! Connect/Disconnecy the active rig.
	void switch_rig();
	//! Returns the active rig interface.
	rig_if* rig();

	//! Deactivate all rigs
	void deactivate_rigs();

protected:

	//! Callback from switvhing tabs.
	static void cb_tabs(Fl_Widget* w, void* v);
	//! Callback when closing a tab - v is not used.
	static void cb_close_tab(Fl_Widget* w, void* v);

	//! Map the labels to the widgets
	std::map<std::string, Fl_Widget*> label_map_;

	//! Default tab
	int default_tab_;
};

