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
#pragma once

//! \file contest_algo.h
//! 
//! This file provides the class definition, file format and data structure to
//! support contest scoring and logging algorithms.
//! 

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <variant>

class record;
struct qth_info_t;
class contest_scorer;

//! The class will read in the contest algorithm definitions from a file and create
//! the data structures to support the algorithm. It will then use the data structures
//! to calculate the score for a QSO based on the contest algorithm and the QSO data.
//! \code
//! FILE STRUCTURE (BNF):
//! <contest_algorithm_file> ::= <exchange_definition> <scoring_definitions>
//! <exchange_definition> ::= "exchange:" <field_definition>+ <EOL>
//! <field_definition> ::= <field_name> | <conditional_field>
//! <conditional_field> ::= "if" <condition> ":" <field_name>
//! <condition> ::= (<field_name> <condition_specifier>) | ("same" <field_name>) | ("other" <field_name>)
//! <condition_specifier> ::= "=" <value> | "in_list" <value_list> | "matches" <regex>
//! <value_list> ::= <value> ("," <value>)*
//! <scoring_definitions> ::= <multiplier_definition> <points_definition>+ <total_definition>
//! <multiplier_definition> ::= "multiplier:" <multiplier_specification>+ <EOL>
//! <multiplier_specification> ::= <field_name>+
//! <points_definition> ::= "points:" [<condition> ("&" <condition>)* ":"] <points_value> <EOL>
//! <points_value> ::= <points_calculation> | <value>
//! <points_calculation> ::= <distance_calculation> ; Others to be added as needed
//! <total_definition> ::= "total:" <points_calculation> <EOL>
//! <points_calculation> ::= <product_calculation>
//! <product_calculation> ::= "multiplier" "*" "points"
//! <field_name> ::= <ADIF field name> | <alias>
//! <alias> ::= "RST" | "SERIAL+" | "GRID4"
//! <ADIF field name> ::= [A-Z0-9_]+ ; ADIF field names per ADIF standard
//! <value> ::= [A-Z0-9]+ ; Values are case-sensitive and per ADIF standard 
//! <regex> ::= <regular expression> ; Regular expressions per standard regex syntax
//! <distance_calculation> ::= "distance" <field_name> (operator <value>); Distance calculation based on field values (eg GRID4) and QSO location
//! <EOL> ::= "\n" ; End of line character
//! \endcode
//! 
//! EXAMPLE 1: Basic contest with a single exchange field and simple scoring
//! \code
//! exchange: RST SERIAL+
//! multiplier: DXCC BAND
//! points: other DXCC: 1
//! total: multiplier * points
//! \endcode
//! 
//! EXAMPLE 2: Contest with more complex scoring
//! \code
//! exchange: RST ITUZ
//! multiplier: ITUZ BAND
//! points: same ITUZ: 1
//! points: ITUZ in_list A, B, C: 1
//! points: other ITUZ & same CONT: 3
//! points: other ITUZ & other CONT: 5
//! total: multiplier * points
//! \endcode
class contest_algo {

	//! Condition specifier types
	enum class condition_type {
		EQUALS,           //! = <value>
		IN_LIST,         //! in_list <value_list>
		MATCHES,         //! matches <regex>
		SAME,            //! same <field_name>
		OTHER            //! other <field_name>
	};

	//! Forward declarations
	struct condition;
	struct field_definition;

	//! Represents a single condition in the BNF
	struct condition {
		condition_type type;
		std::string field_name;
		std::variant<std::string, std::vector<std::string>> value;  //! Single value or list of values

		condition() : type(condition_type::EQUALS) {}
	};

	//! Represents a conditional field: if <condition> : <field_name>
	struct conditional_field {
		condition cond;
		std::string field_name;

		conditional_field() = default;
	};

	//! Represents a field definition (can be simple field name or conditional)
	struct field_definition {
		bool is_conditional;
		std::string field_name;                      //! Used when is_conditional = false
		std::unique_ptr<conditional_field> cond_field;  //! Used when is_conditional = true

		field_definition() : is_conditional(false) {}
		field_definition(const std::string& name) : is_conditional(false), field_name(name) {}

		field_definition(const field_definition&) = delete;
		field_definition& operator=(const field_definition&) = delete;
		field_definition(field_definition&&) = default;
		field_definition& operator=(field_definition&&) = default;
	};

	//! Represents the exchange definition section
	struct exchange_definition {
		std::vector<field_definition> fields;

		exchange_definition() = default;
	};

	//! Represents a multiplier specification
	struct multiplier_definition {
		std::vector<std::string> field_names;

		multiplier_definition() = default;
	};

	//! Distance calculation for points
	struct distance_calculation {
		std::string field_name;
		std::string op;      //! operator (e.g., *, /, +, -)
		std::string value;

		distance_calculation() = default;
	};

	//! Points value can be a number or a calculation
	struct points_value {
		bool is_calculation;
		int numeric_value;                              //! Used when is_calculation = false
		std::unique_ptr<distance_calculation> calc;     //! Used when is_calculation = true

		points_value() : is_calculation(false), numeric_value(0) {}
		explicit points_value(int val) : is_calculation(false), numeric_value(val) {}

