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

*/#pragma once

#include "cty_data.h"

#include <map>

#include <FL/Fl_Double_Window.H>

class Fl_Output;



//! Dialog to assist the user in updating the various country data files.
class cty_dialog :
    public Fl_Double_Window
{
public:
    //! Constructor.
    
    //! \param W width of window.
    //! \param H height of window.
    //! \param L label for window.
    cty_dialog(int W, int H, const char* L = nullptr);
    //! Destructor.
    ~cty_dialog();

    //! Override of Fl_Double_Window::handle().
    
    //! Takes focus on clicking within it to allow F1 to open userguide.
    virtual int handle(int event);
    //! Instantiate all the component widgets.
    void create_form();
    //! Update component widgets when data changes.
    void update_widgets();

    //! Fetch new data: \p v provides data source type.
    static void cb_update(Fl_Widget* w, void* v);
    //! Reload fetched data into internal database.
    static void cb_reload(Fl_Widget* w, void* v);
    //! Close the dialog
    static void cb_close(Fl_Widget* w, void* v);
    //! Open directory browser at directory containing all the country data files.
    static void cb_browser(Fl_Widget* w, void* v);
    //! Copy working back to source
    static void cb_release(Fl_Widget* w, void* v);

protected:

    //! Widgets displaying timestamps that can be updated.
    std::map<cty_data::cty_type_t, Fl_Output*> w_timestamps_;
    //! Widgets displaying data versions that can be updated.
    std::map<cty_data::cty_type_t, Fl_Output*> w_versions_;
    
};

