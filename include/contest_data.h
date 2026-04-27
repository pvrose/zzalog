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

*/#pragma once

#include "nlohmann/json.hpp"

#include "file_types.h"

#include "zc_file_holder.h"

#include <chrono>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>


class contest_reader;
class contest_score;
class contest_write;
using json = nlohmann::json;

//! Definition of contest timeframe
struct ct_date_t {
	std::chrono::system_clock::time_point start;     //!< Start of the contest period.
	std::chrono::system_clock::time_point finish;    //!< End of the contest period.

	//! Tests if the specified timepoint \p tp is within the contest period.
	bool has_inside(std::chrono::system_clock::time_point tp) const {
		return (tp > start) && (tp < finish);
	}

	//! Constructor - defaults start and finish to a valid timepoint.
	ct_date_t() {
		start = std::chrono::system_clock::now();
		finish = std::chrono::system_clock::now();
	}
};

//! Conversion of ct_date_t to JSON
void to_json(json& j, const ct_date_t& s);
//! Conversion of JSON to ct_data_t
void from_json(const json& j, ct_date_t& s);

//! Definition of the contest.
struct ct_data_t {
	std::string algorithm;        //!< Scoring and exchange algorithm.
	ct_date_t date;          //!< Period of the contest.
};

//! Conversion of ct_data_ to JSON
void to_json(json& j, const ct_data_t& s);
//! Conversion of JSON to ct_data_t
void from_json(const json& j, ct_data_t& s);

//! Amalgamated contest list entry
struct ct_entry_t {
	std::string id;               //!< Identifier for the contest.
	std::string index;            //!< Speciifc instance of the contest (eg 2025)
	ct_data_t* definition;        //!< Contest definition.
};

//! The contest definition database
class contest_data
{
	friend class contest_reader;
	friend class contest_writer;
	friend class contest_dialog;

public:
	//! Constructor.
	contest_data();
	//! Destructor.
	~contest_data();

	//! Get contest data structure for specific contest.
	
	//! \param id Identifier for the contest.
	//! \param ix Identfier of the specific instance.
	//! \param create Create the entry in the database for later editing.
	//! \return contest database entry for the specified contest.
	ct_data_t* get_contest(std::string id, std::string ix, bool create = false);
	//! Get the number of contests registered.

	//! \return number of entries in the contest database.
	int get_contest_count();
	//! Get the contest entry as indexed.
	
	//! \param number the index of the entry.
	ct_entry_t* get_contest_info(int number);
	//! Get all the instance identifiers for specific contest
	
	//! \param id identifier of the contest.
	//! \return the set of instances of this contest.
	std::set<std::string>* get_contest_indices(std::string id);

	//! Get all the available algorithms.
	std::set<std::string>* get_algorithms() {
		return &algorithms_;
	}

	//! Get the definition file for the specified algorithm.
	std::string get_algorithm_file(std::string algorithm) {
		if (algorithm_map_.find(algorithm) != algorithm_map_.end()) {
			return file_holder_->get_filename(algorithm_map_[algorithm]);
		}
		else {
			add_algorithm(algorithm);
			return get_algorithm_file(algorithm);
		}
	}

	//! Add an algorithm to the list of available algorithms.
	void add_algorithm(std::string algorithm);


protected:
	//! Load data from contests.xml into the internal database.
	bool load_data();
	//! Store the internal database into contests.xml.
	bool save_data();
	//! SAve as JSON
	bool save_json(std::ofstream& os);
	//! Save algoriths.json to persist the list of available algorithms.
	bool save_algorithms(std::ofstream& os);
	//! Load as JSON
	bool load_json(std::ifstream& is);
	//! Load algoriths.json to populate the list of available algorithms. 
	//! This does not load the individual algorithm definition files.
	bool load_algorithms(std::ifstream& is);

	// The databases 
	//! Individual contests mapped by ID and index (e.g. year)
	
	//! - <B>Inner map</B> Addressed by instance identifier.
	//! - <B>Outer map</B> Addressed by contest identifier.
	std::map<std::string, std::map<std::string, ct_data_t*> > contests_;
	//! Consolidated database of all contest entries.
	std::vector<ct_entry_t*> contest_infos_;

	//! List of available algorithms.
	std::set<std::string> algorithms_;

	//! Map algorithms to definition files
	std::map<std::string, file_types> algorithm_map_;

};

extern contest_data* contest_data_;