		points_value(const points_value&) = delete;
		points_value& operator=(const points_value&) = delete;
		points_value(points_value&&) = default;
		points_value& operator=(points_value&&) = default;
	};

	//! Represents a points definition line
	struct points_definition {
		std::vector<condition> conditions;  //! Can have multiple conditions with & operator
		points_value value;

		points_definition() = default;

		points_definition(const points_definition&) = delete;
		points_definition& operator=(const points_definition&) = delete;
		points_definition(points_definition&&) = default;
		points_definition& operator=(points_definition&&) = default;
	};

	//! Product calculation: "multiplier" "*" "points"
	struct product_calculation {
		std::string left_operand;   //! e.g., "multiplier"
		std::string op;             //! e.g., "*"
		std::string right_operand;  //! e.g., "points"

		product_calculation() : op("*") {}
	};

	//! Total definition
	struct total_definition {
		product_calculation calculation;

		total_definition() = default;
	};

	//! Complete contest algorithm file structure
	struct contest_algorithm_file {
		exchange_definition rx_exchange;
		exchange_definition tx_exchange;
		multiplier_definition multiplier;
		std::vector<points_definition> points;
		total_definition total;

		contest_algorithm_file() = default;
	};

public:

	//! Basic contest scoring element
	struct score_result {
		//! Points scored from the QSO
		int qso_points = 0;
		//! Additional multiplier caused by QSO
		int multiplier = 0;
	};


	//! Constructor. Reads the contest algorithm file and populates the data structures.
	//! \param algorithm The name of the contest algorithm to load (eg "CQWW").
	//! \param filename Optional filename to load the algorithm from - if
	//! not specified, the algorithm definition will be loaded from the location
	//! specified in the contest data database for the given algorithm name.
	contest_algo(const std::string& algorithm, const std::string& filename = "");

	//! Algorithm specific method to split text into a number of fields.

	//! This method takes the received report and updates the 
	//! supplied record with information.
	//! \param qso QSO record to update.
	//! \param text The received contest exchange.
	void parse_exchange(record* qso, const std::string& text);
	//! Algorithm specific method to generate text from a number of fields.

	//! This method generates the report to be sent from information in the QSO record
	//! \param qso QSO record to provide exchange.
	//! \result The exchange to be sent.
	std::string generate_exchange(const record* qso);
	//! Algorithm specific method to score an individual QSO.

	//! This method calculates how many points the QSO scores and
	//! whether it provides a multiplier. 
	//! \param qso QSO record to score
	//! \param multipliers Set of multipliers currently worked: the method should
	//! update this if this is a new multiplier.
	//! \result The points scored by the QSO and the number of multipliers added.
	score_result score_qso(record* qso, std::set<std::string>& multipliers);
	//! Algorithm uses serial number.

	//! \result Return true if the contest requires a serial number. Return false if it does not.
	bool uses_serno() const {
		return uses_serno_;
	};

	//! Return all fields used in algorithm.

	//! \result This method should return a vector<string> comprising all the
	//! fields referenced by the algorithm.
	std::set<std::string> fields() {
		std::set<std::string> all_fields;
		all_fields.insert(rx_items_.begin(), rx_items_.end());
		all_fields.insert(tx_items_.begin(), tx_items_.end());
		all_fields.insert(scoring_items_.begin(), scoring_items_.end());
		return all_fields;
	}

	//! Algorithm is valid
	bool valid() const {
		return algo_data_ != nullptr;
	}


private:

	//! Method to parse contest algorithm file and populate data structures
	bool parse_algorithm_file(const std::string& filename);

	//! Helper: Parse exchange line
	bool parse_exchange_line(const std::string& line, exchange_definition& exchange, bool is_rx);

	//! Helper: Parse multiplier line
	bool parse_multiplier_line(const std::string& line, multiplier_definition& multiplier);

	//! Helper: Parse condition string
	bool parse_condition(const std::string& cond_str, condition& cond);

	//! Helper: Parse points line
	bool parse_points_line(const std::string& line, points_definition& points_def);

	//! Helper: Parse points value
	bool parse_points_value(const std::string& value_str, points_value& value);

	//! Helper: Parse total line
	bool parse_total_line(const std::string& line, total_definition& total);

	//! Method to convert aliased field names to ADIF field names
	std::string resolve_field_name(const std::string& name, bool is_rx);

	//! method to get the 'my' value for a field.
	std::string get_my_value(const std::string& field_name);

	//! Method to evaluate a condition against a QSO record
	bool evaluate_condition(const record* qso, const condition& cond);

	//! Method to evaluate a distance calculation (updates \p qso)
	int evaluate_distance_calculation(record* qso, const distance_calculation& value);

	//! Return the default report for the contest algorithm (eg "599")
	std::string default_report() const;

	//! main data structure representing the contest algorithm
	contest_algorithm_file* algo_data_ = nullptr;

	//! Flag indicating that incrementing serial numbers are used
	bool uses_serno_ = false;

	//! Used field names in the RX exchange (after resolving aliases)
	std::set<std::string> rx_items_;
	//! Used field names in the TX exchange (after resolving aliases)
	std::set<std::string> tx_items_;
	//! Used field names in the scoring definitions (after resolving aliases)
	std::set<std::string> scoring_items_;

	//! Pointer to 'my' data.
	const qth_info_t* my_info_ = nullptr;

};
