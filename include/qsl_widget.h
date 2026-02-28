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
#include <FL/Fl_Widget.H>

class qsl_display;

//! Class to provide a widget to encapsulate the QSL design image.

//! It allows the drawing to be scaled to fit the size of this widget.
class qsl_widget :
    public Fl_Widget
{
public:
    //! Constructor.

    //! \param X horizontal position within host window
    //! \param Y vertical position with hosr window
    //! \param W width 
    //! \param H height
    //! \param L label
    qsl_widget(int X, int Y, int W, int H, const char* L = nullptr);
    //! Destructor.
    ~qsl_widget();

    //! Display widget.
    qsl_display* display();
    //! Override of Fl_Widget::draw().
    
    //! Scales the qsl_display object and draws an enclosing box.
    virtual void draw();

    //! Override of Fl_Widget::handle().
    
    //! Invokes a callback when the left mouse button is released. This 
    //! is usually used to open a window with a full-size version of
    //! the qsl_display.
    virtual int handle(int event);

protected:
    
    //! The QSL image container.
    qsl_display* display_;

};

