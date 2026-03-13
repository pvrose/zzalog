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
//! <A class="el" HREF=file:../../zzacommon/api/html/index.html>ZZACOMMON API</A>
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

class band_data;
class band_window;
class book;
class club_handler;
class config;
class contest_data;
class cty_data;
class eqsl_handler;
class extract_data;
class fields;
class fldigi_handler;
class import_data;
class intl_dialog;
class keyring;
class lotw_handler;
class main_window;
class menu_bar;
class qrz_handler;
class qsl_dataset;
class qso_manager;
class record;
class rig_data;
class spec_data;
class zc_status;
class stn_data;
class stn_window;
class tabbed_forms;
class zc_ticker;
class toolbar;
class url_handler;
class wsjtx_handler;
class wx_handler;
class Fl_PNG_Image;
class Fl_Widget;
enum rig_debug_level_e;

// Debug switches
// Operation switches - _S versions used to override sticky switch
//! Automatically upload QSOs to QSL sites -  by "-n"
extern bool AUTO_UPLOAD;
//! Version of \p AUTO_UPLOAD read from settings
extern bool AUTO_UPLOAD_S;
//! Automatically save QSO record after each change -  by "-a"
extern bool AUTO_SAVE;
//! Version of \p AUTO_SAVE read from settings.
extern bool AUTO_SAVE_S;
//! Dark mode: Dark background, light forreground -  by "-k"
extern bool DARK;
//! Version of \p DARK read from settings.
extern bool DARK_S;
//! Print version details instead of running ZZALOG -  by "-v"
extern bool DISPLAY_VERSION;
//! Print command-line interface instead of running ZZALOG -  by "-h"
extern bool HELP;
//! Start with an empty logbook -  by "-e"
extern bool NEW_BOOK;
//! Do not add file to recent file list -  by "-p"
extern bool PRIVATE;
//! Open file in read-only mode -  by "-r"
extern bool READ_ONLY;
//! Resum logging including previous session -  by "-m"
extern bool RESUME_SESSION;
//! Development flag: used to enable/disable features only in development mode ("-g")
extern bool DEVELOPMENT_MODE;
//! Start up off-air - do not connect rigs
extern bool START_OFF_AIR;
//! Open userguide instead of running ZZALOG -  by "-h html"
extern bool HELP_HTML;
//! Open PDF userguide instead of running ZZALOG -  by "-h pdf"
extern bool HELP_PDF;

//! Access to FLTK global attribute to  default text size throughout ZZALOG.
extern int FL_NORMAL_SIZE;

//! main window width
const unsigned int WIDTH = 1000;
//! main window height
const unsigned int HEIGHT = 650;

//! File_holder customisation - debug switches
const uint32_t DEBUG_RESET_ADIF = 1;        //!< Reset all.json (ADIF)
const uint32_t DEBUG_RESET_BAND = 1 << 1;   //!< Reset band_plan.json
const uint32_t DEBUG_RESET_CTY = 1 << 2;    //!< Reset cty.json
const uint32_t DEBUG_RESET_INTL = 1 << 3;   //!< Reset intl_chars.txt
const uint32_t DEBUG_RESET_APPS = 1 << 4;   //!< Reset apps.json
const uint32_t DEBUG_RESET_SETT = 1 << 5;   //!< Reset settings.json
const uint32_t DEBUG_RESET_RIGS = 1 << 6;   //!< Reset rigs.json
const uint32_t DEBUG_RESET_FLDS = 1 << 7;   //!< Reset fields.json
const uint32_t DEBUG_RESET_TEST = 1 << 8;   //!< Reset contests.json
const uint32_t DEBUG_RESET_ICON = 1 << 9;   //!< Reset Icons
const uint32_t DEBUG_RESET_STN = 1 << 10;   //!< Reset station.json
const uint32_t DEBUG_RESET_CTY1 = 1 << 11;    //!< Reset cty.xml
const uint32_t DEBUG_RESET_CTY2 = 1 << 12;    //!< Reset cty.csv
const uint32_t DEBUG_RESET_CTY3 = 1 << 13;    //!< Reset prefix.lst
const uint32_t DEBUG_RESET_CTY4 = 1 << 14;    //!< Reset iso.csv
const uint32_t DEBUG_RESET_QSL = 1 << 15;     //!< Reset qsl.json
const uint32_t DEBUG_RESET_KEYS = 1 << 16;    //!< Reset keys.json
const uint32_t DEBUG_RESET_ALL = 0xffff;    //!< Reset all
const uint32_t DEBUG_RESET_CALL =
	DEBUG_RESET_CTY |
	DEBUG_RESET_CTY1 |
	DEBUG_RESET_CTY2 |
	DEBUG_RESET_CTY3 |
	DEBUG_RESET_CTY4;                       //!< Reset all country files
