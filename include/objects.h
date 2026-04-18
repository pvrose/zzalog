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
#include "zc_drawing.h"
#include <zc_status.h>
//! \file
//! Contains parameters used to present data about various ZZALOG objects

//! \brief The various views and data objects.
enum object_t : uint8_t {
	OT_NONE,             //!< No object (for default use)
	OT_MAIN,             //!< The main log view (book & view)
	OT_RECORD,           //!< The record form (view)
	OT_EXTRACT,          //!< Extracted records (book & view)
	OT_IMPORT,           //!< Imported records (book)
	OT_PREFIX,           //!< Prefix reference view (data & view)
	OT_ADIF,             //!< ADIF specification (data & view)
	OT_REPORT,           //!< Report view (view)
	OT_DXCC,             //!< DXCC Status report (view)
	OT_BAND,             //!< Band-plan data
	OT_CARD,             //!< Used for printing QSL card labels
	OT_WSJTX,            //!< Import from WSJT-X
	OT_RIGS,             //!< Reading Rig data XML file
	OT_RIG,              //!< Connecting a rig
	OT_QSLS,             //!< Redaing QSL data XML file
	OT_STN,              //!< Reading Station data XML file
	OT_CONTEST,          //!< Processing contest data
	OT_EQSL_IMAGE,       //!< Downloading eQSL iamges
	OT_DELETED,		     //!< Deleted records (used in undo/redo)
};

//! \brief Default parameters to use in tab view labels and/or progress clock.
const object_data_map OBJECT_DATA = {
	{ OT_NONE, { "NOTHING", FL_BACKGROUND_COLOR }},
	{ OT_MAIN, { "LOG", FL_GREEN } },
	{ OT_EXTRACT, { "EXTRACT", FL_CYAN } },
	{ OT_IMPORT, { "IMPORT", FL_BLUE } },
	{ OT_RECORD, { "RECORD", FL_MAGENTA } },
	{ OT_PREFIX, { "CTY DATA", FL_YELLOW } },
	{ OT_ADIF, { "ADIF SPEC", fl_color_average(FL_RED, FL_YELLOW, (float)(1.0 / 3.0)) } },
	{ OT_REPORT, { "REPORT", fl_color_average(FL_RED, FL_WHITE, 0.25) } },
	{ OT_DXCC, { "DXCC", FL_YELLOW } },
	{ OT_BAND, { "BAND", COLOUR_CLARET } },
	{ OT_CARD, { "QSL CARD", fl_color_average(FL_BLUE, FL_WHITE, 0.25) } },
	{ OT_WSJTX, { "WSJT-X", COLOUR_NAVY } },
	{ OT_RIGS, { "RIG DATA", COLOUR_PINK } },
	{ OT_RIG, { "RIG", fl_lighter(COLOUR_PINK)}},
	{ OT_QSLS, { "QSL", COLOUR_MAUVE } },
	{ OT_STN, { "STATION DATA", COLOUR_APPLE } },
	{ OT_CONTEST, { "CONTEST", COLOUR_ORANGE } },
	{ OT_EQSL_IMAGE, { "EQSL CARD", fl_darker(FL_YELLOW) } },
	{ OT_DELETED, { "DELETED", FL_DARK_RED } }
};
