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
#include "view.h"
#include "book.h"
#include "zc_drawing.h"



// Constructor
view::view()
	: my_book_(nullptr)
	, min_w_(WIDTH/4)
	, min_h_(HEIGHT/4)
{
}

// Destructor
view::~view()
{
}

//  Return the book set for this view
book* view::get_book() {
	return my_book_;
}

// Set the book for this view
void view::set_book(book* book) {
	my_book_ = book;
}

// Return the minimum width resizing
int view::min_w() const { return min_w_; }

// Return the minimum height resizing
int view::min_h() const { return min_h_; }
