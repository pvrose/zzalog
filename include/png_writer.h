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

#include <string>
#include <cstdio>

#include "png.h"

class Fl_RGB_Image;
class book;
class record;



//! Class that outputs the image for an e-mail QSL card.

//! It uses the library png supplied as part of the FLTK installation.
class png_writer
{
public:
	//! Constructor.
	png_writer();
	//! Destructor.
	~png_writer();

	//! Get the PNG filename for the \p qso.
	static std::string png_filename(record* qso);
	//! Generate files for the \p qsos specified.
	bool write_book(book* qsos);

	//! Write the RGB \p image out to a .png file \p filename.
	bool write_image(Fl_RGB_Image* image, std::string filename);

protected:

	//! Initialise the PNG instance
	bool initialise_png();
	//! Write one line of \p data
	bool write_row(const char* data);
	//! Finalise and close the interface
	bool finalise_png();
	//! Clearup
	void tidy_png();

	//! Callback required by libpng library to handle error.
	static void error_handler(png_structp png, png_const_charp msg);

	//! Output file handle.
	FILE* out_file_;

	//! The image to be written out.
	Fl_RGB_Image* image_;

	//! Local copy of data.
	char* buffer_;

	// libpng items
	jmp_buf jump_buf_;     //!< Used to skip data after error.
	png_structp png_;      //!< PNG data structure.
	png_infop info_;       //!< PNG information structure

};

