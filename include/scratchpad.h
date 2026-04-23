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

#include <cstdint>
#include <stack>
#include <utility>

class Fl_Box;
class Fl_Button;
class field_choice;
class Fl_Group;
class Fl_Output;
class Fl_Radio_Round_Button;
class Fl_Text_Editor;


//! \brief A scratchpad window for typing copied code and then copying 
//! and pasting it into the main log window.
//! 
//! The user will type into the scratchpad window and then any selected text
//! can be copied either to the clipboard or into configurable fields in
//! the main dashboard or directly into a QSO record.
class scratchpad : public Fl_Double_Window {
public:
	//! Constructor and destructor.
	scratchpad();
	~scratchpad();

	//! Update the states of the widgets based on the current selection and copy target.
	void update_widget_states();

	//! Notification that the main window's paste target has changed, so update the display.
	void update_paste_target() {
		update_widget_states();
	}

	//! Callback for the copy button.
	static void copy_cb(Fl_Widget* w, void* data);
	//! Callback from the clear button.
	static void clear_cb(Fl_Widget* w, void* data);
	//! Callback from the radio buttons to select the copy target.
	static void target_cb(Fl_Widget* w, void* data);
	//! Callback from the text buffer when text is modified.
	static void modify_cb(int pos, int nInserted, int nDeleted, 
						  int nRestyled, const char* deletedText, void* cbArg);

	//! \name Key Bindings
	//! @{
	//! Key bindings for text editing operations:
	//!
	//! | Operation                   | Primary Key    |
	//! |-----------------------------|----------------|
	//! | Select word at cursor       | Ctrl+D         |
	//! | Add next word to selection  | Ctrl+D (again) |
	//! | Add previous word           | Ctrl+Shift+D   |
	//! | Undo last selection         | Ctrl+U         |
	//! | Unselect                    | Escape         |
	//! | Copy selection              | Ctrl+C         |
	//! | Clear text editor           | Ctrl+L         |
	//! @}

	//! Key binding handler for CTRL-D (select word at cursor / add next word).
	static int kf_select(int c, Fl_Text_Editor* editor);
	//! Key binding handler for CTRL-SHIFT-D (add previous word to selection).
	static int kf_select_previous(int c, Fl_Text_Editor* editor);
	//! Key binding handler for CTRL-U (undo last selection).
	static int kf_undo_select(int c, Fl_Text_Editor* editor);
	//! Key binding handler for Escape (unselect).
	static int kf_unselect(int c, Fl_Text_Editor* editor);
	//! Key binding handler for CTRL-C (copy selection).
	static int kf_copy(int c, Fl_Text_Editor* editor);
	//! Key binding handler for CTRL-L (clear text editor).
	static int kf_clear(int c, Fl_Text_Editor* editor);

private:

	//! Enumeration for the copy target options.
	enum copy_target : uint8_t {
		CLIPBOARD,  //!< Copy to clipboard
		WIDGET,     //!< Copy to the widget expecting copied data
		FIELD,      //!< Copy directly into the selected field in the current QSO record
		QSO         //!< Copy directly into the predicted field in the current QSO record
	};

	//! Create the form and its widgets.
	void create_form();
	//! Load settings
	void load_settings();
	//! Save settings
	void save_settings();

	//! Copy selection to clipboard or target field.
	void copy_selection();
	//! Clear all text in the editor.
	void clear_text_editor();

	//! Push current selection onto the selection history stack for undo functionality.
	void push_selection_to_history();

	//! Get the reference name of the paste widget
	const char* pastee_name();

	//! Get the predicted field name (based on selected text and current QSO)
	const char* predicted_name();


	//! The text editor widget for the scratchpad.
	Fl_Text_Editor* editor_;
	//! Copy selected text to clipboard or target field.
	Fl_Button* bn_copy_;
	//! Clear all text in the editor.
	Fl_Button* bn_clear_;
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
	//! Box to display info on paste widget.
	Fl_Box* box_pastee_;
	//! Box to display info on predicted field.
	Fl_Box* box_predicted_;
	//! Output to display copy text
	Fl_Output* op_copy_text_;

	//! The currently selected copy target.
	copy_target copy_target_ = CLIPBOARD;

	//! Stack to keep track of previous selections for undo functionality.
	std::stack<std::pair<int, int>> selection_history_;

};

//! Global pointer to the scratchpad window instance.
extern scratchpad* scratchpad_;