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

#include "scratchpad.h"

#include "book.h"
#include "field_choice.h"
#include "qso_data.h"
#include "qso_manager.h"
#include "record.h"
#include "regices.h"

#include "zc_drawing.h"
#include "zc_fltk.h"
#include "zc_settings.h"

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Widget.H>

#include <algorithm>
#include <cstdarg>
#include <iostream>
#include <regex>
#include <stack>
#include <string>
#include <utility>

extern Fl_Widget* paste_target_;
extern bool closing_;
scratchpad* scratchpad_ = nullptr;

constexpr int XWIDTH = 200; //!< Width of either major section of the form (editor and controls)

// Constructor
scratchpad::scratchpad() :
	Fl_Double_Window(XWIDTH * 2 + 3 * GAP, 300, "Operating Scratchpad")
{
	create_form();
	load_settings();
	update_widget_states();
};

// Destructor
scratchpad::~scratchpad() {
	save_settings();
}

// Create the form
void scratchpad::create_form() {
	begin();
	// The text editor widget for the scratchpad.
	int curr_x = x() + GAP;
	int curr_y = y() + GAP;
	int curr_w = w() - 3 * GAP;
	int curr_h = h() - 2 * GAP;
	editor_ = new Fl_Text_Editor(curr_x, curr_y, 200, curr_h);
	Fl_Text_Buffer* buffer = new Fl_Text_Buffer();
	editor_->buffer(buffer);
	buffer->add_modify_callback(modify_cb, this);
	editor_->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
	// Set up key bindings for the text editor.
	editor_->add_key_binding('d', FL_CTRL, kf_select);
	editor_->add_key_binding('d', FL_CTRL | FL_SHIFT, kf_select_previous);
	editor_->add_key_binding('u', FL_CTRL, kf_undo_select);
	editor_->add_key_binding(FL_Escape, 0, kf_unselect);
	editor_->add_key_binding('c', FL_CTRL, kf_copy);
	editor_->add_key_binding('l', FL_CTRL, kf_clear);

	curr_x += editor_->w() + GAP + WLABEL;

	op_copy_text_ = new Fl_Output(curr_x, curr_y, WSMEDIT, HBUTTON, "Copy:");
	op_copy_text_->box(FL_BORDER_BOX);
	op_copy_text_->align(FL_ALIGN_LEFT);
	op_copy_text_->tooltip("The text that will be copied to the selected target when the Copy button is pressed");

	curr_x = editor_->w() + GAP;
	curr_y += op_copy_text_->h();

	Fl_Group* controls = new Fl_Group(curr_x, curr_y, 200, 4 * HBUTTON + HTEXT + GAP);
	controls->box(FL_EMBOSSED_BOX);
	controls->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

	curr_y += HTEXT;

	Fl_Group* g_targets = new Fl_Group(curr_x, curr_y, HBUTTON, 4 * HBUTTON, "Paste:");
	g_targets->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	curr_x += GAP;
	// Radio button for copying to clipboard
	bn_clipboard_ = new Fl_Radio_Round_Button(curr_x, curr_y, HBUTTON, HBUTTON, "Clipboard");
	bn_clipboard_->align(FL_ALIGN_RIGHT);
	bn_clipboard_->callback(target_cb, (void*)CLIPBOARD);
	bn_clipboard_->value(copy_target_ == CLIPBOARD);
	bn_clipboard_->tooltip("Copied text will be sent to the clipboard");
	curr_y += bn_clipboard_->h();
	bn_widget_ = new Fl_Radio_Round_Button(curr_x, curr_y, HBUTTON, HBUTTON, "Widget");
	bn_widget_->align(FL_ALIGN_RIGHT);
	bn_widget_->callback(target_cb, (void*)WIDGET);
	bn_widget_->value(copy_target_ == WIDGET);
	bn_widget_->tooltip("Copied text will be sent to the widget expecting copied data");
	curr_y += bn_widget_->h();
	bn_qso_ = new Fl_Radio_Round_Button(curr_x, curr_y, HBUTTON, HBUTTON, "QSO:-");
	bn_qso_->align(FL_ALIGN_RIGHT);
	bn_qso_->callback(target_cb, (void*)QSO);
	bn_qso_->value(copy_target_ == QSO);
	bn_qso_->tooltip("Copied text will be sent to the predicted field in the current QSO record");
	curr_y += bn_qso_->h();
	bn_field_ = new Fl_Radio_Round_Button(curr_x, curr_y, HBUTTON, HBUTTON, "QSO...");
	bn_field_->align(FL_ALIGN_RIGHT);
	bn_field_->callback(target_cb, (void*)FIELD);
	bn_field_->value(copy_target_ == FIELD);
	bn_field_->tooltip("Copied text will be sent to the selected field in the current QSO record");
	g_targets->end();

	curr_x = g_targets->x() + g_targets->w() + WLABEL;
	curr_y = g_targets->y() + HBUTTON;
	// Box to contain paste widget reference data
	box_pastee_ = new Fl_Box(curr_x, curr_y, WSMEDIT, HBUTTON, "Pastee");
	box_pastee_->box(FL_BORDER_BOX);
	box_pastee_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	curr_y += box_pastee_->h();

	// Box to display the predicted field name.
	box_predicted_ = new Fl_Box(curr_x, curr_y, WSMEDIT, HBUTTON, "Next");
	box_predicted_->box(FL_BORDER_BOX);
	box_predicted_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	curr_y += box_predicted_->h();

	// Field choice widget to select the field to copy to when the "Field" copy target is selected.
	field_choice_ = new field_choice(curr_x, curr_y, WSMEDIT, HTEXT);
	field_choice_->tooltip("Select the field to copy to when the \"Field\" copy target is selected");
	field_choice_->set_dataset("Fields", "NAME");
	curr_y += field_choice_->h() + GAP;

	controls->end();
	controls->resizable(nullptr);

	// Copy button to copy the selected text to the selected target.
	curr_x = controls->x() + GAP;
	curr_y = controls->y() + controls->h() + GAP;
	bn_copy_ = new Fl_Button(curr_x, curr_y, WBUTTON, HBUTTON, "Copy");
	bn_copy_->callback(copy_cb, this);
	bn_copy_->tooltip("Copy the selected text to the selected target");
	curr_x += bn_copy_->w() + GAP;
	// Clear button to clear the text editor.

	bn_clear_ = new Fl_Button(curr_x, curr_y, WBUTTON, HBUTTON, "Clear");
	bn_clear_->callback(clear_cb, this);
	bn_clear_->tooltip("Clear the text editor");

	end();
	resizable(editor_);
}

