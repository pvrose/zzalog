/*
	Copyright 2026, Philip Rose, GM3ZZA

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

#include <FL/Fl_Double_Window.H>

class Fl_Button;
class field_choice;
class Fl_Group;
class Fl_Radio_Round_Button;


//! \brief A scratchpad window for typing copied code and then copying 
//! and pasting it into the main log window.
//! 
//! The user will type into the scratchpad window and then any selected text
//! can be copied either to the clipboard or into configurable fields in
//! the main dashboard or directly into a QSO record.
class scratchpad : public Fl_Double_Window {
public:
	//! Constructor and destructor.
	scratchpad(int x, int y, int w, int h, const char* title = 0);
	~scratchpad();

	//! Callback for the copy button.
	static void copy_cb(Fl_Widget* w, void* data);
	//! Callback from the clear button.
	static void clear_cb(Fl_Widget* w, void* data);
	//! Callback from the radio buttons to select the copy target.
	static void target_cb(Fl_Widget* w, void* data);
	//! Callback from the field choice to select the field to copy to.
	static void field_choice_cb(Fl_Widget* w, void* data);

	//! \name Key Bindings
	//! @{
	//! Key bindings for text editing operations:
	//!
	//! | Operation                   | Primary Key    |
	//! |-----------------------------|----------------|
	//! | Select word at cursor       | Ctrl+D         |
	//! | Add next word to selection  | Ctrl+D (again) |
	//! | Unselect                    | Escape         |
	//! | Copy selection              | Ctrl+C         |
	//! | Clear text editor           | Ctrl+L         |
	//! @}

	//! Key binding handler for CTRL-D (select word at cursor / add next word).
	static int kf_select(int c, Fl_Text_Editor* editor);
	//! Key binding handler for Escape (unselect).
	static int kf_unselect(int c, Fl_Text_Editor* editor);
	//! Key binding handler for CTRL-C (copy selection).
	static int kf_copy(int c, Fl_Text_Editor* editor);
	//! Key binding handler for CTRL-L (clear text editor).
	static int kf_clear(int c, Fl_Text_Editor* editor);

private:

	//! Select word at cursor position.
	void select_word_at_cursor();
	//! Add next word occurrence to selection.
	void add_next_word_to_selection();
	//! Unselect/clear current selection.
	void unselect();
	//! Copy selection to clipboard or target field.
	void copy_selection();
	//! Clear all text in the editor.
	void clear_text_editor();

	//! The text editor widget for the scratchpad.
	Fl_Text_Editor* editor_;
	//! Copy selected text to clipboard or target field.
	Fl_Button* bn_copy_;
	//! Clear all text in the editor.
	Fl_Button* bn_clear_;
	//! Group of radio buttons to select copy target.
	Fl_Group* g_targets_;
	//! Radio button for copying to clipboard.
	Fl_Radio_Round_Button* bn_clipboard_;
	//! Radio button for copying to the widget expecting copied data.
	Fl_Radio_Round_Button* bn_widget_;
	//! Radio button for copying directly into the selected field in the current QSO record.
	Fl_Radio_Round_Button* bn_field_;
	//! Radio button for copying directly into the predicted field in the current QSO record.
	Fl_Radio_Round_Button* bn_qso_;
	//! Radio button for selecting the field into which to copy the selected text.
	field_choice* field_choice_;

}