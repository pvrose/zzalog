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

#include "qsl_data.h"

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <FL/Fl.H>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum extract_mode_t : uint8_t;

// Class to manage QSL designs

//! Contains data needed to access logbooks by call on QSL server sites.
struct qsl_call_data {
	bool used;                           //!< Data is used.
	std::string key;                     //!< logbook access key (QRZ.com). Used for Log nickname for eQSL.cc
	unsigned long long last_logid;       //!< Universal QSO record identifier.
	std::string last_download;           //!< Date last downloaded (YYYYMMDD)
};

void to_json(json& j, const qsl_call_data& s);
void from_json(const json& j, qsl_call_data& s);

//! Contains data needed to access QSL server sites.
struct server_data_t {
	bool upload_per_qso{false};          //!< Upload per QSO
	bool enabled{false};                 //!< Access enabled
	std::string user{""};                //!< User-name (or e-mail
	std::string password{""};            //!< Password
	std::string last_downloaded{""};     //!< Date of last download
	bool download_confirmed{false};      //!< Download confirmed as well (eqSL)
	std::string qso_message{""};         //!< Message to add to a QSL card (eQSL)
	std::string swl_message{""};         //!< Message to add to a QSL card - SWL (eQSL)
	std::string export_file{""};         //!< File for uploading (LotW)
	bool use_api{false};                 //!< Use API (QRZ.com)
	bool use_xml{false};                 //!< Use XML (QRZ.com)
	std::string mail_server{""};         //!< Mail server (eMail)
	std::string cc_address{""};          //!< cc Address (eMail)
	std::map<std::string, qsl_call_data*> call_data; //!< API logbook data (QRZ.com, eQSL)
};

//! The container for all data pertinant to QSL card designs and QSL server sites.
class qsl_dataset
{
public:

	//! Constructor.
	qsl_dataset();
	//! Destructor.
	~qsl_dataset();

	//! Return the QSL design associated with the \p callsign and QSL \p type
	qsl_data* get_card(std::string callsign, qsl_data::qsl_type type, bool create);
	//! Get the path to settings/Datapath/QSLs
	std::string get_path();
	//! Returns the server data associated with the \p server (per extract_mode_t)
	server_data_t* get_server_data(std::string server);
	//! Returns QRZ logbook credentials
	qsl_call_data* get_qrz_api(std::string callsign);
	//! Returns set of callsign not wanting electronic QSLs
	std::set<std::string>* get_no_qsl_list();

	//! Store carddesigns
	void save_data();
	//! Save JSON file
	void save_json();
	//! Create server data
	bool new_server(std::string server);
	//! Current server name during save
	static std::string server_name();
	//! \p callsign does not want QSLs
	bool no_qsl(std::string callsign);


protected:
	//! Read card designs
	void load_data();
	//! Read from JSON
	bool load_json();
	//! Get JSON file from settings
	std::string json_file();

	//! QSL card data
	std::map<qsl_data::qsl_type, std::map<std::string, qsl_data*>* > data_;
	//! QSL server data
	std::map<std::string, server_data_t*> server_data_; 
	//! Do not QSL list
	std::set<std::string>  no_qsl_list_;
	//! Path to QSL data
	std::string qsl_path_;
	//! Load failed
	bool load_failed_;
	//! Current server name during save
	static std::string server_name_;
};

extern qsl_dataset* qsl_dataset_;
