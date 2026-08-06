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

// local header files
#include "main.h"

#include "about_dialog.h"
#include "band_data.h"
#include "band_window.h"
#include "book.h"
#include "club_handler.h"
#include "config.h"
#include "contest_data.h"
#include "cty_data.h"
#include "debug_flags.h"
#include "eqsl_handler.h"
#include "extract_data.h"
#include "field_choice.h"
#include "fields.h"
#include "file_types.h"
#include "fldigi_handler.h"
#include "import_data.h"
#include "intl_dialog.h"
#include "keyring.h"
#include "lotw_handler.h"
#include "main_window.h"
#include "menu_bar.h"
#include "qrz_handler.h"
#include "qsl_dataset.h"
#include "qso_manager.h"
#include "record.h"
#include "rig_data.h"
#include "scratchpad.h"
#include "spec_data.h"
#include "spec_tree.h"
#include "stn_data.h"
#include "stn_dialog.h"
#include "tabbed_forms.h"
#include "toolbar.h"
#include "zc_url_handler.h"
#include "wsjtx_handler.h"
#include "wx_handler.h"

#include "hamlib/rig.h"
#include "boost/version.hpp"

#include "zc_debug.h"
#include "zc_banner.h"
#include "zc_drawing.h"
#include "zc_file_holder.h"
#include "zc_settings.h"
#include "zc_status.h"
#include "zc_ticker.h"
#include "zc_fltk.h"

// C/C++ header files
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <list>
#include <map>
#include <string>

// FLTK header files
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_types.h>
#include <FL/Fl_Widget.H>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <FL/Fl_Window.H>

#include <boost/filesystem.hpp>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/dll/runtime_symbol_info.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

// Application details - created by CMake.
extern std::string APP_NAME;
extern std::string APP_VERSION;
extern std::string APP_TIMESTAMP;
extern std::string CONTACT;
extern std::string COPYRIGHT;
extern std::string ZZACOMMON_VERSION;

// Debug switches
debug_flag DEBUG_ALL = 0xFFFFFFFF; 
extern debug_flag DEBUG_NEXT;
extern debug_flag DEBUG_QUICK;   //!< Print quick debugging messages -  by "-d q"
extern debug_flag DEBUG_THREADS;  //!< Print thread debugging messages -  by "-d t"
extern debug_flag DEBUG_CURL;
extern debug_flag DEBUG_SOCKET;
extern debug_flag DEBUG_XMLRPC;
extern debug_flag DEBUG_DEVELOPMENT; 
extern debug_flag DEBUG_TEST_PRODUCT;  //!< Test production mode in development -  by "-d "
debug_flag DEBUG_RIGS = DEBUG_NEXT << 1;    //!< Print file reset messages -  by "-d r"
//! Print callsign parsing messages -  by "-d d"
debug_flag DEBUG_PARSE = DEBUG_NEXT << 2;
//! Print QSO modification status - by "-d m"
debug_flag DEBUG_MOD_STATUS = DEBUG_NEXT << 3;
//! Copy status messages to the terminal - by "-d s"
debug_flag DEBUG_STATUS = DEBUG_NEXT << 4;
//! Set hamlib debugging verbosity level -  by "-d h=<level>"
rig_debug_level_e HAMLIB_DEBUG_LEVEL = RIG_DEBUG_ERR;

//! main window width
unsigned int WIDTH = 1000;
//! main window height
unsigned int HEIGHT = 650;

// Operation switches - _S versions used to override sticky switch
//! Automatically upload QSOs to QSL sites -  by "-n"
bool AUTO_UPLOAD = true;
//! Version of \p AUTO_UPLOAD read from settings
bool AUTO_UPLOAD_S = false;
//! Automatically save QSO record after each change -  by "-a"
bool AUTO_SAVE = true;
//! Version of \p AUTO_SAVE read from settings.
bool AUTO_SAVE_S = false;
//! Dark mode: Dark background, light forreground -  by "-k"
bool DARK = false;
//! Version of \p DARK read from settings.
bool DARK_S = false;
//! Print version details instead of running ZZALOG -  by "-v"
bool DISPLAY_VERSION = false;
//! Print command-line interface instead of running ZZALOG -  by "-h"
bool HELP = false;
//! Start with an empty logbook -  by "-e"
bool NEW_BOOK = false;
//! Do not add file to recent file list -  by "-p"
bool PRIVATE = false;
//! Open file in read-only mode -  by "-r"
bool READ_ONLY = false;
//! Resum logging including previous session -  by "-m"
bool RESUME_SESSION = false;
//! Start all rigs - by "-s all"
bool START_ALL_RIGS = false;
//! List of rigs to start - by "-s <rig1> -s <rig2>"
std::list<std::string> START_RIGS = {};
//! Open userguide instead of running ZZALOG -  by "-h html"
bool HELP_HTML = false;
//! Open PDF userguide instead of running ZZALOG -  by "-h pdf"
bool HELP_PDF = false;
//! Default font size -  by "-z <size>"
int DEFAULT_FONT_SIZE = DEFAULT_DEFAULT_SIZE;
//! \p DEFAULT_FONT_SIZE taken from switch
bool DEFAULT_FONT_SIZE_S = false;


//! File holder customisation - control data
const std::map < uint8_t, file_control_t > FILE_CONTROL = {
	// ID, { filename, reference, read-only
	{ FILE_ADIF, { "all.json", true, true, DEBUG_RESET_ADIF } },
	{ FILE_BANDPLAN, { "band_plan.json", true, false, DEBUG_RESET_BAND } },
	{ FILE_COUNTRY_CLUB, { "cty.xml", true, false, DEBUG_RESET_CTY1 } },
	{ FILE_COUNTRY_CFILES, { "cty.csv", true, false, DEBUG_RESET_CTY2 }},
	{ FILE_COUNTRY_DXATLAS, { "Prefix.lst", true, false, DEBUG_RESET_CTY3 }},
	{ FILE_COUNTRY_ISO, { "iso.csv", true, false, DEBUG_RESET_CTY4 }},
	{ FILE_COUNTRY, { "cty.json", false, false, DEBUG_RESET_CTY, false } },
	{ FILE_INTLCHARS, { "intl_chars.txt", true, true, DEBUG_RESET_INTL }},
	{ FILE_ICON_GMAPS, { "google-maps.png", true, true, DEBUG_RESET_ICON }},
	{ FILE_ICON_PDF, { "pdf.png", true, true, DEBUG_RESET_ICON}},
	{ FILE_ICON_QRZ, { "qrz_1.jpg", true, true, DEBUG_RESET_ICON}},
	{ FILE_APPS, { "apps.json", false, false, DEBUG_RESET_APPS}},
	{ FILE_SETTINGS, { "ZZALOG.json", false, false, DEBUG_RESET_SETT }},
	{ FILE_RIGS, { "rigs.json", false, false, DEBUG_RESET_RIGS }},
	{ FILE_FIELDS, { "fields.json", false, false, DEBUG_RESET_FLDS}},
	{ FILE_CONTEST, { "contests.json", false, false, DEBUG_RESET_TEST }},
	{ FILE_SOLAR, { "solar.xml", false, false, 0}},
	{ FILE_STATUS, { "status.txt", false, false, 0}},
	{ FILE_STATION, { "station.json", false, false, DEBUG_RESET_STN }},
	{ FILE_ICON_ZZA, { "rose.png", true, true, 0}},
	{ FILE_QSL, { "qsl.json", false, false, DEBUG_RESET_QSL }},
	{ FILE_KEYS, { "keys.json", true, false, DEBUG_RESET_KEYS }},
	{ FILE_CONTESTS, { "contests/algorithms.json", true, false, DEBUG_RESET_TEST, true } }
	// Individual contest algorithm files will be dynamically added here...
};