// Load settings
void scratchpad::load_settings() {
	zc_settings settings;
	zc_settings views(&settings, "Views");
	zc_settings scratchpad_settings(&views, "Scratchpad");
	bool open;
	scratchpad_settings.get<bool>("Open Automatically", open, false);
	int top, left;
	scratchpad_settings.get<int>("Top", top, 100);
	scratchpad_settings.get<int>("Left", left, 100);
	position(left, top);
	if (open) {
		show();
	}
}

// Save settings
void scratchpad::save_settings() {
}


// Add dummmy callbacks for the buttons and field choice while developing the form - these will be implemented later
void scratchpad::copy_cb(Fl_Widget* w, void* data) {
	scratchpad* sp = zc::ancestor_view<scratchpad>(w);
	sp->copy_selection();
}

// Callback for the clear button to clear the text editor.
void scratchpad::clear_cb(Fl_Widget* w, void* data) {
	scratchpad* sp = zc::ancestor_view<scratchpad>(w);
	sp->clear_text_editor();
}

// Callback for the radio buttons to select the copy target.
void scratchpad::target_cb(Fl_Widget* w, void* data) {
	scratchpad* sp = zc::ancestor_view<scratchpad>(w);
	sp->copy_target_ = (copy_target)(uintptr_t)data;
	sp->update_widget_states();
}

// Callback from the text buffer when text is modified.
void scratchpad::modify_cb(int pos, int nInserted, int nDeleted,
						   int nRestyled, const char* deletedText, void* cbArg) {
	scratchpad* sp = static_cast<scratchpad*>(cbArg);
	sp->update_widget_states();
}

// Add key bindings for the text editor.
// Key binding handler for CTRL-D (select word at cursor / add next word).
int scratchpad::kf_select(int c, Fl_Text_Editor* editor) {
	scratchpad* sp = zc::ancestor_view<scratchpad>(editor);
	sp->push_selection_to_history();
	Fl_Text_Buffer* buffer = editor->buffer();
	// If  there is no selection, select the word at the cursor position.
	if (!buffer->selected()) {
		int pos = editor->insert_position();
		int pos_start = buffer->word_start(pos);
		int pos_end = buffer->word_end(pos);
		buffer->select(pos_start, pos_end);
	}
	else {
		// Get the currently selected text.
		int sel_start, sel_end;
		buffer->selection_position(&sel_start, &sel_end);
		editor->next_word();
		int pos = editor->insert_position();
		int pos_end = buffer->word_end(pos);
		buffer->select(sel_start, pos_end);
	}
	return 1; // Return 1 to indicate that the key event was handled - TODO: is this true?
}

