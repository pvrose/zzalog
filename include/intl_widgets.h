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
#ifndef __INTL_WIDGETS__
#define __INTL_WIDGETS__

#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Editor.H>

/* 
This file provides versions of standard input widgets that allow pastes from
the international character dialog
*/


	//! Vesrion of Fl_Text_Editor that accepts pastes from intl_dialog
	class intl_editor : public Fl_Text_Editor
	{
		//! Whether the editor is inserting or overwriting
		bool insert_mode_;

	public:
		//! Constructor

		//! \param X horizontal position within host window
		//! \param Y vertical position with hosr window
		//! \param W width 
		//! \param H height
		//! \param L label
		intl_editor(int X, int Y, int W, int H, const char* L = "");
		//! Destructor.
		virtual ~intl_editor();

		//! Event handler - handle event as normal then set the cursor depending on current insert mode.
		int handle(int event);
	};


	//! Version of Fl_Input that accepts pastes from intl_dialog
	class intl_input : public Fl_Input
	{
	public:
		//! Constructor

		//! \param X horizontal position within host window
		//! \param Y vertical position with hosr window
		//! \param W width 
		//! \param H height
		//! \param label label
		intl_input(int X, int Y, int W, int H, const char* label = nullptr);
		//! Descriptor.
		virtual ~intl_input();

		//! Event handler - tells intl_dialog that this widget has focus.
		int handle(int event);
	};

#endif