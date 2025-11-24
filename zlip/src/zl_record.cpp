#include "zl_record.h"

#include "utils.h"

#include <chrono>
#include <stdexcept>
#include <string>

zl_record::zl_record() {
}
zl_record::~zl_record() {
}
// ! Returns match_result_t between QSO \p record1 and QSO \p record2.
// Copied from record::match_records with json parameters
zl_record::match_result_t zl_record::match_records(const json& qso1, const json& qso2, match_flags_t flags) {
	// return NOMATCH if no record to compare
	if (!qso1.is_array() || !qso2.is_array()) {
		return MT_NOMATCH;
	}
	// Match conditions
	int nondate_mismatch_count = 0; // Important fields mismatch
	int trivial_mismatch_count = 0; // Trivial fields mismatch
	int net_mismatch_count = 0; // QSOs do not form part of a net
	bool dates_match = true;
	bool location_match = true;
	bool swl_match = true;
	bool call_match = true;
	// For each field in this record
	for (auto& element : qso1) {
		for (auto& it : element.items()) {
			std::string field_name = it.key();
			std::string value = it.value();
			// Time/date match
			if (field_name == "QSO_DATE" ||
				field_name == "TIME_ON") {
				dates_match &= items_match(qso1, qso2, field_name);
			}
			else if (
				// Location information matches
				field_name == "CONT" ||
				field_name == "DXCC" ||
				field_name == "GRIDSQUARE" ||
				field_name == "IOTA" ||
				field_name == "PFX") {
				location_match &= items_match(qso1, qso2, field_name);
			}
			else if (
				// Ignore QSL information
				field_name.substr(0, 7) == "QRZCOM_" ||
				field_name.substr(0, 8) == "CLUBLOG_" ||
				field_name == "EQSL_QSLRDATE" ||
				field_name == "EQSL_QSL_RCVD" ||
				field_name == "EQSL_QSLSDATE" ||
				field_name == "EQSL_QSL_SENT" ||
				field_name == "LOTW_QSLRDATE" ||
				field_name == "LOTW_QSL_RCVD" ||
				field_name == "LOTW_QSLSDATE" ||
				field_name == "LOTW_QSL_SENT" ||
				field_name == "QSLRDATE" ||
				field_name == "QSL_RCVD" ||
				field_name == "QSLSDATE" ||
				field_name == "QSL_SENT") {
			}
			else if (
				// Band, mode MUST match
				field_name == "BAND" ||
				field_name == "MODE") {
				if (!(items_match(qso1, qso2, field_name))) {
					swl_match = false;
					nondate_mismatch_count++;
					net_mismatch_count++;
				}
			}
			else if (
				// CALL and STATION_CALLSIGN _MUST_ match - but not for SWL
				field_name == "CALL" ||
				field_name == "STATION_CALLSIGN") {
				if (!(items_match(qso1, qso2, field_name))) {
					nondate_mismatch_count++;
					call_match = false;
				}
			}
			else {
				// Other matches ignored for SWL reports
				if (!(items_match(qso1, qso2, field_name))) {
					trivial_mismatch_count++;
				}
			}
		}
	}
	// All non-trivial fields match - EXACT
	if (dates_match && location_match && nondate_mismatch_count == 0) {
		return MT_EXACT;
	}
	// One or more location field mismatches
	else if (dates_match && !location_match && nondate_mismatch_count == 0) {
		return MT_LOC_MISMATCH;
	}
	// Need more detailed analysis 
	else {
		// The two records overlap
		std::chrono::system_clock::time_point this_on = ctimestamp(qso1);
		std::chrono::system_clock::time_point this_off = ctimestamp(qso1, true);
		std::chrono::system_clock::time_point that_on = ctimestamp(qso2);
		std::chrono::system_clock::time_point that_off = ctimestamp(true);
		std::chrono::seconds min30(1800);
		// Make the QSO lengths >= 30 minutes
		if (this_off - this_on < min30) this_off = this_on + min30;
		if (that_off - that_on < min30) that_off = that_on + min30;
		bool overlap =
			(that_on >= this_on && that_on <= this_off) ||
			(that_off >= this_on && that_off <= this_off) ||
			(this_on >= that_on && this_on <= that_off) ||
			(this_off >= that_on && this_off <= that_off);
		bool is_swl = (qso1[0]["SWL"] == "Y");
		bool other_swl = (qso2[0]["SWL"] == "Y");
		// Both records are SWL and match
		if (is_swl && other_swl && swl_match && overlap) {
			return MT_2XSWL_MATCH;
		}
		// is_swl and time within 30 - SWL_MATCH
		else if (is_swl && !other_swl && swl_match && overlap) {
			return MT_SWL_MATCH;
		}
		// is_swl - no match
		else if (is_swl && !other_swl && !(swl_match && overlap)) {
			return MT_SWL_NOMATCH;
		}
		else if (overlap && nondate_mismatch_count == 0) {
			// Date fields match within 30 minutes and all fields match - PROBABLE
			if (trivial_mismatch_count == 0) {
				return MT_PROBABLE;
			}
			// Date fields match within 30 minutes and important fields match match - POSSIBLE
			else {
				return MT_POSSIBLE;
			}
		}
		// Date fields out by > 30 mins and other fields agree - POSSIBLE
		else if (nondate_mismatch_count == 0) {
			return MT_UNLIKELY;
		}
		// The two records overlap their times on and off 
		else if (overlap && net_mismatch_count == 0 && !call_match) {
			return MT_OVERLAP;
		}
		// Significant difference - NOMATCH
		else {
			return MT_NOMATCH;
		}
	}
}

