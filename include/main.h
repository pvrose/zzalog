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
//! \mainpage ZZALOG API Documentation
//! 
//! This document describes the code interfaces between the constituent parts
//! of ZZALOG. For the user interface see <A class="el" HREF=file:../../userguide/html/index.html>Userguide.</A>
//! 
//! ZZALOG uses the common library ZZACOMMON, whose API is described at
//! <A class="el" HREF=file:zzacommon/index.html>ZZACOMMON API</A>
//! 
//! \section revision Release History
//! See <A class="e1" HREF=file:../../userguide/html/release_notes.html>Release Notes.</A>
//!
//! \section ack Acknowledgements
//! See <A class="e1" HREF=file:../../userguide/html/ack.html>Acknowledgements</A>
//!
//! \copyright Philip Rose GM3ZZA 2025-2026.
//!
//! ZZALOG is based in part on the work of the FLTK project <A HREF=https://www.fltk.org>https://www.fltk.org</A>.
#include "zc_file_holder.h"

#include "hamlib/rig.h"

#include <list>
#include <string>

class record;
class Fl_PNG_Image;
class Fl_Widget;
enum rig_debug_level_e;


// Forward declarations
//! Get the backup filename.
//! \param source Logfile name
//! \param max_depth Number of backups kept.
//! \return filename.
std::string backup_filename(std::string source, int& max_depth);
//! Restores file from backup location.
void restore_backup();

//! Callback for main_window and qso_manager.

//! \param w calling widget.
//! \param v not used.
void cb_bn_close(Fl_Widget* w, void* v);

//! Callback used by FLTK when parsing command-line arguments.

//! \param argc count of arguments.
//! \param argv array of arguments.
//! \param i index of argument to decode.
//! \return index of next argumant to decode.
int cb_args(int argc, char** argv, int& i);

//! Print the help message
void show_help();

//! Get the specified logbook filename

//! \param arg_filename filename supplied by argument.
//! \return selected filename: argument if specified otherwise most recently file opened.
std::string get_file(char* arg_filename);

//! Add some global properties
void add_properties();

//! Read the recent file list from the settings.
void recent_files();

//! Read the following data items:

//! - ADIF specification.
//! - Callsign parsing database.
//! - International character .
//! - Bandplan data.
//! - QSL Designs.
//! - Configured rig data.
//! - Contest specifications.
void add_data();

//! Read the logbook data.

//! \param arg filename supplied as command-line argument.
void add_book(char* arg);

//! Instantiate the following external protocol handlers:

//! - Generic HTTP and UDP handler.
//! - eQSL.cc interface.
//! - Logbook of the World interface.
//! - QRZ.com interface.
//! - Clublog.org interface.
//! - WSJT-X interface.
//! - FlDigi interface (FlLog emulator).
//! - openweather.org interface
void add_qsl_handlers();

//! Instantiate the QSO Manager (Dashboard)
void add_dashboard();

//! Label the main_window window as "[PROGRAM_ID] [PROGRAM_VERSION]: \a text".

//! \param text 
void main_window_label(const std::string& text);

//! Instantiate main_window.
void create_window();

//! Add the component widgets to the main_window.

//! \param curr_y Y-coordinate of last widget added plus its height.
void add_widgets(int& curr_y);

//! Resize and reposition main_window to as it was when last opened or nearest

//! position on current screem.
void resize_window();

//! Delete all created data items
void tidy();

//! Display the arguments in the status log.

//! \param argc number of arguments
//! \param argv array of arguments.
void print_args(int argc, char** argv);

//! Checks the supplied argument \a this_record is within the current session.

//! \param this_record QSO to check.
//! \return true if the QSO is within the surrent session.
bool in_current_session(record* this_record);

//! Customises various aspects when using FLTK widgets
void customise_fltk();

//! Read the sticky switches from the settings.
void read_saved_switches();

//! Save the sticky switches to the settings file.
void save_switches();

//! Initialise hamlib
void load_rig_data();

//! Main program entry point.

//! \param argc number of command-line arguments
//! \param argv array of command-line arguments.
int main(int argc, char** argv);

//! Backs up file to separate location.
void backup_file();

//! Add the specified file to the recent files list.
void set_recent_file(std::string filename);

//! Open the user-guide at the specified page &lt;\a file&gt;.html.
void open_html(const char* file);

//! Open the file \p full_filename  with default application
void open_doc(std::string full_filename);

//! Open the PDF version of the User Guide
void open_pdf();

//! Get \n th recent file
std::string recent_file(int n);

//! Flag to prevent more than one closure process at the same time.
extern bool closing_;

//! Flag to mark everything loaded.
extern bool initialised_;

//! Time loaded.
extern time_t session_start_;

//! Previous frequency.
extern double prev_freq_;

//! File is new (neither in argument or settings.
extern bool new_file_;

//! Using backp.
extern bool using_backup_;

//! Sticky switches mesasge.
extern std::string sticky_message_;

//! Common seed to use in password encryption - maintaned with sessions.
extern uint32_t seed_;
