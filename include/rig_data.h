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

#include <string>
#include <set>
#include <map>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct hamlib_data_t;

//! This structure provides the configuration data for the rig interface.
struct cat_data_t {
    hamlib_data_t* hamlib = nullptr;   //!< Hamlib API configuration  
    bool use_cat_app = false;          //!< Use another app for accessing CAT interface.
    bool override_hamlib = false;      //!< Override values obtained from CAT interface
    std::string app = "";              //!< Name of the command to launch app
    std::string nickname = "";         //!< Short form of the name used in CAT menu.
    bool auto_start = false;           //!< Automatically start app when ZZALOG starts.
    bool auto_connect = false;         //!< Automatically connect to app after starting it.
	bool auto_pdown = false;           //!< Automatically power down rig when disconnecting.
    double connect_delay = 1.0;        //!< Delay between starting app and connecting (in seconds).
};

void to_json(nlohmann::json& j, const cat_data_t& s);
void from_json(const nlohmann::json& j, cat_data_t& s);

//! This structure configures the use of the rig interface.
struct rig_data_t {          
    int default_app = -1;              //!< Index into cat_data for the default CAT method.
    std::string antenna = "";          //!< Preferred antenna when using this rig.
    bool use_instant_values = false;   //!< Use values just radfrom rig rather than smoothed ones.
    std::vector<cat_data_t*> cat_data; //!< Methods of accessing this particular rig.
};

void to_json(nlohmann::json& j, const rig_data_t& s);
void from_json(const nlohmann::json& j, rig_data_t& s);

//! This class provides the data required for configuring, accessing and using each rig.
class rig_data {

public:
    //! Constructor
    rig_data();
    //! Destructor
    ~rig_data();
    //! Returns reference to the CAT for the \p rig with index \ app. 
    cat_data_t* cat_data(std::string rig, int app = -1);
    //! Returns all the rigs currently supported by this database.
    std::vector<std::string> rigs();
    //! Returns the rig_data_t structure for the specified \p rig.
    rig_data_t* get_rig(std::string rig); 
    

protected:
    //! Load data from rigs.xml
    void load_data();
    //! LOad data from JSON
    bool load_json();
    //! Store data as JSON
    bool store_json();
    //! Configuration data for all rigs.
    std::map<std::string, rig_data_t*> data_;
    //! Load failed.
    bool load_failed_;
};

extern rig_data* rig_data_;
