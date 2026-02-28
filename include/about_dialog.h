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
#ifndef __ABOUT_DIALOG__
#define __ABOUT_DIALOG__

#include "win_dialog.h"

#include <FL/Fl_Widget.H>



	//! Dialog to provide program information.
	
	//! This supplies details of the compilation status of ZZALOG. 
	//! This acknowledges all third-party libraries and data used in ZZALOG.
	class about_dialog :
		public win_dialog
	{
	public:
		//! Constructor.
		about_dialog();
		//! Destructor.
		virtual ~about_dialog();

		//! Callback when \p OK button is clicked.
		
		//! \param w the clicked button
		//! \param v not used.
		static void cb_bn_ok(Fl_Widget* w, void* v);
		//! Callback when \p CANCEL button clicked.
		
		//! \param w the clicked button
		//! \param v not used.
		static void cb_bn_cancel(Fl_Widget* w, void * v);

	protected:

	};

#endif