// ! Returns true if items \p field_name match between \p qso1 and \p qso2.
bool zl_record::items_match(const json& qso1, const json& qso2, std::string field_name) {
	std::string lhs = qso1.at(field_name).get<std::string>();
	std::string rhs = qso2.at(field_name).get<std::string>(); 
	// Convert both fields to upper case
	for (unsigned int i = 0; i < lhs.length(); i++) {
		lhs[i] = toupper(lhs[i]);
	}
	for (unsigned int i = 0; i < rhs.length(); i++) {
		rhs[i] = toupper(rhs[i]);
	}
	if (lhs == rhs ||
		lhs == "" ||
		rhs == "") {
		// Fields are equal or either is blank
		return true;
	}
	// Special cases
	else if (field_name == "GRIDSQUARE" || field_name == "MY_GRIDSQUARE") {
		// Special case for GRIDSQUARE
		// they compare if they are equal for the length
		// of the shorter.
		int iLength = std::min<int>(lhs.length(), rhs.length());
		iLength = std::min<int>(iLength, 4);
		if (lhs.substr(0, iLength) == rhs.substr(0, iLength)) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (field_name == "CALL") {
		// Ignore /M or /P
		if (lhs.length() == rhs.length() - 2 && rhs[lhs.length()] == '/') {
			return true;
		}
		else if (rhs.length() == lhs.length() - 2 && lhs[rhs.length()] == '/') {
			return true;
		}
		else {
			// Remove intermediate dashes in SWL calls
			std::string lhs1;
			for (int ix = 0; ix < lhs.length(); ix++) {
				if (lhs[ix] != '-') lhs1 += lhs[ix];
			}
			std::string rhs1;
			for (int ix = 0; ix < rhs.length(); ix++) {
				if (rhs[ix] != '-') rhs1 += rhs[ix];
			}
			if (lhs1 == rhs1) return true;
			else return false;
		}
	}
	else if (field_name == "TIME_ON" || field_name == "TIME_OFF") {
		// TIME_ON or TIME_OFF - 
		// they compare if they are equal to the minute
		if (lhs.substr(0, 4) == rhs.substr(0, 4)) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (field_name == "MODE" || field_name == "SUBMODE") {
		// Special case for MODE - check against SUBMODE as well (both ways)
		if (lhs == to_upper(qso2.at("SUBMODE").get<std::string>()) ||
			lhs == to_upper(qso2.at("MODE").get<std::string>())) {
			return true;
		}
		// The case where the input record has deprecated MODE against existing record SUBMODE
		else if (field_name == "MODE" &&
			to_upper(qso1.at("SUBMODE").get<std::string>()) == rhs) {
			return true;
		}
		// TODO : Use spec_data_ to check for equivalent modes
		//else if (field_name == "MODE" &&
		//	((spec_data_->dxcc_mode(lhs) == qso2.at("MODE").get<std::string>()) || 
		//	(spec_data_->dxcc_mode(qso2.at("MODE").get<std::string>()) == lhs))) {
		//	// Some records from LotW only have generic modes - both ways
		//	return true;
		//}
		else {
			return false;
		}

	}
	else if (field_name == "FREQ" || field_name == "FREQ_RX") {
		// TODO: Ignore differences in frequency until we have a better way of comparing
		return true;
		// Frequency may be given to fewer decimal places
		// Match if in same band
		//double f1 = stod(qso1.at(field_name).get<std::string>());
		//double f2 = stod(qso2.at(field_name).get<std::string>());
		//if (spec_data_->band_for_freq(f1) == spec_data_->band_for_freq(f2)) {
		//	return true;
		//}
		//else {
		//	return false;
		//}
	}
	return false;
}

// ! Get the date and time as a std::chrono::system_clock::timepoisnt
std::chrono::system_clock::time_point zl_record::ctimestamp(const json& qso, bool time_off) {
	try {
		std::tm qso_time = {};
		// Get start timestamp
		std::string qso_date_field = time_off ? "QSO_DATE_OFF" : "QSO_DATE";
		std::string time_field = time_off ? "TIME_OFF" : "TIME_ON";
		std::string qso_date = qso[0][qso_date_field];
		std::string qso_time_str = qso[0][time_field];
		qso_time.tm_year = std::stoi(qso_date.substr(0, 4)) - 1900;
		qso_time.tm_mon = std::stoi(qso_date.substr(4, 2)) - 1;
		qso_time.tm_mday = std::stoi(qso_date.substr(6, 2));
		qso_time.tm_hour = std::stoi(qso_time_str.substr(0, 2));
		qso_time.tm_min = std::stoi(qso_time_str.substr(2, 2));
		qso_time.tm_sec = 0;
		if (qso_time_str.length() == 6) {
			qso_time.tm_sec = std::stoi(qso_time_str.substr(4, 2));
		}
		// To stop it getting randomly std::set in implementations that do not consistently initialise structures
		qso_time.tm_isdst = false;
		time_t tt = mktime(&qso_time);
		return std::chrono::system_clock::from_time_t(tt);
	}
	catch (std::invalid_argument&) {
		// Return an invalid time
		return std::chrono::system_clock::time_point::min();
	}
}