// Key binding handler for CTRL-SHIFT-D (add previous word to selection).
int scratchpad::kf_select_previous(int c, Fl_Text_Editor* editor) {
	scratchpad* sp = zc::ancestor_view<scratchpad>(editor);
	sp->push_selection_to_history();
	Fl_Text_Buffer* buffer = editor->buffer();
	// If there is no selection, select the word at the cursor position.
	if (!buffer->selected()) {
		int pos = editor->insert_position();
		int pos_start = buffer->word_start(pos);
		int pos_end = buffer->word_end(pos);
		buffer->select(pos_start, pos_end);
		// If still no selection (e.g. if the cursor is at the end of the text), 
		// move to the previous word and select it.
		if (!buffer->selected()) {
			editor->previous_word();
			pos = editor->insert_position();
			pos_start = buffer->word_start(pos);
			pos_end = buffer->word_end(pos);
			buffer->select(pos_start, pos_end);
		}
	}
	else {
		// Get the currently selected text.
		int sel_start, sel_end;
		buffer->selection_position(&sel_start, &sel_end);
		// Move to the previous word
		editor->insert_position(sel_start);
		editor->previous_word();
		int pos = editor->insert_position();
		int pos_start = buffer->word_start(pos);
		buffer->select(pos_start, sel_end);
	}
	return 1; // Return 1 to indicate that the key event was handled
}

// Key binding handler for CTRL-U (undo last selection).
int scratchpad::kf_undo_select(int c, Fl_Text_Editor* editor) {
	Fl_Text_Buffer* buffer = editor->buffer();
	// Pop selection from history and select it if it exists.
	scratchpad* sp = zc::ancestor_view<scratchpad>(editor);
	if (!sp->selection_history_.empty()) {
		std::pair<int, int> sel = sp->selection_history_.top();
		sp->selection_history_.pop();
		buffer->select(sel.first, sel.second);
		// Debug output to verify that the selection history is working correctly.
		std::cout << "Popped selection from history: (" << sel.first << ", " << sel.second << ")" << std::endl;
		return 1; // Return 1 to indicate that the key event was handled
	}
	else {
		// No selection history, so unselect any current selection.
		buffer->unselect();
		return 1; // Return 1 to indicate that the key event was handled - TODO: is this true?
	}
	return 0; // Return 0 to indicate that the key event was not handled - TODO: is this true?
}

// Push the current selection to the selection history stack.
void scratchpad::push_selection_to_history() {
	Fl_Text_Buffer* buffer = editor_->buffer();
	if (buffer->selected()) {
		int sel_start, sel_end;
		buffer->selection_position(&sel_start, &sel_end);
		selection_history_.push(std::make_pair(sel_start, sel_end));
		// Debug output to verify that the selection history is working correctly.
		std::cout << "Pushed selection to history: (" << sel_start << ", " << sel_end << ")" << std::endl;
	}
}

// Key binding handler for Escape (unselect).
int scratchpad::kf_unselect(int c, Fl_Text_Editor* editor) {
	Fl_Text_Buffer* buffer = editor->buffer();
	// If there is a selection, unselect it. Otherwise, do nothing.
	if (buffer->selected()) {
		buffer->unselect();
		return 1; // Return 1 to indicate that the key event was handled - TODO: is this true?
	}
	return 1; // Return 1 to indicate that the key event was handled - TODO: is this true?
}
// Key binding handler for CTRL-C (copy selection).
int scratchpad::kf_copy(int c, Fl_Text_Editor* editor) {
	copy_cb(editor, nullptr);
	return 1; // Return 1 to indicate that the key event was handled - TODO: is this true?
}

// Key binding handler for CTRL-L (clear text editor).
int scratchpad::kf_clear(int c, Fl_Text_Editor* editor) {
	clear_cb(editor, nullptr);
	return 1; // Return 1 to indicate that the key event was handled - TODO: is this true?
}



