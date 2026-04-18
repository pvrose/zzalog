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
#include "basic.h"

#include "contest_algorithm.h"
#include "record.h"
#include "stn_data.h"
#include "zc_fltk.h"

#include <set>
#include <string>
#include <vector>

// Constructor - add algorithmic specific data here
contests::basic::basic() : contest_algorithm() {
	// Add the QSO fields used in scoring and exchanges (RX)
	rx_items_ = { "RST_RCVD", "SRX" };
	// Add the QSO fields used in scoring and exchanges (TX)
	tx_items_ = { "RST_SENT", "STX" };
}

// Algorithm specific method to split text into a number of fields
void contests::basic::parse_exchange(record* qso, std::string text) {
	std::vector<std::string> words;
	zc::split_line(text, words, ' ');
	int ix = 0;
	for (auto& it : rx_items_) {
		qso->item(it, words[ix]);
		ix++;
	}
}

// Algorithm specific method to generate text from a number of fieds
std::string contests::basic::generate_exchange(record* qso) {
	std::string result = "";
	int ix = 0;
	set_default_rst(qso);
	qso->item("MY_DXCC", my_info_->data.at(DXCC_ID));
	for (auto& it : tx_items_) {
		result += qso->item(it);
		ix++;
		if (ix < tx_items_.size()) result += ' ';
	}
	return result;
}

// Algorithm specific method to score an individual QSO
score_result contests::basic::score_qso(record* qso, std::set<std::string>& multipliers) {
	// Multiplier is number of DXCCs worked on each band
	std::string multiplier = qso->item("DXCC") + " " + qso->item("BAND");
	score_result result{};
	if (multipliers.find(multiplier) == multipliers.end()) {
		result.multiplier = 1;
		multipliers.insert(multiplier);
	}
	// QSO points - 1 per QSO in different DXCC
	if (qso->item("DXCC") != my_info_->data.at(DXCC_ID)) {
		result.qso_points = 1;
	}
	return result;
}
