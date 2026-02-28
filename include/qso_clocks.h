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

class qso_clock;
class qso_wx;
class condx_view;
class zc_tabs_nonav;

//! This class contains the clock and weather displays.
class qso_clocks :
    public Fl_Group
{
public:

    //! Constructor.

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    qso_clocks(int X, int Y, int W, int H, const char* L = nullptr);
    //! Destructor.
    ~qso_clocks();

    //! Override Fl_Group::handle accepts focue to enable keyboard F1 to open userguide.
    virtual int handle(int event);

    //! Loads previous timezone from settings
    void load_values();
    //! Instatntiate component widgets.
    void create_form();
    //! Save current timezone to settings.
    void save_values() const;
    //! Configure widgets after data changes.
    void enable_widgets();
    //! Returns whether clock is displaying system locale's timezone.
    bool is_local() const;
    //! Returns Weather control
    qso_wx* wx();

    //! Callback on tabs
    static void cb_tabs(Fl_Widget* w, void* v);

protected:
    // The two instances
    qso_clock* clock_;         //!< Clock display.
    zc_tabs_nonav* tabs_;         //!< Contains weather and solar data tabs.
    qso_wx* qso_weather_;      //!< Weather display.
    condx_view* condx_;        //!< Solar conditions.

    //! Clock is displaying timezone from system locale.
    bool local_;

};

