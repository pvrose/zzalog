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
#include <FL/Fl_Scroll.H>

#include <map>
#include <string>


enum qth_value_t : char;
class Fl_Button;
class Fl_Input;
class Fl_Input_Choice;
class Fl_Output;
struct qth_info_t;

//! This class represents a single line in the stn_qth_dlg editing pane.
class stn_qth_widget :
    public Fl_Group
{
public:
    //! Constructor 

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    stn_qth_widget(int X, int Y, int W, int H, const char* L = nullptr);

    //! Destructor
    ~stn_qth_widget();

    //! Set the values into the inputs
    void enable_widgets();

	//! Return location valid
//	bool location_valid();

	//! Callback - entering value into an input

    //! \param w widget that raised the callback
    //! \param v qth_value_t 
    static void cb_ip_data(Fl_Widget* w, void* v);

protected:
    //! Instantiate the component widgets
    void create_form();

	//! Location data valid
	bool location_valid_;

    // Widgets
    Fl_Input* ip_descr_;         //!< Description
    std::map<qth_value_t, Fl_Input*> ip_data_;
    //!< Data items
};



//! Class  provides a container to hold several stn_qth_widget items
class stn_qth_cntnr :
    public Fl_Scroll
{
public:
    //! Constructor 

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    stn_qth_cntnr(int X, int Y, int W, int H, const char* L = "");

    //! Destructor
    ~stn_qth_cntnr();

    //! Get selected widget
    stn_qth_widget* value();

    //! Get selected ID
    std::string get_selected();

    //! Set selected widget
    void set_selected(std::string id);

    //! Redraw widgets
    void redraw_widgets();

protected:

    //! Contained widgets
    std::map<std::string, stn_qth_widget*> widgets_;

    //! Selected widget
    std::string selected_;

};


//! \brief This class provides the QTH pane of the stn_dialog
class stn_qth_dlg :
    public Fl_Group
{
public:
    //! Constructor 

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    stn_qth_dlg(int X, int Y, int W, int H, const char* L);

    //! Desctructor
    ~stn_qth_dlg();

    //! Enable widgets
    void enable_widgets();

    //! Set selected
    void set_location(std::string s);

    //! Get location
    std::string get_location();

	//! Returns location valid (meets certain criteria: \see location_valid_)
//	bool location_valid();
    
    //! Callback from "Add" button
    static void cb_add(Fl_Widget* w, void* v);

    //! Callback from "Delete" button
    static void cb_delete(Fl_Widget* w, void* v);

    //! Callback from "Clear" button to clear all values for specific identifier.
    static void cb_clear(Fl_Widget* w, void* v);

    //! Callback from "Update from call" updates locator items that can be decoded from the call.
    static void cb_update(Fl_Widget* w, void* v);

    //! Callback from "Rename" button
    static void cb_rename(Fl_Widget* w, void* v);

    //! callback from choice
    static void cb_choice(Fl_Widget* w, void* v);

    //! Callback from "Check log" button
    static void cb_check(Fl_Widget* w, void* v);

    //! CAllback from "Set Default" button
    static void cb_default(Fl_Widget* w, void* v);

protected:
    //! Load data
    void load_data();

    //! Instantiate component widgets
    void create_form();
    
    //! Update from call
    void update_from_call();

    //! Populate call choice
    void populate_calls();

    //! Populate location choice
    void populate_locations();

    // Widgets
    Fl_Button* bn_update_;           //!< Update selected data from call
    Fl_Input_Choice* ch_call_;       //!< Select callsign
    Fl_Button* bn_clear_;            //!< Clear all fields
    Fl_Button* bn_add_;              //!< Add an entry to QTH data
    Fl_Input_Choice* ip_new_;        //!< Select location name
    Fl_Button* bn_delete_;           //!< Selete selected entry
    Fl_Button* bn_rename_;           //!< Rename selected entry as new entry
    Fl_Button* bn_check_;            //!< Check log
    Fl_Button* bn_default_;          //!< Set as default
    stn_qth_cntnr* table_;           //!< Table displaying QTH details

    bool selected_new_ = false;      //!< A new location has been enetered (for Add or REname)

	bool location_valid_ = false;    //!< Location is valid, either City or Gridsquare is set.

    std::string new_location_;       //!< New location entered
    std::string location_;           //!< Current selected location
};

