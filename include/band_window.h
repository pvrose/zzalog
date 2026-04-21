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
#include <FL/Fl_Double_Window.H>

class band_editor;
class band_widget;
class zc_tabs_nonav;
class Fl_Choice;

//! A separate window that displays the full bandplan view.
class band_window :
    public Fl_Double_Window
{
public:
    //! Constructor

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    band_window(int X, int Y, int W, int H, const char* L = nullptr);
    //! Destructor
    ~band_window();

    //! Overload of Fl_Double_Window::draw().
    
    //! Passes on the selection colours to band_widget. 
    virtual void draw();
    
    //! Set the frequency
    
    //! \param tx transmit frequency.
    //! \param rx receive frequency.
    void set_frequency(double tx, double rx);

    //! Callback from band_widget.
    
    //! Callback called when band_widget is clicked. The frequency
    //! under the mouse click is read and passed to the current rig
    //! to switch frequency to that clicked.
    static void cb_widget(Fl_Widget* w, void* v);

    //! Callback from tabs
    static void cb_tabs(Fl_Widget* w, void* v);

	//! Callback from band choice widget
	static void cb_band_choice(Fl_Widget* w, void* v);

protected:
    //! Enable widgets (redraw tabs)
    void enable_widgets();

	//! Populate the band choice widget with the available bands and select the current band.
	void populate_band_choice();

    //! The tabs container
    zc_tabs_nonav* tabs_;
    //! The band_widget instance.
    band_widget* bw_;
    //! The band_editor instance.
    band_editor* be_;
	//! The choice widget for band selection.
	Fl_Choice* band_choice_;

};

extern band_window* band_window_;