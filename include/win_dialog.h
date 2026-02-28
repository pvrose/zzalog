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
#ifndef __DIALOG__
#define __DIALOG__

#include "zc_drawing.h"

#include <FL/Fl_Window.H>



	//! Used for general button responses
	enum button_t {
		BN_OK,      //!< OK
		BN_CANCEL,  //!< Cancel
		BN_SPARE    //!< Any third and subsequent commands - use BN_SPARE+n
	};

	//! \brief This class is the base class for free-standing dialogs -
	//! it provides a standard way of waiting for the dialog to be closed.
	class win_dialog : public Fl_Window
	{
	public:
		//! Constructor.

		//! \param W width 
		//! \param H height
		//! \param L label
		win_dialog(int W, int H, const char* L = 0);
		//! Destructor.
		virtual ~win_dialog();

		//! Display the dialog until OK or CANCEL Is clicked (pending_button_ is cleared)
		button_t display();
		//! Call at end of any button call-back to clear pending_button_
		void do_button(button_t button);

	protected:
		//! Which button was clicked.
		button_t button_;

	};
#endif