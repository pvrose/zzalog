#pragma once

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

//! This class contains the match methods between QSO records.

class zl_record {
public:
	//! Constructor
	zl_record();
	//! Destructor
	virtual ~zl_record();
	//! Match result enumeration
	enum match_result_t : uint8_t
	{
		MT_NOMATCH = 0,      //!< No matching record found in log
		MT_EXACT,            //!< An exact match found in log
		MT_PROBABLE,         //!< A close match - same band/date/call but time out by upto 30 minutes
		MT_POSSIBLE,         //!< Call found but something important differs
		MT_UNLIKELY,         //!< A close match - same band/date/call but time out by > 30 mins
		MT_LOC_MISMATCH,     //!< A close match but a location field differs
		MT_SWL_MATCH,        //!< An SWL report that is a close match to existing activity
		MT_SWL_NOMATCH,      //!< An SWL report that is no match for any activity
		MT_2XSWL_MATCH,      //!< An SWL report matches an existing SWL report
		MT_OVERLAP,          //!< The two records have same freq/mode and times overlap
	};
	//! Flags used in match_records
	enum match_flags_t : uint8_t
	{
		MR_NONE,             //!< No special match instructions - default
		MR_ALLOW_LOC = 1,    //!< ALlow limited location mismatch (used for LOTW)
		MR_ALLOW_QSLS = 2,   //!< Allow processing of QSL_SENT* (used for OQRS)
	};

	//! Returns match_result_t between QSO \p record1 and QSO \p record2.
	//! \param qso1 QSO record as JSON object.
	//! \param qso1 QSO record as JSON object.
	//! \param flags match_flags_t to use to control the match.
	static match_result_t match_records(const json& qso1, const json& qso2, match_flags_t flags = MR_NONE);

	//! Returns true if items \p field_name match between \p qso1 and \p qso2.
	static bool items_match(const json& qso1, const json& qso2, std::string field_name);

	//! Get the date and time as a std::chrono::system_clock::timepoisnt
	//! \param qso QSO record as JSON object.
	static std::chrono::system_clock::time_point ctimestamp(const json& qso, bool time_off = false);
};

//! JSON serialization for zl_match::match_result_t
NLOHMANN_JSON_SERIALIZE_ENUM (zl_record::match_result_t, {
	{ zl_record::MT_NOMATCH, "No Match" },
	{ zl_record::MT_EXACT, "Exact Match" },
	{ zl_record::MT_PROBABLE, "Probable Match" },
	{ zl_record::MT_POSSIBLE, "Possible Match" },
	{ zl_record::MT_UNLIKELY, "Unlikely Match" },
	{ zl_record::MT_LOC_MISMATCH, "Location Mismatch" },
	{ zl_record::MT_SWL_MATCH, "SWL Match" },
	{ zl_record::MT_SWL_NOMATCH, "SWL No Match" },
	{ zl_record::MT_2XSWL_MATCH, "2x SWL Match" },
	{ zl_record::MT_OVERLAP, "Overlap" },
	}
);

//! JSON serialization for zl_match::match_flags_t
//! Note: Flags are serialized as an array of strings.
//! E.g. ["MR_ALLOW_LOC", "MR_ALLOW_QSLS"]
NLOHMANN_JSON_SERIALIZE_ENUM(zl_record::match_flags_t, {
	{ zl_record::MR_NONE, "MR_NONE" },
	{ zl_record::MR_ALLOW_LOC, "MR_ALLOW_LOC" },
	{ zl_record::MR_ALLOW_QSLS, "MR_ALLOW_QSLS" },
	}
);