void scratchpad::update_widget_states() {
//	if (closing_) return;

	// Update the copy text output box to show the currently selected text.
	op_copy_text_->value(editor_->buffer()->selected() ? editor_->buffer()->selection_text() : "");

	// Enable or disable the field choice and name display boxes.
	switch (copy_target_) {
	case CLIPBOARD:
		field_choice_->deactivate();
		box_pastee_->deactivate();
		box_pastee_->label("");
		box_predicted_->deactivate();
		box_predicted_->label("");
		break;
	case WIDGET:
		field_choice_->deactivate();
		box_pastee_->activate();
		box_pastee_->copy_label(pastee_name());
		box_predicted_->deactivate();
		box_predicted_->label("");
		break;
	case QSO:
		field_choice_->deactivate();
		box_pastee_->deactivate();
		box_pastee_->label("");
		box_predicted_->activate();
		box_predicted_->copy_label(predicted_name());
		break;
	case FIELD:
		field_choice_->activate();
		box_pastee_->deactivate();
		box_pastee_->label("");
		box_predicted_->deactivate();
		box_predicted_->label("");
		break;
	}
	// Disable the copy button if there is no selection in the text editor
	if (editor_->buffer()->selected()) {
		bn_copy_->activate();
	} else {
		bn_copy_->deactivate();
	}
	// Disable the clear button if there is no text in the text editor
	if (editor_->buffer()->length()) {
		bn_clear_->activate();
	} else {
		bn_clear_->deactivate();
	}
	// Disable the QSO target options if there is no current QSO record
	if (qso_manager_ && qso_manager_->data()->current_qso()) {
		bn_qso_->activate();
		bn_field_->activate();
	} else {
		bn_qso_->deactivate();
		bn_field_->deactivate();
	}
	// Disable the widget target option if there is no widget expecting copied data
	if (paste_target_) {
		bn_widget_->activate();
	} else {
		bn_widget_->deactivate();
	}
}

// Clear text in the editor
void scratchpad::clear_text_editor() {
	editor_->buffer()->text("");
}

// Get the name of the widget that is the target for pasting, or an empty string if there is no such widget.
const char* scratchpad::pastee_name() {
	// If the widget is a field_input, return the field name.
	if (paste_target_) {
		field_input* fi = dynamic_cast<field_input*>(paste_target_);
		if (fi) {
			return fi->field_name();
		}
	}
	// Otherwise, return the widget's label if it has one
	else if (paste_target_ && paste_target_->label()) {
		return paste_target_->label();
	}
	return "None";
}

// Get the name of the predicted field in the current QSO record.
// If it looks like a signal report (2 or 3 numbers) return RST_RCVD.
// If it looks like a 4 or 6 digit locator, return GRIDSQUARE.
// If it looks like a callsign and the current QSO record has no callsign, return CALLSIGN.
// Otherwise if the current QSO record has no name, return NAME.
// Otherwise if the current QSO record has no QTH return QTH.
// Otherwise return "".
const char* scratchpad::predicted_name() {
	if (!qso_manager_ || !qso_manager_->data()->current_qso()) {
		return "";
	}
	record* qso = qso_manager_->data()->current_qso();
	const char* selected_text = editor_->buffer()->selection_text();
	if (qso->item("RST_RCVD").empty() && std::regex_match(selected_text, std::regex("^\\s*\\d{2,3}\\s*$"))) {
		return "RST_RCVD";
	}
	else if (qso->item("GRIDSQUARE").empty() && std::regex_match(selected_text, std::regex(REGEX_GRIDSQUARE))) {
		return "GRIDSQUARE";
	}
	else if (qso->item("CALL").empty() && std::regex_match(selected_text, std::regex(REGEX_CALL_BODY))) {
		return "CALLSIGN";
	}
	else if (qso->item("NAME").empty()) {
		return "NAME";
	}
	else if (qso->item("QTH").empty()) {
		return "QTH";
	}
	else {
		return "";
	}
}

// Copy the selected text to the selected target.
void scratchpad::copy_selection() {
	if (!editor_->buffer()->selected()) return;
	const char* selected_text = editor_->buffer()->selection_text();
	int len = std::strlen(selected_text);
	switch (copy_target_) {
	case CLIPBOARD:
		Fl::copy(selected_text, len);
		break;
	case WIDGET:
		if (paste_target_) {
			// Copy to clibpoard and then send a paste event to the target widget.
			Fl::copy(selected_text, len);
			// Paste to currently open editor
			Fl::paste(*paste_target_);
		}
		break;
	case QSO:
		if (qso_manager_ && qso_manager_->data()->current_qso() && predicted_name()[0] != '\0') {
			qso_manager_->data()->current_qso()->item(std::string(predicted_name()), std::string(selected_text));
			qso_manager_->update_qso(HT_CHANGED, qso_manager_->data()->current_number(), -1);
		}
		break;
	case FIELD:
		if (qso_manager_ && qso_manager_->data()->current_qso() ) {
			std::string field_name = field_choice_->value();
			qso_manager_->data()->current_qso()->item(field_name, std::string(selected_text));
			qso_manager_->update_qso(HT_CHANGED, qso_manager_->data()->current_number(), -1);
		}
		break;
	}
}