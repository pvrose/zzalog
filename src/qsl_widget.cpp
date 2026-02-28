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
#include "qsl_widget.h"
#include "qsl_display.h"

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>


qsl_widget::qsl_widget(int X, int Y, int W, int H, const char* L) :
	Fl_Widget(X, Y, W, H, L)
{
	box(FL_BORDER_BOX);
	display_ = new qsl_display(x(), y(), w(), h());
}

qsl_widget::~qsl_widget() {}

// Draw will draw the QSL design, image or text after resizing
void qsl_widget::draw() {
	display_->resize(w(), h());
	display_->draw();
	if (box() == FL_BORDER_BOX) {
		fl_rect(x(), y(), w(), h(), FL_FOREGROUND_COLOR);
	}
}

// Handle intercepts mouse button click to invoke the callback
int qsl_widget::handle(int event) {
	switch (event) {
	case FL_RELEASE: {
		switch (Fl::event_button()) {
		case FL_LEFT_MOUSE: {
			if ((when() & FL_WHEN_RELEASE) == FL_WHEN_RELEASE) do_callback();
			return true;
		}
		}
		break;
	}
	}
	return Fl_Widget::handle(event);
}

// Return a pointer to the qsl_display instance
qsl_display* qsl_widget::display() {
	return display_;
}
