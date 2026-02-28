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
#include "contest_algorithm.h"

#include "contest_scorer.h"
#include "main.h"
#include "qso_manager.h"
#include "record.h"
#include "spec_data.h"
#include "stn_data.h"

#include <map>
#include <string>
#include <vector>

typedef std::vector<std::string> field_list;

//! Map of available algorithms
#include "basic.h"
#include "iaru_hf.h"
std::map< std::string, contest_algorithm* > algorithms_ = {
	{ "Basic", new contests::basic },
	{ "IARU-HF", new contests::iaru_hf }
};

// Constructor - should be overriden by the algoritmic specific
contest_algorithm::contest_algorithm() {
	my_info_ = nullptr;
	scorer_ = nullptr;
	rx_items_.clear();
	tx_items_.clear();

}


contest_algorithm::~contest_algorithm() {
	rx_items_.clear();
	tx_items_.clear();
}

void contest_algorithm::attach(contest_scorer* cs) {
	scorer_ = cs;
	my_info_ = stn_data_->get_qth(qso_manager_->get_default(qso_manager::QTH));
}

void contest_algorithm::set_default_rst(record* qso) {
	std::string contest_mode = spec_data_->dxcc_mode(qso->item("MODE"));
	if (contest_mode == "CW" || contest_mode == "DATA") {
		// CW/Data
		qso->item("RST_SENT", std::string("599"));
	}
	else {
		// Phone
		qso->item("RST_SENT", std::string("59"));
	}

}

// Return all fields used in algorithm
field_list contest_algorithm::fields() {
	std::vector<std::string> result;
	for (auto& it : rx_items_) {
		result.push_back(it);
	}
	for (auto& it : tx_items_) {
		result.push_back(it);
	}
	return result;
}