//! Access to FLTK global attribute to  default text size throughout ZZALOG.
extern int FL_NORMAL_SIZE;

//! \cond

//! List of files most recently opened. Maximum: 4 files. 
std::list<std::string> recent_files_;
//! \endcond
//! Flag to prevent more than one closure process at the same time.
bool closing_ = false;

//! Flag to mark everything loaded.
bool initialised_ = false;

//! Time loaded.
time_t session_start_ = (time_t)0;

//! Previous frequency.
double prev_freq_ = 0.0;

//! Filename in arguments.
char* arg_filename_ = nullptr;

//! File is new (neither in argument or settings.
bool new_file_ = false;

//! Using backp.
bool using_backup_ = false;

//! Sticky switches mesasge.
std::string sticky_message_ = "";

//! Common seed to use in password encryption - maintaned with sessions.
uint32_t seed_ = 0;

//! Development directory
std::string development_directory_;

//! Default location for auto-generated compile fodder
std::string default_code_directory_ = "";

//! Widget to receive pastes from dialogs with paste functionality.
Fl_Widget* paste_target_ = nullptr;

// Get the backup filename
std::string backup_filename(std::string source, int& max_depth) {
	zc_settings top_settings;
	zc_settings behav_settings(&top_settings, "Behaviour");
	zc_settings backup_settings(&behav_settings, "Backup");
	// Get back-up directory
	std::string backup;
	backup_settings.get<std::string>("Path", backup, "");
	backup_settings.get("Depth", max_depth, -1);
	bool first = true;
	while (backup.length() == 0) {
		Fl_Native_File_Chooser* chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
		if (first) {
			chooser->title("Operation requires a backup location.\n"
				"Select directory for backup");
			first = false;
		}
		else {
			chooser->title("Backup location is REQUIRED to continue.\n"
				"Select directory for backup");
			fl_beep(FL_BEEP_ERROR);
		}
		if (chooser->show() == 0) {
			backup = chooser->filename();
		}
		delete chooser;
	}
	// Now get the depth.
	if (max_depth == -1) {
		const char* text = fl_input("Please specify the maximum number of backups - default 8.", "8");
		max_depth = atoi(text);
	}
	// Save the result of the chooser
	backup_settings.set<std::string>("Path", backup);
	backup_settings.set("Depth", max_depth);
	backup += "/" + zc::terminal(source);
	return backup;
}

// Restore from last backup
void restore_backup() {
	//std::string filename = book_->filename();
	//// Remove existing book
	//status_->misc_status(ST_WARNING, "LOG: Closing current book!");
	//menu_bar::cb_mi_file_new(nullptr, nullptr);
	//zc_settings top_settings;
	//zc_settings behav_settings(&top_settings, "Behaviour");
	//zc_settings backup_settings(&behav_settings, "Backup");
	//std::string backup;
	//backup_settings.get<std::string>("Last Backup", backup, "");
	//// Get backup data
	//READ_ONLY = true;
	//book_->load_data(backup);
}


// This callback intercepts the close command and performs checks and tidies up
// Updates recent files settings
void cb_bn_close(Fl_Widget* w, void*v) {
	// The close button can only be clicked at certain times in the closure process
	// when Fl::wait() is called.
	if (closing_) {
		status_->misc_status(ST_WARNING, "ZZALOG: Already closing!");
	}
	else {
		closing_ = true;
		// Stop the ticker
		ticker_->stop_all();
		status_->close();
		Fl::check();
		status_->misc_status(ST_NOTE, "ZZALOG: Closing...");
		// Currently modifying a (potentially new) record
		if (book_ && (book_->is_dirty_record(book_->get_record()) || book_->new_record()) ) {
			fl_beep(FL_BEEP_QUESTION);
			switch (fl_choice("You are currently modifying a record? Save or Quit?", "Save", "Quit", nullptr)) {
			case 0:
				// Save
				status_->misc_status(ST_NOTE, "ZZALOG: Saving current open record");
				qso_manager_->end_qso();
				break;
			case 1:
				// Quit - delete any new record
				status_->misc_status(ST_WARNING, "ZZALOG: Quitting current unsaved record");
				book_->delete_record(book_->new_record());
				break;
			}
		}
		// Wait for auto-import of files to complete
		if (import_data_) {
			if (!import_data_->update_complete()) {
				fl_beep(FL_BEEP_QUESTION);
				switch (fl_choice("There is an import in process. Do you want to let it finish or abandon it?", "Finish", "Abandon", nullptr)) {
				case 0:
					// Gracefully wait for import to complete
					status_->misc_status(ST_NOTE, "ZZALOG: Allowing current import to complete before closing");
					import_data_->stop_update(false);
					while (!import_data_->update_complete()) Fl::check();
					break;
				case 1:
					// Immediately stop the import
					status_->misc_status(ST_WARNING, "ZZALOG: Abandonimg current import");
					import_data_->stop_update(true);
					break;
				}
			}
		}
		// Wait for eQSL card downloads - user can cancel
		if (eqsl_handler_ && eqsl_handler_->requests_queued()) {
			fl_beep(FL_BEEP_QUESTION);
			switch (fl_choice("There are outstanding eQSL card image requests. Do you want to cancel download, wait or cancel exit?", "Cancel download", "Wait", "Cancel exit")) {
			case 0:
				// Cancel the download immediately
				status_->misc_status(ST_WARNING, "ZZALOG: Abandonning outstanding card image fetches");
				eqsl_handler_->enable_fetch(eqsl_handler::EQ_ABANDON);
				break;
			case 1:
				// Wait for the request std::queue to empty
				status_->misc_status(ST_NOTE, "ZZALOG: Continuing card image download before closing");
				while (eqsl_handler_->requests_queued()) Fl::check();
				break;
			case 2:
				// Cancel Exit - don't doing anything else
				status_->misc_status(ST_WARNING, "ZZALOG: Abandoning close down!");
				closing_ = false;
				return;
			}
		}

		// Check the book needs saving
		if (book_ && (book_->is_dirty())) {
			fl_beep(FL_BEEP_QUESTION);
			switch (fl_choice("Book has been modified. Do you want to save and exit, exit or cancel exit?", "Exit", "Save && Exit", "Cancel Exit")) {
			case 0:
				// Exit
				status_->misc_status(ST_WARNING, "ZZALOG: Closing without saving recent changes");
				break;
			case 1:
				status_->misc_status(ST_NOTE, "ZZALOG: Saving changes before closing");
				// Save and Exit
				if (READ_ONLY) {
					// Open the Save As dialog and save
					menu_bar::cb_mi_file_saveas(w, (void*)OT_MAIN);
				}
				else {
					// Save the file
					menu_bar::cb_mi_file_save(w, (void*)OT_MAIN);
				}
				break;
			case 2:
				// Cancel Exit - don't doing anything else
				status_->misc_status(ST_WARNING, "ZZALOG: Abandoning close down");
				closing_ = false;
				return;
			}
		}

		// Flush the mirror/target versions
		if (book_) book_->flush_data();

		// Save the window position
		zc_settings top_settings;
		zc_settings view_settings(&top_settings, "Views");
		zc_settings main_settings(&view_settings, "Main Window");
		main_settings.set("Left", main_window_->x_root());
		main_settings.set("Top", main_window_->y_root());
		main_settings.set("Width", main_window_->w());
		main_settings.set("Height", main_window_->h());
		// Save the banner size
		zc_settings banner_settings(&view_settings, "Banner");
		int bw = status_->get_banner()->w();
		int bh = status_->get_banner()->h();
		banner_settings.set("Width", bw);
		banner_settings.set("Height", bh);
		// Save the International Characters dialog position
		zc_settings intl_settings(&view_settings, "International Characters");
		if (intl_dialog_) {
			intl_settings.set("Left", intl_dialog_->x_root());
			intl_settings.set("Top", intl_dialog_->y_root());
			intl_settings.set<bool>("Open Automatically", intl_dialog_->visible());
		}
		// Save the scratchpad position
		zc_settings scratchpad_settings(&view_settings, "Scratchpad");
		if (scratchpad_) {
			scratchpad_settings.set("Left", scratchpad_->x_root());
			scratchpad_settings.set("Top", scratchpad_->y_root());
			scratchpad_settings.set<bool>("Open Automatically", scratchpad_->visible());
		}

		// Save sticky switches
		save_switches();

		// Hide all the open windows - this will allow Fl to close the app.
		Fl_Window* wx = Fl::first_window();
		for (; wx; wx = Fl::first_window()) {
			// Keep the banner showing if we need to see a severe or fatal error.
			wx->hide();
		}
	}
}

