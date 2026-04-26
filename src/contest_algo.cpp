/*
    Copyright 2026, Philip Rose, GM3ZZA

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
#include "contest_algo.h"

#include "contest_data.h"
#include "qso_data.h"
#include "qso_manager.h"
#include "record.h"
#include "spec_data.h"
#include "stn_data.h"

#include "zc_file_holder.h"
#include "zc_status.h"
#include "zc_utils.h"

#include <algorithm>
#include <fstream>
#include <regex>
#include <map>
#include <set>
#include <variant>
#include <vector>
#include <string>

extern zc_file_holder* file_holder_;
extern zc_status* status_;

// Constructor.
contest_algo::contest_algo(const std::string& algorithm, const std::string& filename) {
	my_info_ = stn_data_->get_qth(qso_manager_->get_default(qso_manager::QTH));
	std::string algo_path = filename.empty() ? contest_data_->get_algorithm_file(algorithm) : filename;
	if (!parse_algorithm_file(algo_path)) {
		status_->misc_status(ST_ERROR, "CONTEST: Failed to parse contest algorithm file %s", algo_path.c_str());
	};
}

// Parse the given exchange per the rx_exchange definition and update
// the QSO record with the parsed fields.
void contest_algo::parse_exchange(record* qso, const std::string& text) {
	std::vector<std::string> tokens;
	zc::split_line(text, tokens, ' ');
	// For each item in the rx exchange definition, take the next token
	// and update the QSO record with the field value.
	int token_index = 0;
	for (const auto& item : algo_data_->rx_exchange.fields) {
		if (token_index >= tokens.size()) {
			// Not enough tokens in the exchange text - ignore the rest of the items.
			break;
		}
		else if (item.is_conditional) {
			// Conditional field - check the condition and if it is met, update the QSO record with the field value.
			if (evaluate_condition(qso, item.cond_field->cond)) {
				qso->item(item.field_name, tokens[token_index]);
			}
		}
		else {
			// Simple field - update the QSO record with the field value.
			qso->item(item.field_name, tokens[token_index]);
		}
		token_index++;
	}
}

// Generate the exchange text from the QSO record per the tx_exchange definition.
std::string contest_algo::generate_exchange(const record* qso) {
	std::string exchange_text;
	for (const auto& item : algo_data_->tx_exchange.fields) {
		if (item.is_conditional) {
			// Conditional field - check the condition and if it is met, add the field value to the exchange text.
			if (evaluate_condition(qso, item.cond_field->cond)) {
				exchange_text += get_my_value(item.field_name) + " ";
			}
		}
		else {
			// Simple field - add the field value to the exchange text.
			exchange_text += get_my_value(item.field_name) + " ";
		}
	}
	return exchange_text;
}

// Score the QSO by evaluating the points definitions and checking for any new multipliers.
contest_algo::score_result contest_algo::score_qso(record* qso, std::set<std::string>& multipliers) {
	score_result result;
	// Evaluate multiplier specifications to check for any new multipliers.
	std::string multiplier_value;
	for (const auto& field_name : algo_data_->multiplier.field_names) {
		if (!multiplier_value.empty()) {
			multiplier_value += "-";
		}
		multiplier_value += qso->item(field_name);
	}
	if (!multiplier_value.empty() && multipliers.find(multiplier_value) == multipliers.end()) {
		multipliers.insert(multiplier_value);
		result.multiplier++;
	}
	// Evaluate points definitions to calculate points scored by the QSO.
	for (const auto& points_def : algo_data_->points) {
		bool conditions_met = true;
		for (const auto& cond : points_def.conditions) {
			if (!evaluate_condition(qso, cond)) {
				conditions_met = false;
				break;
			}
		}
		if (conditions_met) {
			if (points_def.value.is_calculation) {
				// Points value is a calculation - evaluate it to get the points scored.
				result.qso_points += evaluate_distance_calculation(qso, *points_def.value.calc);
			}
			else {
				// Points value is a numeric value - add it to the points scored.
				result.qso_points += points_def.value.numeric_value;
			}
		}
	}
	return result;
}

// Evaluate a condition against the QSO record.
bool contest_algo::evaluate_condition(const record* qso, const condition& cond) {
	std::string field_value = qso->item(cond.field_name);
	switch (cond.type) {
	case condition_type::EQUALS:
		return field_value == std::get<std::string>(cond.value);
	case condition_type::IN_LIST: {
		const auto& value_list = std::get<std::vector<std::string>>(cond.value);
		return std::find(value_list.begin(), value_list.end(), field_value) != value_list.end();
	}
	case condition_type::MATCHES: {
		const std::string& regex_pattern = std::get<std::string>(cond.value);
		std::regex pattern(regex_pattern);
		return std::regex_match(field_value, pattern);
	}
	case condition_type::SAME:
		return field_value == get_my_value(cond.field_name);
	case condition_type::OTHER:
		return field_value != get_my_value(cond.field_name);
	default:
		return false;
	}
}

const std::map<std::string, std::string> RX_ALIASES = {
	{"CALLSIGN", "CALL"},
	{"SERIAL", "SRX" },
	{"RST", "RST_RCVD" }
};

const std::map<std::string, std::string> TX_ALIASES = {
	{"CALLSIGN", "CALL"},
	{"SERIAL", "STX" },
	{"RST", "RST_SENT" }
};

// Convert aliased field names to ADIF field names.
std::string contest_algo::resolve_field_name(const std::string& name, bool is_rx) {
	if (is_rx) {
		auto it = RX_ALIASES.find(name);
		if (it != RX_ALIASES.end()) {
			return it->second;
		}
	}
	else {
		auto it = TX_ALIASES.find(name);
		if (it != TX_ALIASES.end()) {
			return it->second;
		}
	}
	return name;  // Return the original name if no alias found
}

// Get the 'my' value for a field (e.g., for SAME and OTHER conditions).
// NB: This uses the field names that relate to the contacted station in the QSO record,
// so we can use qso->item(name) == get_my_value(name) for SAME conditions et sim. for OTHER.
std::string contest_algo::get_my_value(const std::string& field_name) {
	// Look up the field name in the ADIF to QTH value map to find the
	// corresponding QTH value, and then return the value from my_info_ if it exists.
	if (ADIF_2_QTH_VALUE_T.find(field_name) != ADIF_2_QTH_VALUE_T.end()) {
		qth_value_t qth_value = ADIF_2_QTH_VALUE_T.at(field_name);
		if (my_info_->data.find(qth_value) != my_info_->data.end()) {
			return my_info_->data.at(qth_value);
		}
		else {
			return "";  // Return empty string if QTH value not found
		}
	}
	if (field_name == "RST_SENT") {
		return default_report();
	}
	if (field_name == "CALL") {
		return qso_manager_->get_default(qso_manager::stn_item_t::CALLSIGN);
	}
	if (field_name == "QTH") {
		return qso_manager_->get_default(qso_manager::stn_item_t::QTH);
	}
	if (field_name == "OP") {
		return qso_manager_->get_default(qso_manager::stn_item_t::OP);
	}
	return "";  // Return empty string if no match found
}

// Evaluate a distance calculation to get the points scored by the QSO.
// This will update qso->item("DISTANCE") with the calculated distance.
int contest_algo::evaluate_distance_calculation(record* qso, const distance_calculation& calc) {
	int sz_grid;
	if (calc.field_name == "GRID4") {
		sz_grid = 4;
	}
	else if (calc.field_name == "GRID6") {
		sz_grid = 6;
	}
	else {
		return 0;  // Unsupported field for distance calculation
	}
	std::string their_grid = qso->item("GRIDSQUARE");
	std::string my_grid = get_my_value("GRIDSQUARE");
	if (their_grid.length() < sz_grid || my_grid.length() < sz_grid) {
		status_->misc_status(ST_WARNING, "CONTEST: Invalid grid squares for distance calculation: '%s' and '%s'", my_grid.c_str(), their_grid.c_str());
		return 0;  // Invalid grid squares for distance calculation
	}
	their_grid = their_grid.substr(0, sz_grid);
	my_grid = my_grid.substr(0, sz_grid);
	zc::lat_long_t their_coords = zc::grid_to_latlong(their_grid);
	zc::lat_long_t my_coords = zc::grid_to_latlong(my_grid);
	double distance;
	double bearing;
	zc::great_circle(my_coords, their_coords, bearing, distance);
	int i_distance = static_cast<int>(std::round(distance));
	qso->item("DISTANCE", std::to_string(i_distance));
	if (calc.op == "*") {
		return static_cast<int>(distance * std::stod(calc.value));
	}
	else if (calc.op == "/") {
		return static_cast<int>(std::ceil(distance / std::stod(calc.value)));
	}
	else if (calc.op == "+") {
		return i_distance + std::stoi(calc.value);
	}
	else if (calc.op == "-") {
		return i_distance - std::stoi(calc.value);
	}
	else {
		return 0;  // Unsupported operator for distance calculation
	}
}

// Parse the contest algorithm file and populate the data structures.
bool contest_algo::parse_algorithm_file(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		status_->misc_status(ST_ERROR, "CONTEST: Cannot open file: %s", filename.c_str());
		return false;
	}

	// Allocate the algorithm data structure
	algo_data_ = new contest_algorithm_file();

	std::string line;
	int line_number = 0;
	bool has_exchange = false;
	bool has_multiplier = false;
	bool has_total = false;

	while (std::getline(file, line)) {
		line_number++;

		// Trim whitespace
		size_t start = line.find_first_not_of(" \t\r\n");
		size_t end = line.find_last_not_of(" \t\r\n");
		if (start == std::string::npos) {
			continue; // Empty line
		}
		line = line.substr(start, end - start + 1);

		// Skip comments and empty lines
		if (line.empty() || line[0] == '#' || line.find("//") == 0) {
			continue;
		}

		// Parse based on line type
		if (line.find("exchange:") == 0) {
			// Parse once for RX, once for TX with different field name resolution
			if (!parse_exchange_line(line, algo_data_->rx_exchange, true)) {
				status_->misc_status(ST_ERROR, "CONTEST: Failed to parse exchange (RX) at line %d in %s", line_number, filename.c_str());
				delete algo_data_;
				algo_data_ = nullptr;
				return false;
			}
			if (!parse_exchange_line(line, algo_data_->tx_exchange, false)) {
				status_->misc_status(ST_ERROR, "CONTEST: Failed to parse exchange (TX) at line %d in %s", line_number, filename.c_str());
				delete algo_data_;
				algo_data_ = nullptr;
				return false;
			}
			has_exchange = true;
		}
		else if (line.find("multiplier:") == 0) {
			if (!parse_multiplier_line(line, algo_data_->multiplier)) {
				status_->misc_status(ST_ERROR, "CONTEST: Failed to parse multiplier at line %d in %s", line_number, filename.c_str());
				delete algo_data_;
				algo_data_ = nullptr;
				return false;
			}
			has_multiplier = true;
		}
		else if (line.find("points:") == 0) {
			points_definition points_def;
			if (!parse_points_line(line, points_def)) {
				status_->misc_status(ST_ERROR, "CONTEST: Failed to parse points at line %d in %s", line_number, filename.c_str());
				delete algo_data_;
				algo_data_ = nullptr;
				return false;
			}
			algo_data_->points.push_back(std::move(points_def));
		}
		else if (line.find("total:") == 0) {
			if (!parse_total_line(line, algo_data_->total)) {
				status_->misc_status(ST_ERROR, "CONTEST: Failed to parse total at line %d in %s", line_number, filename.c_str());
				delete algo_data_;
				algo_data_ = nullptr;
				return false;
			}
			has_total = true;
		}
		else {
			status_->misc_status(ST_WARNING, "CONTEST: Unknown line at %d in %s: %s", line_number, filename.c_str(), line.c_str());
		}
	}

	file.close();

	// Validate required sections
	if (!has_exchange) {
		status_->misc_status(ST_ERROR, "CONTEST: Missing exchange definition in %s", filename.c_str());
		delete algo_data_;
		algo_data_ = nullptr;
		return false;
	}
	if (!has_multiplier) {
		status_->misc_status(ST_ERROR, "CONTEST: Missing multiplier definition in %s", filename.c_str());
		delete algo_data_;
		algo_data_ = nullptr;
		return false;
	}
	if (algo_data_->points.empty()) {
		status_->misc_status(ST_ERROR, "CONTEST: No points definitions in %s", filename.c_str());
		delete algo_data_;
		algo_data_ = nullptr;
		return false;
	}
	if (!has_total) {
		status_->misc_status(ST_ERROR, "CONTEST: Missing total definition in %s", filename.c_str());
		delete algo_data_;
		algo_data_ = nullptr;
		return false;
	}

	return true;
}

// Helper: Parse exchange line
bool contest_algo::parse_exchange_line(const std::string& line, exchange_definition& exchange, bool is_rx) {
	size_t colon_pos = line.find(':');
	if (colon_pos == std::string::npos) return false;

	std::string fields_str = line.substr(colon_pos + 1);
	std::vector<std::string> tokens;
	zc::split_line(fields_str, tokens, ' ');

	for (const auto& token : tokens) {
		if (token.empty()) continue;

		field_definition field_def;

		// Check for conditional field: "if <condition> : <field_name>"
		if (token.find("if ") == 0) {
			field_def.is_conditional = true;
			field_def.cond_field = std::make_unique<conditional_field>();

			size_t cond_colon = token.find(" : ");
			if (cond_colon == std::string::npos) {
				status_->misc_status(ST_ERROR, "CONTEST: Invalid conditional field syntax: %s", token.c_str());
				return false;
			}

			std::string cond_str = token.substr(3, cond_colon - 3);
			std::string field_name = token.substr(cond_colon + 3);

			if (!parse_condition(cond_str, field_def.cond_field->cond)) {
				return false;
			}

			field_def.cond_field->field_name = resolve_field_name(field_name, is_rx);
			if (is_rx) {
				rx_items_.insert(field_def.cond_field->field_name);
			} else {
				tx_items_.insert(field_def.cond_field->field_name);
			}
		}
		else {
			// Simple field
			field_def.is_conditional = false;
			field_def.field_name = resolve_field_name(token, is_rx);

			if (is_rx) {
				rx_items_.insert(field_def.field_name);
			} else {
				tx_items_.insert(field_def.field_name);
			}

			// Check for serial number usage
			if (token == "SERIAL" || token == "SERIAL+") {
				uses_serno_ = true;
			}
		}

		exchange.fields.push_back(std::move(field_def));
	}

	return !exchange.fields.empty();
}

// Helper: Parse multiplier line
bool contest_algo::parse_multiplier_line(const std::string& line, multiplier_definition& multiplier) {
	size_t colon_pos = line.find(':');
	if (colon_pos == std::string::npos) return false;

	std::string fields_str = line.substr(colon_pos + 1);
	std::vector<std::string> tokens;
	zc::split_line(fields_str, tokens, ' ');

	for (const auto& token : tokens) {
		if (token.empty()) continue;
		std::string field_name = resolve_field_name(token, true);
		multiplier.field_names.push_back(field_name);
		scoring_items_.insert(field_name);
	}

	return !multiplier.field_names.empty();
}

// Helper: Parse condition string
bool contest_algo::parse_condition(const std::string& cond_str, condition& cond) {
	// Check for "same <field_name>"
	if (cond_str.find("same ") == 0) {
		cond.type = condition_type::SAME;
		cond.field_name = resolve_field_name(cond_str.substr(5), true);
		scoring_items_.insert(cond.field_name);
		return true;
	}

	// Check for "other <field_name>"
	if (cond_str.find("other ") == 0) {
		cond.type = condition_type::OTHER;
		cond.field_name = resolve_field_name(cond_str.substr(6), true);
		scoring_items_.insert(cond.field_name);
		return true;
	}

	// Check for "field = value"
	size_t equals_pos = cond_str.find(" = ");
	if (equals_pos != std::string::npos) {
		cond.type = condition_type::EQUALS;
		cond.field_name = resolve_field_name(cond_str.substr(0, equals_pos), true);
		cond.value = cond_str.substr(equals_pos + 3);
		scoring_items_.insert(cond.field_name);
		return true;
	}

	// Check for "field in_list value1, value2, ..."
	size_t in_list_pos = cond_str.find(" in_list ");
	if (in_list_pos != std::string::npos) {
		cond.type = condition_type::IN_LIST;
		cond.field_name = resolve_field_name(cond_str.substr(0, in_list_pos), true);

		std::string values_str = cond_str.substr(in_list_pos + 9);
		std::vector<std::string> value_list;
		zc::split_line(values_str, value_list, ',');

		// Trim whitespace from each value
		for (auto& val : value_list) {
			size_t start = val.find_first_not_of(" \t");
			size_t end = val.find_last_not_of(" \t");
			if (start != std::string::npos) {
				val = val.substr(start, end - start + 1);
			}
		}

		cond.value = value_list;
		scoring_items_.insert(cond.field_name);
		return true;
	}

	// Check for "field matches regex"
	size_t matches_pos = cond_str.find(" matches ");
	if (matches_pos != std::string::npos) {
		cond.type = condition_type::MATCHES;
		cond.field_name = resolve_field_name(cond_str.substr(0, matches_pos), true);
		cond.value = cond_str.substr(matches_pos + 9);
		scoring_items_.insert(cond.field_name);
		return true;
	}

	status_->misc_status(ST_ERROR, "CONTEST: Unknown condition syntax: %s", cond_str.c_str());
	return false;
}

// Helper: Parse points line
bool contest_algo::parse_points_line(const std::string& line, points_definition& points_def) {
	size_t colon_pos = line.find(':');
	if (colon_pos == std::string::npos) return false;

	std::string rest = line.substr(colon_pos + 1);

	// Trim leading whitespace
	size_t start = rest.find_first_not_of(" \t");
	if (start != std::string::npos) {
		rest = rest.substr(start);
	}

	// Check if there's a condition part (before the last colon)
	size_t last_colon = rest.rfind(':');
	if (last_colon != std::string::npos) {
		std::string conditions_str = rest.substr(0, last_colon);
		std::string value_str = rest.substr(last_colon + 1);

		// Trim value string
		start = value_str.find_first_not_of(" \t");
		if (start != std::string::npos) {
			value_str = value_str.substr(start);
		}

		// Parse conditions (may have & separator)
		std::vector<std::string> cond_tokens;
		zc::split_line(conditions_str, cond_tokens, '&');

		for (auto& cond_token : cond_tokens) {
			// Trim whitespace
			start = cond_token.find_first_not_of(" \t");
			size_t end = cond_token.find_last_not_of(" \t");
			if (start != std::string::npos) {
				cond_token = cond_token.substr(start, end - start + 1);
			}

			if (!cond_token.empty()) {
				condition cond;
				if (!parse_condition(cond_token, cond)) {
					return false;
				}
				points_def.conditions.push_back(cond);
			}
		}

		// Parse value
		if (!parse_points_value(value_str, points_def.value)) {
			return false;
		}
	}
	else {
		// No condition, just value
		if (!parse_points_value(rest, points_def.value)) {
			return false;
		}
	}

	return true;
}

// Helper: Parse points value
bool contest_algo::parse_points_value(const std::string& value_str, points_value& value) {
	// Check for distance calculation
	if (value_str.find("distance ") == 0) {
		value.is_calculation = true;
		value.calc = std::make_unique<distance_calculation>();

		std::vector<std::string> tokens;
		zc::split_line(value_str, tokens, ' ');

		if (tokens.size() >= 4) {
			value.calc->field_name = tokens[1];
			value.calc->op = tokens[2];
			value.calc->value = tokens[3];
			return true;
		}
		else {
			status_->misc_status(ST_ERROR, "CONTEST: Invalid distance calculation syntax: %s", value_str.c_str());
			return false;
		}
	}
	else {
		// Numeric value
		try {
			value.is_calculation = false;
			value.numeric_value = std::stoi(value_str);
			return true;
		}
		catch (...) {
			status_->misc_status(ST_ERROR, "CONTEST: Invalid points value: %s", value_str.c_str());
			return false;
		}
	}
}

// Helper: Parse total line
bool contest_algo::parse_total_line(const std::string& line, total_definition& total) {
	size_t colon_pos = line.find(':');
	if (colon_pos == std::string::npos) return false;

	std::string calc_str = line.substr(colon_pos + 1);

	// Trim whitespace
	size_t start = calc_str.find_first_not_of(" \t");
	if (start != std::string::npos) {
		calc_str = calc_str.substr(start);
	}

	// Parse "multiplier * points" or similar
	std::vector<std::string> tokens;
	zc::split_line(calc_str, tokens, ' ');

	if (tokens.size() >= 3) {
		total.calculation.left_operand = tokens[0];
		total.calculation.op = tokens[1];
		total.calculation.right_operand = tokens[2];
		return true;
	}
	else {
		status_->misc_status(ST_ERROR, "CONTEST: Invalid total calculation syntax: %s", calc_str.c_str());
		return false;
	}
}

// Get default report.`
std::string contest_algo::default_report() const {
	// Find the actual mode from the QSO record.
	// Get the current QSO record from the QSO manager.
	record* current_qso = qso_manager_->data()->current_qso();
	if (current_qso == nullptr) {
		return "59";  // Default report if no current QSO
	}
	std::string mode = current_qso->item("MODE");
	// Convert it to CW or PHONE etc. as needed for the contest algorithm.
	std::string dxcc_mode = spec_data_->dxcc_mode(mode);
	if (dxcc_mode == "CW") {
		return "599";
	}
	else if (dxcc_mode == "DATA") {
		return "599";
	}
	else {
		return "59";  // Default report for other modes (AM, FM, SSB, SSTV, ATV, etc.)
	}
}