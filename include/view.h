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
#ifndef __VIEW__
#define __VIEW__

#include "zc_drawing.h"

#include <cstdint>
#include <map>
#include <string>



	class book;
	enum hint_t : uint8_t;
	typedef size_t qso_num_t;

	//! This is a base class for use with updating - each view will also inherit indirectly from Fl_Widget.
	class view
	{
	public:
		//! Constructor.
		view();
		//! Destructor
		virtual ~view();

		// Public methods
	public:
		//! something has changed in the book

		//! \param hint An indication of what has changed.
		//! \param record_num_1 The index of the QSO record that has specifically ben modified.
		//! \param record_num_2 The index of an asscoiated QSO record. 
		virtual void update(hint_t hint, qso_num_t record_num_1, qso_num_t record_num_2 = 0) = 0;
		//! Returns minimum width the view can be resized
		int min_w() const;
		//! Returns minimum height the view can be resized
		int min_h() const;
		//! Set book used by the view.
		void set_book(book* book);
		//! Returns the book 
		book* get_book();

		// protected methods
	protected:

		// protected attributes
	protected:
		//! The version of the book being displayed in the view
		book * my_book_;
		//! Minimum width
		int min_w_;
		//! Minimum height
		int min_h_;
	};
#endif