void cb_bn_close(Fl_Window* w, void* v) {
	cb_bn_close((Fl_Widget*)w, v);
}

// Callback to parse arguments
// See show_help() for meaning of switches
int cb_args(int argc, char** argv, int& i) {
	int i_orig = i;
	// auto save
	if (strcmp("-a", argv[i]) == 0 || strcmp("--auto_save", argv[i]) == 0) {
		AUTO_SAVE = true;
		AUTO_SAVE_S = true;
		i += 1;
	}
	// Debug
	else if (strcmp("-d", argv[i]) == 0 || strcmp("--debug", argv[i]) == 0) {
		i += 1;
		bool debugs = true;
		while (debugs && i < argc) {
			int save_i = i;
			if (strcmp("c", argv[i]) == 0 || strcmp("curl", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_CURL);
				i += 1;
			}
			else if (strcmp("noc", argv[i]) == 0 || strcmp("nocurl", argv[i]) == 0) {
				zc_app::clear_debug(DEBUG_CURL);
				i += 1;
			}
			else if (strcmp("d", argv[i]) == 0 || strcmp("decode", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_PARSE);
				i += 1;
			}
			else if (strcmp("p", argv[i]) == 0 || strcmp("production", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_TEST_PRODUCT);
				i += 1;
			}
			else if (strcmp("s", argv[i]) == 0 || strcmp("status", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_STATUS);
				i += 1;
			}
			else if (strncmp("h=", argv[i], 2) == 0) {
				int v = atoi(argv[i] + 2);
				HAMLIB_DEBUG_LEVEL = (rig_debug_level_e)v;
				i += 1;
			}
			else if (strncmp("hamlib=", argv[i], 7) == 0) {
				int v = atoi(argv[i] + 7);
				HAMLIB_DEBUG_LEVEL = (rig_debug_level_e)v;
				i += 1;
			}
			else if (strcmp("k", argv[i]) == 0 || strcmp("socket", argv[i]) ==0) {
				zc_app::set_debug(DEBUG_SOCKET);
				i += 1;
			}
			else if (strcmp("m", argv[i]) == 0 || strcmp("mod", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_MOD_STATUS);
				i += 1;
			}
			else if (strcmp("q", argv[i]) == 0 || strcmp("quick", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_QUICK);
				i += 1;
			}
			else if (strcmp("r", argv[i]) == 0 || strcmp("run", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_RIGS);
				i += 1;
			}
			else if (strcmp("s", argv[i]) == 0 || strcmp("status", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_STATUS);
				i += 1;
			}
			else if (strcmp("t", argv[i]) == 0 || strcmp("threads", argv[i]) == 0) {
				zc_app::set_debug(DEBUG_THREADS);
				i += 1;
			}
			else if (strcmp("not", argv[i]) == 0 || strcmp("nothreads", argv[i]) == 0) {
				zc_app::clear_debug(DEBUG_THREADS);
				i += 1;
			}
			else if (strcmp("x", argv[i]) == 0 || strcmp("xmlrpc", argv[i]) ==0) {
				zc_app::set_debug(DEBUG_XMLRPC);
				i += 1;
			}
			else if (argv[i] && argv[i][0] == '-') {
				// Next switch - stop processing debug arguments
				debugs = false;
			}
			else
			{
				printf("Unexpected debug argument %s - ignored\n", argv[i]);
				i += 1;
			}
			// Not processed any parameter
			if (i == save_i) debugs = false;
		}
	}
	// New file
	else if (strcmp("-e", argv[i]) == 0 || strcmp("--new", argv[i]) == 0) {
		NEW_BOOK = true;
		i += 1;
	}
	// Help
	else if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
		i += 1;
		if (i < argc) {
			if (strcmp("html", argv[i]) == 0) {
				HELP_HTML = true;
				i += 1;
			}
			else if (strcmp("pdf", argv[i]) == 0) {
				HELP_PDF = true;
				i += 1;
			}
			else {
				HELP = true;
			}
		}
		else {
			HELP = true;
		}
	}
	// Dark
	else if (strcmp("-k", argv[i]) == 0 || strcmp("--dark", argv[i]) == 0) {
		DARK = true;
		DARK_S = true;
		i += 1;
	}
	// Dark
	else if (strcmp("-l", argv[i]) == 0 || strcmp("--light", argv[i]) == 0) {
		DARK = false;
		DARK_S = true;
		i += 1;
	}
	// Resume session
	else if (strcmp("-m", argv[i]) == 0 || strcmp("--resume", argv[i]) == 0) {
		RESUME_SESSION = true;
		i += 1;
	}
	// auto upload
	else if (strcmp("-n", argv[i]) == 0 || strcmp("--noisy", argv[i]) == 0) {
		AUTO_UPLOAD = true;
		AUTO_UPLOAD_S = true;
		i += 1;
	}
	// Private log - do not update recent files
	else if (strcmp("-p", argv[i]) == 0 || strcmp("--private", argv[i]) == 0) {
		PRIVATE = true;
		i += 1;
	}
	// No auto upload
	else if (strcmp("-q", argv[i]) == 0 || strcmp("--quiet", argv[i]) == 0) {
		AUTO_UPLOAD = false;
		AUTO_UPLOAD_S = true;
		i += 1;
	}
	// Look for read_only (-r or --read_only)
	else if (strcmp("-r", argv[i]) == 0 || strcmp("--read_only", argv[i]) == 0) {
		READ_ONLY = true;
		i += 1;
	}
	// Look for start rigs (-s or --start)
	else if (strcmp("-s", argv[i]) == 0 || strcmp("--start", argv[i]) == 0) {
		i += 1;
		if (argv[i]) {
			if (strcmp("all", argv[i]) == 0) {
				START_ALL_RIGS = true;
				i += 1;
			}
			else {
				// Remove the rig if already listed to prevent it being added twice.
				START_RIGS.remove(argv[i]);
				START_RIGS.push_back(argv[i]);
				i += 1;
			}
		}
		else {
			printf("No rig specified after %s - ignored\n", argv[i-1]);
		}
	}
	// Look for test mode (-t or --test) 
	else if (strcmp("-t", argv[i]) == 0 || strcmp("--test", argv[i]) == 0) {
		AUTO_UPLOAD = false;
		AUTO_SAVE = false;
		AUTO_UPLOAD_S = true;
		AUTO_SAVE_S = true;
		i += 1;
	}
	// Look for normal mode (-u or --usual) 
	else if (strcmp("-u", argv[i]) == 0 || strcmp("--usual", argv[i]) == 0) {
		AUTO_UPLOAD = true;
		AUTO_SAVE = true;
		AUTO_UPLOAD_S = true;
		AUTO_SAVE_S = true;
		i += 1;
	}
	// Version
	else if (strcmp("-v", argv[i]) == 0 || strcmp("--version", argv[i]) == 0) {
		DISPLAY_VERSION = true;
		i += 1;
	}
	// No auto save
	else if (strcmp("-w", argv[i]) == 0 || strcmp("--wait_save", argv[i]) == 0) {
		AUTO_SAVE = false;
		AUTO_SAVE_S = true;
		i += 1;
	}
	// Reset configuration
	else if (strcmp("-x", argv[i]) == 0 || strcmp("--reset", argv[i]) == 0) {
		i += 1;
		while (argv[i] && argv[i][0] != '-') {
			bool valid = false;
			if (strcmp("adif", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_ADIF;
				valid = true;
			}
			if (strcmp("apps", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_APPS;
				valid = true;
			}
			if (strcmp("bandplan", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_BAND;
				valid = true;
			}
			if (strcmp("contest", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_TEST;
				valid = true;
			}
			if (strcmp("country", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_CTY;
				valid = true;
			}
			if (strcmp("cty_all", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_CALL;
				valid = true;
			}
			if (strcmp("fields", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_FLDS;
				valid = true;
			}
			if (strcmp("icons", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_ICON;
				valid = true;
			}
			if (strcmp("intl", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_INTL;
				valid = true;
			}
			if (strcmp("keys", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_KEYS;
				valid = true;
			}
			if (strcmp("qsl", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_QSL;
				valid = true;
			}
			if (strcmp("reference", argv[i]) == 0) {
				DEBUG_RESET_CONFIG = DEBUG_RESET_REFS;
				valid = true;
			}
			if (strcmp("rigs", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_RIGS;
				valid = true;
			}
			if (strcmp("settings", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_SETT;
				valid = true;
			}
			if (strcmp("station", argv[i]) == 0) {
				DEBUG_RESET_CONFIG |= DEBUG_RESET_STN;
				valid = true;
			}
			if (strcmp("all", argv[i]) == 0) {
				DEBUG_RESET_CONFIG = DEBUG_RESET_ALL;
				valid = true;
			}
			if (!valid) {
				printf("Unexpected reset argument %s - ignored\n", argv[i]);
			}
			i += 1;
		}
	}
	else if (strcmp("-z", argv[i]) == 0 || strcmp("--size", argv[i]) == 0) {
		i += 1;
		if (argv[i]) {
			int size = atoi(argv[i]);
			if (size >= 8 && size <= 12) {
				DEFAULT_FONT_SIZE = size;
				DEFAULT_FONT_SIZE_S = true;
			}
			else {
				printf("Unexpected font size %s - ignored\n", argv[i]);
			}
			i += 1;
		}
		else {
			printf("No font size specified after %s - ignored\n", argv[i-1]);
		}
	}
	if (i == i_orig ) {
		// Not processed any argumant
		if (argv[i] && *argv[i] == '-') {
			int i_fltk = Fl::arg(argc, argv, i);
			if (i_fltk == 0) {
				printf("ZZALOG: Unrecognised switch %s", argv[i]);
				i += 1;
			}
			return i;
		} else {
			arg_filename_ = argv[i];
			i += 1;
			return i;
		}
	} else {
		return argc;
	}
}

// Show help listing
void show_help() {
	char text[] = 
	"zzalog [switches] [filename] \n"
	"\n"
	"switches:\n"
	"\t-a|--auto_save\tDo automatically save each change (sticky)\n"
  	"\t-d|--debug [mode...]\n"
		"\t\tc|curl\tincrease verbosity from libcurl\n"
		"\t\t\tnoc|nocurl\n"
		"\t\td|decode\tShow callsign decoding\n"
		"\t\th=N|hamlib=N\tSet hamlib debug level (default ERRORS)\n"
		"\t\tk|socket\tPrint socket traffic\n"
		"\t\tm|mods\tPrint messages when make QSOs dirty or clean\n"
		"\t\to|override\tOverride automatic development detection\n"
		"\t\tq|quick\tShorten long timeout and polling intervals\n"
		"\t\tr|rig\tPrint rig diagnostics\n"
		"\t\ts|status\tCopy status messages to the terminal\n"
		"\t\tt|threads\tProvide debug tracing on thread use\n"
		"\t\t\tnot|nothreads\n"
		"\t\tx|xmlrpc\tPrint XMLRPC requests and responses\n"
	"\t-e|--new\tCreate new file\n"
	"\t-h|--help\tshow this help message\n"
	"\t-h|--help [format]\tOpen user guide in specified format\n"
		"\t\thtml\tOpen the user guide in HTML format\n"
		"\t\tpdf\tOpen the user guide in PDF format\n"
	"\t-k|--dark\tDark mode (sticky)\n"
	"\t-l|--light\tLight mode (sticky)\n"
	"\t-m|--resume\tResume the previous session\n"
	"\t-n|--noisy\tDo publish QSOs to online sites (sticky)\n"
	"\t-p|--private\tDo not update recent files list\n"
	"\t-q|--quiet\tDo not publish QSOs to online sites (sticky)\n"
	"\t-r|--read_only\tOpen file in read only mode\n"
	"\t-s|--start [rig]...\tStart specified rig(s) (more than 1 allowed)\n"
		"\t\tall\tStart all rigs\n"
	"\t-t|--test\tTest mode: infers -q -w\n"
	"\t-u|--usual\tNormal mode: infers -a -n\n"
	"\t-v|--version\tDisplay version details\n"
	"\t-w|--wait_save\tDo not automatically save each change (sticky)\n"
	"\t-x|--reset [data]...\tReset configuration data (more than 1 allowed\n"
		"\t\tadif\tADIF specification file (all.json)\n"
		"\t\tapps\tApps configuration file (apps.json)\n"
		"\t\tbandplan\tBand-plan data (band_plan.json)\n"
		"\t\tcontest\tContest data (contests.json)\n"
		"\t\tcountry\tCountry data (cty.json)\n"
		"\t\tcty_all\tCountry data (all files)\n"
		"\t\tfields\tFields data (fields.json)\n"
		"\t\ticons\tToolbar icons (various)\n"
		"\t\tintl\tInternational character set (intl_chars.txt)\n"
		"\t\tkeys\tReference data API keys (keys.json)\n"
		"\t\tqsl\tQSL Server configuration and image design (qsl.json)\n"
		"\t\treference\tReference data (adif bandplan coungtry icons intl keys)\n"
		"\t\trigs\tRig configuration data (rigs.json)\n"
		"\t\tsettings\tZZALOG configuration (ZZALOG.json)\n"
		"\t\tstation\tOperator/QTH/Callsign configuration (station.json)\n"
		"\t\tall\tAll files\n"
	"\t-z|--size [size]\tSet default font size (sticky)\n"
	"\n";
	printf(text);
	printf(sticky_message_.c_str());
	printf("\n");
}

// Use supplied argument, or read the latest file from settings or open file chooser if that's an empty std::string
std::string get_file(char* arg_filename) {
	std::string result = "";
	if (!arg_filename || !(*arg_filename)) {
		std::string filename = "";
		if (recent_files_.size()) filename = recent_files_.front();
		if (!filename.length()) {
			status_->misc_status(ST_WARNING, "ZZALOG: No log file - assuming a new installation.");
			stn_default defaults = stn_data_->defaults();
			std::string def_filename = zc::to_lower(defaults.callsign) + ".adi";
			Fl_Native_File_Chooser* chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
			chooser->title("Select log file name");
			chooser->preset_file(def_filename.c_str());
			chooser->filter("ADI Files\t*.adi\nADX Files\t*.adx");
			if (chooser->show() == 0) {
				result = chooser->filename();
			}
			delete chooser;
			NEW_BOOK = true;
		}
		else {
			result = filename;
		}
	}
	else {
		result = arg_filename;
	}
	return result;
}

// Add some global properties
void add_properties() {
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	zc_settings tip_settings(&view_settings, "Tooltip");
	Fl_Font font;
	Fl_Fontsize size;
	float duration;
	tip_settings.get("Duration", duration, (float)zc::TIP_SHOW);
	tip_settings.get("Font Name", font, 0);
	tip_settings.get("Font Size", size, FL_NORMAL_SIZE);
	// Set the default tooltip properties
	Fl_Tooltip::size(size);
	Fl_Tooltip::font(font);
	Fl_Tooltip::delay(duration);
}

// Get the recent files from settings
void recent_files() {
	recent_files_.clear();
	zc_settings top_settings;
	zc_settings behav_settings(&top_settings, "Behaviour");
	behav_settings.get("Recent Files", recent_files_, {});
	if (recent_files_.size() > 4) {
		recent_files_.resize(4);
	}
}

// read in the prefix and adif reference data
void add_data() {
	// Note closing can get set during any of the below actions.
	if (!closing_) {
		keyring_ = new keyring;
	}
	if (!closing_) {
		// add ADIF specification data.
		spec_data_ = new spec_data;
		// Check if loaded
		if (!spec_data_->valid()) {
			// This sets a callback to close the app
			status_->misc_status(ST_FATAL, "Do not have a valid ADIF reference - check installation");
			Fl::check();
		}
		else {
			// This can only be done once it has been fully created
			spec_data_->process_bands();
			// Draw the specification view
			((spec_tree*)tabbed_forms_->get_view(OT_ADIF))->populate_tree(false);
		}
	}
	if (!closing_) {
		if (!club_handler_) club_handler_ = new club_handler;
		// Get pfx_data
		cty_data_ = new cty_data;
		tabbed_forms_->update_views(nullptr, HT_NEW_CTY, -1);
	}
	// Add the Station details database - needs spec_data_ and cty_data_
	if (!closing_) {
		stn_data_ = new stn_data;
		stn_window_ = new stn_window();
		stn_window_->hide();
		stn_data_->load_data();
		if (status_ && status_->get_banner()) {
			status_->get_banner()->set_banner_text(stn_data_->current().callsign.c_str(), FL_FOREGROUND_COLOR);
		}
	}
	// And band plan data
	if (!closing_) {
		band_data_ = new band_data;
	}
	// Add the QSL design data
	if (!closing_) {
		qsl_dataset_ = new qsl_dataset;
	}
	// Add the contest details database
	if (!closing_) {
		contest_data_ = new contest_data;
	}
}

// read in the log data
void add_book(char* arg) {
	if (!closing_) {
		// Create the book options and set them in the forms
		book_ = new book;
		navigation_book_ = book_;
		import_data_ = new import_data;
		extract_data_ = new extract_data;
		deleted_records_ = new book;
		deleted_records_->book_type(OT_DELETED);
		// Tell the views that a book now exists
		tabbed_forms_->books();

		std::string log_file;

		if (!NEW_BOOK || arg_filename_) {
			// Get filename and load the data
			log_file = get_file(arg);

			// Now try and load the file
			if (!book_->load_data(log_file)) {
				return;
			}
			status_->misc_status(ST_OK, "LOG: Loaded logbook (%s)", book_->get_mirror_use().c_str());
			backup_file();
			set_recent_file(log_file);
		}
		if (NEW_BOOK) {
			if (!book_->store_data(log_file, true)) {
				status_->misc_status(ST_ERROR, "LOG: Failed to create %s", log_file.c_str());
			}
		}
	}
}

// Add the various interface handlers
void add_qsl_handlers() {
	if (!closing_) {
		// URL handler - basic HTML POST and GET
		if (url_handler_ == nullptr) url_handler_ = new zc_url_handler;
		// eQSL - accesses the appropriate URLs to upload and download eQSL data
		if (eqsl_handler_ == nullptr) eqsl_handler_ = new eqsl_handler;
		// LotW - accesses the appropriate URL to download data, TQSL to sign and upload data
		if (lotw_handler_ == nullptr) lotw_handler_ = new lotw_handler;
		// QRZ.com - accesses the appropriate URL to get information about the other station
		if (qrz_handler_ == nullptr) qrz_handler_ = new qrz_handler;
		// ClubLog handler
		if (club_handler_ == nullptr) club_handler_ = new club_handler;
		// WSJT-X server
		if (wsjtx_handler_ == nullptr) {
			wsjtx_handler_ = new wsjtx_handler;
		}
		// FLLOG emulator
		if (fldigi_handler_ == nullptr) fldigi_handler_ = new fldigi_handler;
		// Weather handler
		if (wx_handler_ == nullptr) wx_handler_ = new wx_handler;
	}
}

// Add operating qso_manager (AKA "Dashboard")
void add_dashboard() {
	if (!closing_) {
		char l[128];
		std::string version = APP_VERSION;
#ifdef _DEBUG
		version += " DEBUG";
#endif
		if (!qso_manager_) {
			snprintf(l, sizeof(l), "%s %s: Operating Dashboard", APP_NAME.c_str(), version.c_str());
			qso_manager_ = new qso_manager(10, 10);
			qso_manager_->copy_label(l);
		}
		status_->misc_status(ST_NOTE, "DASH: Opened");
		qso_manager_->hide();
		// Add the scratchpad
		scratchpad_ = new scratchpad;
		snprintf(l, sizeof(l), "%s %s: Operating Scratchpad", APP_NAME.c_str(), version.c_str());
		scratchpad_->copy_label(l);

		// Add intl dialog
		intl_dialog_ = new intl_dialog;
		snprintf(l, sizeof(l), "%s %s: International Characters", APP_NAME.c_str(), version.c_str());
		intl_dialog_->copy_label(l);

	}
}

// Set the text in the main window label
void main_window_label(const std::string& text) {
	// e.g. ZZALOG 3.0.0: <filename> - APP_VERSION includes (Debug) if compiled under _DEBUG
	std::string label = APP_NAME + " " + APP_VERSION;
#ifdef _DEBUG
	label += " DEBUG";
#endif
	label += ": " + text;
	main_window_->copy_label(label.c_str());
}

// Create the main window
void create_window() {
	// Create the main window
	main_window_ = new main_window(WIDTH, HEIGHT);
	main_window_label("");
	// add callback to intercept close command
	main_window_->callback(cb_bn_close);
	main_window_->hide();

}

// Add all the widgets: menu, status and tool bars, and view displays
void add_widgets(int& curr_y) {
	// The menu: disable it until all the data is loaded
	menu_bar_ = new menu_bar(0, curr_y, WIDTH, MENU_HEIGHT);
	main_window_->add(menu_bar_);
	menu_bar_->enable(false);
	curr_y += menu_bar_->h();
	// Toolbar - image buttons representing a number of menu and other commands - disable all menu related buttons
	toolbar_ = new toolbar(0, curr_y, WIDTH, TOOL_HEIGHT);
	main_window_->add(toolbar_);
	toolbar_->update_items();
	curr_y += toolbar_->h();
	// The main views - this is a set of tabs with each view
	tabbed_forms_ = new tabbed_forms(0, curr_y, WIDTH, HEIGHT - curr_y - FOOT_HEIGHT);
	main_window_->add(tabbed_forms_);
	curr_y += tabbed_forms_->h();
	// Add a footer (with copyright
	Fl_Box* footer = new Fl_Box(0, curr_y, WIDTH, FOOT_HEIGHT);
	footer->copy_label(std::string(COPYRIGHT + " " + CONTACT + "    ").c_str());
	footer->labelsize(FL_NORMAL_SIZE - 1);
	footer->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	main_window_->add(footer);
	// Display the main window. Don't show it until it's been resized
	main_window_->end();
}

// now resize the main window
void resize_window() {
	// Get the saved size and position of the window from the settings
	int left;
	int width;
	int top;
	int height;
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	zc_settings main_settings(&view_settings, "Main Window");
	main_settings.get<int>("Left", left, 0);
	main_settings.get<int>("Top", top, 100);
	main_settings.get<int>("Width", width, WIDTH);
	main_settings.get<int>("Height", height, HEIGHT);
	// Only allow the views to resize fully - the bars will resize horizontally
	main_window_->resizable(tabbed_forms_);
	// Get minimum resizing from all the children - horizontal limited by views and toolbar
	int min_w = std::max<int>(tabbed_forms_->min_w(), toolbar_->min_w());
	// Vertical limited by view, the bars remain a fixed height
	int min_h = tabbed_forms_->min_h() + toolbar_->h() + menu_bar_->h();
	main_window_->size_range(min_w, min_h);
	// Set the size to the setting or minimum specified by the view + bars if that's larger
	int rx = left;
	int ry = top;
	int rw = std::max<int>(min_w, width);
	int rh = std::max<int>(min_h, height);
	int sx, sy, sw, sh;
	Fl::screen_work_area(sx, sy, sw, sh);
	if (rx < sx) rx = sx;
	else if (rx + rw > sx + sw) rx = std::max<int>(0, sx + sw - rw);
	if (ry < sy) ry = sy;
	else if (ry + rh > sy + sh) ry = std::max<int>(20, sy + sh - rh);
	main_window_->resize(rx, ry, rw, rh);
}

// Tidy memory
void tidy() {
	// Tidy memory - this is not perfect
	// From inspection of the code - calling this a second time frees the memory
	fl_message_title_default(nullptr);
	delete wx_handler_;
	delete config_;
	delete qso_manager_;
	delete rig_data_;
	delete wsjtx_handler_;
	delete club_handler_;
	delete qrz_handler_;
	delete lotw_handler_;
	delete eqsl_handler_;
	delete url_handler_;
	delete fldigi_handler_;
	delete extract_data_;
	delete import_data_;
	delete book_;
	delete contest_data_;
	delete stn_data_;
	delete qsl_dataset_;
	delete band_data_;
	delete intl_dialog_;
	// This will be used in toolbar_
	intl_dialog_ = nullptr;
	delete scratchpad_;
	delete spec_data_;
	delete keyring_;
	delete cty_data_;
	delete tabbed_forms_;
	delete toolbar_;
	delete fields_;
	if (closing_) status_->misc_status(ST_OK, "ZZALOG: Closed");
	delete status_;
	delete menu_bar_;
	delete main_window_;
}

// Map argument letter to colour name
std::map<uint8_t, std::string> colours = {
	{ 'n', "None" },
	{ 'r', "Red" },
	{ 'g', "Green" },
	{ 'b', "Blue" },
	{ 'm', "Magenta" },
	{ 'c', "Cyan" },
	{ 'y', "Yellow" } 
};

// Display the arguments in the status log
void print_args(int argc, char** argv) {
	// Create a string to hold all the info
	int length = 20;
	for (int i = 0; i < argc; i++) {
		length += strlen(argv[i]);
	}
	char message[256];
	memset(message, 0, sizeof(message));
	// Generate the string
	strcpy(message, "ZZALOG: ");
	for (int i = 0; i < argc; i++) {
		strcat(message, argv[i]);
		strcat(message, " ");
	}
	strcat(message, "Started");
	status_->misc_status(ST_NOTE, message);
	snprintf(message, sizeof(message), "ZZALOG: %s %s", 
		APP_NAME.c_str(), APP_VERSION.c_str());
	status_->misc_status(ST_NOTE, message);
	if (zc_app::debug(DEBUG_DEVELOPMENT)) {
		status_->misc_status(ST_WARNING, "ZZALOG: Development mode");
	}
	snprintf(message, sizeof(message), "ZZALOG: Compiled %s", APP_TIMESTAMP.c_str());
	status_->misc_status(ST_NOTE, message);

	if (AUTO_SAVE) status_->misc_status(ST_NOTE, "ZZALOG: -a - QSOs being saved automatically");
	if (zc_app::debug(DEBUG_CURL)) status_->misc_status(ST_NOTE, "ZZALOG: -d c - Displaying more verbosity from libcurl");
	snprintf(message, sizeof(message), "ZZALOG: -d h=%d - Hamlib debug level %d", 
		(int)HAMLIB_DEBUG_LEVEL, (int)HAMLIB_DEBUG_LEVEL);
	status_->misc_status(ST_NOTE, message);
	if (zc_app::debug(DEBUG_SOCKET)) status_->misc_status(ST_NOTE, "ZZALOG: -d k - Displaying socket packets");
	if (zc_app::debug(DEBUG_MOD_STATUS)) status_->misc_status(ST_NOTE, "ZZALOG: -d m - Displaying QSO dirty status");
	if (zc_app::debug(DEBUG_QUICK)) status_->misc_status(ST_WARNING, "ZZALOG: -d q - Reducing periods of some reguat events");
	if (zc_app::debug(DEBUG_THREADS)) status_->misc_status(ST_NOTE, "ZZALOG: -d t - Displaying thread debug messages");
    if (zc_app::debug(DEBUG_XMLRPC)) status_->misc_status(ST_NOTE, "ZZALOG: -d x - Displaying XMLRPC requests/responses");
	if (NEW_BOOK && !arg_filename_) status_->misc_status(ST_NOTE, "ZZALOG: -e - Starting with empty file");
	if (NEW_BOOK && arg_filename_) status_->misc_status(ST_WARNING, "ZZALOG: -e - filename specified, switch ignored");
	if (DARK) status_->misc_status(ST_NOTE, "ZZALOG: -k - Opening in dark mode");
	if (!DARK) status_->misc_status(ST_NOTE, "ZZALOG: -l - Opening in normal FLTK colours");
	if (RESUME_SESSION) status_->misc_status(ST_NOTE, "ZZALOG: -m - Resuming previous session");
	if (AUTO_UPLOAD) status_->misc_status(ST_NOTE, "ZZALOG: -n - QSOs uploaded to QSL sites automatically");
    if (START_ALL_RIGS) status_->misc_status(ST_NOTE, "ZZALOG: -s all - All rigs auto-connecting");
	else if (START_RIGS.size()) {
		std::string rigs = "";
		for (auto& rig : START_RIGS) {
			rigs += rig + " ";
		}
		status_->misc_status(ST_NOTE, "ZZALOG: -s %s - Auto-connecting specified rigs", rigs.c_str());
	}
	if (PRIVATE) status_->misc_status(ST_WARNING, "ZZALOG: -p - This file not being noted on recent files list");
	if (!AUTO_UPLOAD) status_->misc_status(ST_WARNING, "ZZALOG: -q - QSOs are not being uploaded to QSL sites");
	if (READ_ONLY) status_->misc_status(ST_WARNING, "ZZALOG: -r - File opened read-only");
	if (!AUTO_SAVE) status_->misc_status(ST_WARNING, "ZZALOG: -w - QSOs are not being saved automatically");
	snprintf(message, sizeof(message), "ZZALOG: -x (value = %x) - Reset file (bit signficant)",
		DEBUG_RESET_CONFIG);
	if (DEBUG_RESET_CONFIG) status_->misc_status(ST_WARNING, message);
	status_->misc_status(ST_NOTE, "ZZALOG: -z %d - Default font size %d", DEFAULT_FONT_SIZE, DEFAULT_FONT_SIZE);
}

// Returns true if record is within current session.
bool in_current_session(record* this_record) {
	return difftime(this_record->timestamp(), session_start_) >= 0;
}

// Customise FLTK feature
void customise_fltk(int font_size) {
	zc::customise_fltk(font_size);
	// Set foreground and background colours
	if (DARK) {
		Fl::foreground(240, 240, 240);             // 15/16 White
		Fl::background2(16, 16, 16);               // 1/16 white
		Fl::background(32, 32, 32);                // 1/8 white
	}
	else {
		Fl::foreground(16, 16, 16);                // 1/16 white
		Fl::background2(240, 240, 240);            // 15/16 white
		Fl::background(192, 192, 192);             // 3/4 white
	}
}

// Some switches get saved between sessions - so-called sticky switches
void read_saved_switches() {
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	zc_settings overall_settings(&view_settings, "Overall");
	zc_settings behav_settings(&top_settings, "Behaviour");
	// Read all the sticky switches
	char msg[128];
	strcpy(msg, "ZZALOG: Sticky switches: ");
	if (!DARK_S) {
		overall_settings.get<bool>("Dark Mode", DARK, false);
		if (DARK) strcat(msg, "-k ");
		else strcat(msg, "-l ");
	}
	if (!AUTO_UPLOAD_S) {
		behav_settings.get<bool>("Update per QSO", AUTO_UPLOAD, false);
		if (AUTO_UPLOAD) strcat(msg, "-n ");
		else strcat(msg, "-q ");
	}
	if (!AUTO_SAVE_S) {
		behav_settings.get<bool>("Save per QSO", AUTO_SAVE, false);
		if (AUTO_SAVE) strcat(msg, "-a ");
		else strcat(msg, "-w ");
	}
	if (!DEFAULT_FONT_SIZE_S) {
		overall_settings.get<int>("Default Font Size", DEFAULT_FONT_SIZE, DEFAULT_DEFAULT_SIZE);
		char font_msg[32];
		snprintf(font_msg, sizeof(font_msg), "-z %d ", DEFAULT_FONT_SIZE);
		strcat(msg, font_msg);
	}

	sticky_message_ = msg;
}

// Save "sticky" switches
void save_switches() {
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	zc_settings overall_settings(&view_settings, "Overall");
	zc_settings behav_settings(&top_settings, "Behaviour");
	overall_settings.set("Dark Mode", DARK);
	behav_settings.set("Update per QSO", AUTO_UPLOAD);
	behav_settings.set("Save per QSO", AUTO_SAVE);
	overall_settings.set("Default Font Size", DEFAULT_FONT_SIZE);
}

// Load all the hamlib data, and then the rig connection details
void load_rig_data() {
	rig_set_debug(HAMLIB_DEBUG_LEVEL);
	rig_load_all_backends();
	if (!closing_) {
		rig_data_ = new rig_data;
	}
}

// The main app entry point
int main(int argc, char** argv)
{
	// Allow the main thread to respond to Fl::awake() requests
	Fl::lock();
	// 
	printf("%s %s: Loading...\n", APP_NAME.c_str(), APP_VERSION.c_str());
	// Parse command-line arguments - accept FLTK standard arguments and custom ones (in cb_args)
	int i = 1;
	// Clear all debug flags - they are set by the command line arguments
	zc_app::clear_debug(DEBUG_ALL);
	Fl::args(argc, argv, i, cb_args);
	// Set the default data directories
	bool development = false;
	file_holder_ = new zc_file_holder(argv[0], FILE_CONTROL);
	// Read any switches that stick between calls
	read_saved_switches();
	customise_fltk(DEFAULT_FONT_SIZE);

	// Create the ticker first of all
	ticker_ = new zc_ticker();

	if (DISPLAY_VERSION) {
#ifndef WIN32
		std::string version = APP_VERSION;
		// Display version
		printf("%s Version %s Compiled %s\n", 
			APP_NAME.c_str(), 
			version.c_str(),
			APP_TIMESTAMP.c_str());
		curl_version_info_data* data = curl_version_info(CURLVERSION_LAST);
		printf("|-With libraries\n  |- hamlib (%s)\n  |- FLTK (%d.%d.%d)\n  |- Curl (%s)\n  |- BOOST (%d.%d.%d)\n  |- zzacommon (%s)\n",
			rig_version(),
			FL_MAJOR_VERSION, FL_MINOR_VERSION, FL_PATCH_VERSION,
			data->version,
			BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100,
		    ZZACOMMON_VERSION.c_str()); 
#else
		about_dialog* dlg = new about_dialog;
		dlg->show();
		while (dlg->visible()) Fl::check();
#endif
		return 0;
	}
	if (HELP) {
		// Help requested - display help text and exit
		show_help();
		return 0;
	}
	if (HELP_HTML) {
		printf("ZZALOG: Opening HTML user guide\n");
		open_html("index.html");
		return 0;
	}
	if (HELP_PDF) {
		printf("ZZALOG: Opening PDF user guide\n");
		open_pdf();
		return 0;
	}

	// Ctreate status to handle status messages
	auto status_mode = zc_status::HAS_BANNER | zc_status::HAS_LOGFILE;
	if (zc_app::debug(DEBUG_STATUS)) status_mode |= zc_status::HAS_CONSOLE;
	status_ = new zc_status(status_mode, OBJECT_DATA);
	{
		// Limit the scope of top_settings as it will get saved when destroyed.
		zc_settings top_settings;
		zc_settings view_settings(&top_settings, "Views");
		zc_settings banner_settings(&view_settings, "Banner");
		int bw = status_->get_banner()->w();
		int bh = status_->get_banner()->h();
		int bx = status_->get_banner()->x();
		int by = status_->get_banner()->y();
		banner_settings.get("Width", bw, bw);
		banner_settings.get("Height", bh, bh);
		// Set the banner in the middle of the screen.
		int sx, sy, sw, sh;
		Fl::screen_xywh(sx, sy, sw, sh, bx, by);
		int nx = sx + (sw / 2) - (bw / 2);
		int ny = sy + (sh / 2) - (bh / 2);

		status_->get_banner()->resize(nx, ny, bw, bh);
	}
	std::string bt = APP_NAME + " " + APP_VERSION;
	status_->get_banner()->copy_label(bt.c_str());
	status_->get_banner()->set_banner_text("Loading..", FL_GREEN);

	// File info
	file_holder_->display_info();
	// Now display sticky switch message
	status_->misc_status(ST_NOTE, sticky_message_.c_str());	
	print_args(argc, argv);

	// Read the fields data
	fields_ = new fields;

	// Create window
	create_window();
	status_->callback(main_window_, cb_bn_close);
	add_properties();
	recent_files();

	// add the various drawn items
	int curr_y = 0;
	add_widgets(curr_y);
	// Resize the window
	resize_window();
	// Read in reference data - uses progress
	add_data();
	Fl::check();
	// Read in log book data - uses progress - use supplied argument for filename
	add_book(arg_filename_);
	printf("%s\n", main_window_->label());
	Fl::check();
	// Connect to the rig - load all hamlib backends once only here
	load_rig_data();
	// Add qso_manager
	add_dashboard();
	// Add config - uses dynamic enumerated ADIF fields so needs the book loaded. and manager running
	if (!closing_) {
		config_ = new config(WCONFIG, HCONFIG, "Configuration");
		config_->hide();
	}
	// Add qsl_handlers - note add_rig_if() may have added URL handler
	add_qsl_handlers();
	int code = 0;
	// We are now initialised
	initialised_ = true;
	if (!closing_) {
		// Now we have created everything add the windows items to the menu
		// Enable menu so that we can do thigs while waiting for Fllog client to appear
		menu_bar_->add_windows_items();
		menu_bar_->enable(true);
		menu_bar_->redraw();
		// enable menu
		// now show the window
		main_window_->show(argc, argv);
		qso_manager_->show();
		// Run the application until it is closed
		code = Fl::run();
	}
	// Delete everything we've created
	tidy();
	return code;
}

// Copy existing data to back up file.
void backup_file() {
	int num_backups;
	std::string source = book_->get_filename();
	std::string backup = backup_filename(source, num_backups);
	status_->misc_status(ST_NOTE, "BACKUP: Saving %d backups %s", num_backups, backup.c_str());
	// Renaming backups N-1 => N
	int id = num_backups;
	std::string bu_dest = backup + "." + std::to_string(id--);
	boost::system::error_code ec;
	while (id > 1) {
		boost::filesystem::path p_dest(bu_dest);
		std::string bu_src = backup + "." + std::to_string(id--);
		boost::filesystem::path p_src(bu_src);
		boost::filesystem::rename(p_src, p_dest, ec);
		if (ec) {
			status_->misc_status(ST_WARNING, "BACKUP: Error renaming %s - %s", bu_src.c_str(), ec.what().c_str());
		}
		bu_dest = bu_src;
	}
	// Rename backup to backup.1
	boost::filesystem::path p_dest(bu_dest);
	boost::filesystem::path p_src(backup);
	boost::filesystem::rename(p_src, p_dest, ec);
	if (ec) {
		status_->misc_status(ST_WARNING, "BACKUP: Error renaming %s - %s", backup.c_str(), ec.what().c_str());
	}
	// Copy source to backup
	boost::filesystem::path p_origin(source);
	boost::filesystem::copy(p_origin, p_src, ec);
	if (ec) {
		status_->misc_status(ST_WARNING, "BACKUP: Error copying - %s", source.c_str(), ec.what().c_str());
	}
	else {
		status_->misc_status(ST_OK, "BACKUP: %s Backed up %s", source.c_str(), backup.c_str());
	}
}

// Add the current file to the recent files list
void set_recent_file(std::string filename) {
	// Do not add to recent file list if using backup or CLI inihibited
	if (!PRIVATE && !using_backup_ && filename.length()) {

		// Add or move the file to the front of list
		recent_files_.remove(filename);
		recent_files_.push_front(filename);

		// Update recent files in the settings
		zc_settings top_settings;
		zc_settings behav_settings(&top_settings, "Behaviour");
		if (recent_files_.size() > 4) {
			recent_files_.resize(4);
		}
		behav_settings.set("Recent Files", recent_files_);

		menu_bar_->add_recent_files();

	}

}

void open_html(const char* file) {
	// OS dependent code to open a document
	std::string full_filename = file_holder_->get_directory(FDD_DOCUMENTS) +
		"userguide/html/" + std::string(file);
	open_doc(full_filename);
}

void open_pdf() {
	std::string full_filename = file_holder_->get_directory(FDD_DOCUMENTS) +
		"userguide/ZZALOG.pdf";
	open_doc(full_filename);
}

void open_doc(std::string full_filename) {
#ifdef _WIN32
	HINSTANCE result = ShellExecute(NULL, "open", full_filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
	if ((intptr_t)result <= 32) {
		char msg[128];
		snprintf(msg, sizeof(msg), "ZZALOG: Error opening HTML %s. Error code: %d", 
			full_filename.c_str(),
			(int)(intptr_t)result);
		status_->misc_status(ST_ERROR, msg);
	}
#else 
	std::string cmd = "xdg-open \"" + full_filename + "\"";
	int res = system(cmd.c_str());
	if (res != 0) {
		char msg[128];
		snprintf(msg, sizeof(msg), "ZZALOG: Error opening HTML %s. Error code: %d", 
			full_filename.c_str(),
			res);
		status_->misc_status(ST_ERROR, msg);
	}
#endif
}

std::string recent_file(int n) {
	if (recent_files_.size() <= n) {
		return "";
	}
	else {
		int ix = 0;
		for (auto it = recent_files_.begin(); ; it++, ix++) {
			if (ix == n) {
				return *it;
			}
		}
	}
}

void update_paste_target(Fl_Widget* widget) {
	paste_target_ = widget;
	// Notify any interested parties that the paste target has changed
	scratchpad_->update_paste_target();
}