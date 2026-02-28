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
#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include <FL/Fl_Double_Window.H>





	//! This calss inherits from Fl_Single_Window and is the main application window.
	
	//! It allows custom handling FL_SHOW and FL_HIDE events
	class main_window :
		public Fl_Double_Window
	{
	public:
		//! Constructor.

		//! \param W width 
		//! \param H height
		//! \param label label
		main_window(int W, int H, const char* label = nullptr);
		//! Destructor.
		~main_window();

		//! Override Fl_Double_Window::handle.
	 
		//! On HIDE and SHOW let menu know visibility.
		//! Accept paste events to import QSO records from the clipboard.
		virtual int handle(int event);
	};

#endif