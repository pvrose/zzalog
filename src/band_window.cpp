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
#include "band_window.h"

#include "band_editor.h"
#include "band_widget.h"
#include "ident.h"
#include "qso_manager.h"
#include "rig_if.h"
#include "zc_status.h"

#include "zc_drawing.h"
#include "zc_utils.h"

#include <cmath>
#include <cstdio>
#include <string>

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include "zc_tabs_nonav.h"
#include <FL/Fl_Widget.H>

band_window::band_window(int X, int Y, int W, int H, const char* L) :
	Fl_Double_Window(X, Y, W, H, L)
{
	tabs_ = new zc_tabs_nonav(0, 0, W, H - FOOT_HEIGHT);
	tabs_->labeltype(FL_NO_LABEL);
	tabs_->box(FL_BORDER_BOX);
	tabs_->handle_overflow(Fl_Tabs::OVERFLOW_PULLDOWN);
	tabs_->callback(cb_tabs);
	int rx = 0;
	int ry = 0;
	int rw = 0;
	int rh = 0;
	tabs_->client_area(rx, ry, rw, rh, 0);
	tabs_->box(FL_FLAT_BOX);

	bw_ = new band_widget(rx, ry, rw, rh, "View");
	bw_->labelsize(FL_NORMAL_SIZE + 2);
	bw_->box(FL_BORDER_BOX);
	bw_->type(band_widget::BAND_FULL | band_widget::ZOOMABLE);
	bw_->selection_color(selection_color());
	bw_->callback(cb_widget);
	bw_->when(FL_WHEN_RELEASE);

	be_ = new band_editor(rx, ry, rw, rh, "Editor");
	be_->labelsize(FL_NORMAL_SIZE + 2);
	be_->box(FL_BORDER_BOX);

	tabs_->end();

	Fl_Box* b_cr = new Fl_Box(0, H - FOOT_HEIGHT, W, FOOT_HEIGHT);
	b_cr->box(FL_FLAT_BOX);
	b_cr->copy_label(std::string(COPYRIGHT + " " + CONTACT + "     ").c_str());
	b_cr->labelsize(FL_NORMAL_SIZE - 1);
	b_cr->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

	bw_->show();

	end();
	resizable(bw_);
	show();

	enable_widgets();

}

band_window::~band_window() {}

// Overload draw
void band_window::draw() {
	// Pass on selection colour to the widget
	bw_->color(color(), selection_color());
	Fl_Double_Window::draw();
}

void band_window::set_frequency(double tx, double rx) {
	bw_->value(tx, rx);
	be_->value(tx);
}

// Callback from 
void band_window::cb_widget(Fl_Widget* w, void* v) {
	band_widget* bw = (band_widget*)w;
	double f = bw->frequency(Fl::event_x(), Fl::event_y());
	if (!std::isnan(f)) {
		rig_if* rig = qso_manager_->rig();
		if (rig) {
			if (!rig->set_frequency(f)) {
				char msg[128];
				snprintf(msg, sizeof(msg), "BAND: Unable to std::set frequency %g", f);
				status_->misc_status(ST_WARNING, msg);
			}
		}
	}
}

//! Callback from tabs
void band_window::cb_tabs(Fl_Widget* w, void* v) {
	band_window* that = zc::ancestor_view<band_window>(w);
	that->enable_widgets();
}

//! Format the tabs
void band_window::enable_widgets() {
	// Set standard tab label formats
	for (int ix = 0; ix < tabs_->children(); ix++) {
		Fl_Widget* wx = tabs_->child(ix);
		if (wx == tabs_->value()) {
			wx->labelfont((wx->labelfont() | FL_BOLD) & (~FL_ITALIC));
			wx->labelcolor(FL_FOREGROUND_COLOR);
		}
		else {
			wx->labelfont((wx->labelfont() & (~FL_BOLD)) | FL_ITALIC);
			wx->labelcolor(FL_FOREGROUND_COLOR);
		}
	}
}