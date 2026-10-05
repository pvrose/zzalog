#pragma once

#include "lunasvg.h"
#include <FL/Fl_RGB_Image.H>
#include <algorithm>
#include <string>

//! This is helpers for loading and displaying SVG images in FLTK. It uses the Lunasvg library to render the SVG into a bitmap, which is then converted into an Fl_RGB_Image for display in FLTK.

//! \param svg_data The SVG data as a string.
//! \param width The desired width of the rendered image in pixels.
//! \param height The desired height of the rendered image in pixels.
//! \return A pointer to an Fl_RGB_Image containing the rendered image, or nullptr if the SVG could not be loaded or rendered.
static Fl_RGB_Image* load_svg_image(const std::string& svg_data, int width, int height) {
	bool ok = true;
	lunasvg::Bitmap bitmap;
	auto document = lunasvg::Document::loadFromData(svg_data);
	if (!document) ok = false;
	if (ok) {
		bitmap = document->renderToBitmap(width, height);
		if (bitmap.isNull()) ok = false;
	}
	if (ok) {
		bitmap.convertToRGBA();
		int img_width = bitmap.width();
		int img_height = bitmap.height();
		int buffer_size = img_width * img_height * 4;
		uchar* data = new uchar[buffer_size];
		std::copy(bitmap.data(), bitmap.data() + buffer_size, data);
		Fl_RGB_Image* img = new Fl_RGB_Image((const uchar*)data, img_width, img_height, 4, 0);
		return img;
	}
	else {
		return nullptr;
	}
}

//! This returns the RGBA bitmap data from an SVG string. The returned data is a std::string. The width and height of the bitmap are returned via the width and height reference parameters.
//! \param svg_data The SVG data as a string.
//! \param width The desired width of the rendered image in pixels.
//! \param height The desired height of the rendered image in pixels.
//! \param out_width Reference to an integer that will receive the width of the rendered bitmap.
//! \param out_height Reference to an integer that will receive the height of the rendered bitmap.
//! \return The RGBA bitmap data as a std::string, or an empty string if the SVG could not be loaded or rendered.
static std::string load_svg_bitmap(const std::string& svg_data, int width, int height, int& out_width, int& out_height) {
	bool ok = true;
	lunasvg::Bitmap bitmap;
	auto document = lunasvg::Document::loadFromData(svg_data);
	if (!document) ok = false;
	if (ok) {
		bitmap = document->renderToBitmap(width, height);
		if (bitmap.isNull()) ok = false;
	}
	if (ok) {
		bitmap.convertToRGBA();
		out_width = bitmap.width();
		out_height = bitmap.height();
		int buffer_size = out_width * out_height * 4;
		std::string data((const char*)bitmap.data(), buffer_size);
		return data;
	}
	else {
		out_width = 0;
		out_height = 0;
		return "";
	}
}