const uint32_t DEBUG_RESET_REFS =
	    DEBUG_RESET_ADIF |
		DEBUG_RESET_BAND |
		DEBUG_RESET_CALL |
		DEBUG_RESET_INTL |
		DEBUG_RESET_ICON |
		DEBUG_RESET_KEYS;                   //!< Reset all reference data

//! File holder customisations - file tags
enum file_types : uint8_t {
	FILE_ADIF = file_contents_t::FILE_USER,                  //!< ADIF Specification
	FILE_BANDPLAN,                          //!< Band-plan data
	FILE_COUNTRY_CLUB,                      //!< Country data from Clublog.org
	FILE_COUNTRY_CFILES,                    //!< Country data from country-files.com
	FILE_COUNTRY_DXATLAS,                   //!< Country data from DxAtlas
	FILE_COUNTRY_ISO,                       //!< ISO country data
	FILE_COUNTRY,                           //!< Collated country data
	FILE_INTLCHARS,                         //!< International chatacter set
	FILE_ICON_GMAPS,                        //!< Icon for google maps
	FILE_ICON_PDF,                          //!< Icon for PDF
	FILE_ICON_QRZ,                          //!< Icon for QRZ.com
	FILE_APPS,                              //!< Application configuration file
	FILE_RIGS,                              //!< Rig configuration file
	FILE_FIELDS,                            //!< Field usage configuration file
	FILE_CONTEST,                           //!< Contests configuration file
	FILE_SOLAR,                             //!< Solar data (read every hour at most frequent
	FILE_STATION,                           //!< Station configuration file
	FILE_QSL,                               //!< QSL configuration file
	FILE_KEYS,                              //!< On-line API keys (non-user)
};

//! \cond
// Top level data items - these are declared as externals in each .cpp that uses them
extern band_data* band_data_;
extern band_window* band_window_;
extern book* book_;
extern book* navigation_book_;
extern club_handler* club_handler_;
extern config* config_;
extern contest_data* contest_data_;
extern cty_data* cty_data_;
extern eqsl_handler* eqsl_handler_;
extern extract_data* extract_records_;
extern fields* fields_;
extern fldigi_handler* fldigi_handler_;
extern import_data* import_data_;
extern intl_dialog* intl_dialog_;
extern keyring* keyring_;
extern lotw_handler* lotw_handler_;
extern main_window* main_window_;
extern menu_bar* menu_bar_;
extern qrz_handler* qrz_handler_;
extern qsl_dataset* qsl_dataset_;
extern qso_manager* qso_manager_;
extern rig_data* rig_data_;
extern spec_data* spec_data_;
extern stn_data* stn_data_;
extern stn_window* stn_window_;
extern tabbed_forms* tabbed_forms_;
extern zc_ticker* ticker_;
extern toolbar* toolbar_;
extern url_handler* url_handler_;
extern wsjtx_handler* wsjtx_handler_;
extern wx_handler* wx_handler_;

//! List of files most recently opened. Maximum: 4 files. 
extern std::list<std::string> recent_files_;
//! \endcond

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
void main_window_label(std::string text);

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
