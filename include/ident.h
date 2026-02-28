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
#include <string>

//! Program timestamp - set by CMake
extern std::string APP_TIMESTAMP;
//! Progrsm version - set by CMake
extern std::string APP_VERSION;
//! Program name - set by CMake
extern std::string APP_NAME;
//! Program vendor - set by CMake
extern std::string APP_VENDOR;
//! Program copyright - displayed in all windows.
extern std::string COPYRIGHT;
//! Third-party acknowledgments.
extern std::string PARTY3RD_COPYRIGHT;
//! Contact address for use in FLTK widget labels.
extern std::string CONTACT;
//! Contact address for use in general texts.
extern std::string CONTACT2;
//! Copyright placed in exported data items.
extern std::string DATA_COPYRIGHT;

