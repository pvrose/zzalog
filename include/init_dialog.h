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

#include <string>

#include <FL/Fl_Group.H>

class Fl_Button;
class Fl_Input;
class Fl_Radio_Round_Button;

enum stn_type : uchar;
struct stn_default;

//! This class provides a dialog to enter data for a new installation
class init_dialog :
    public Fl_Group
{
public:

    //! Constructor - sizes and labels itself

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    init_dialog(int X, int Y, int W, int H, const char* L = nullptr);

    //! Destructor
    ~init_dialog();

    //! Instantiate the component widgets
    void create_form();

    //! Copy initial values from staion_defaults_ if the exist
    void load_values();

    //! Configure widgets
    void enable_widgets();

    //! Valid data has been entered
    bool valid_data();

protected:
    //! Callback when "Accept" button clicked
    static void cb_accept(Fl_Widget* w, void* v);

    //! Callback when radio button is selected
    
    //! \param w: widget clicked
    //! \param v: object of type stn_type indicates the new station type
    static void cb_type(Fl_Widget* w, void* v);

    //! VAlid data has been entered. 
    bool valid_data_ = false; 

    //! Invalid data has been entered
    bool invalid_ = false;

    // Widgets
    Fl_Radio_Round_Button* bn_club_;        //!< Club station
    Fl_Radio_Round_Button* bn_indiv_;       //!< Individual station
    Fl_Input* ip_call_;                     //!< Station callsign
    Fl_Input* ip_club_;                     //!< Club name
    Fl_Input* ip_location_;                 //!< Location name
    Fl_Input* ip_name_;                     //!< Club or individual's name
    Fl_Button* bn_accept_;                  //!< Accept these data items
};

