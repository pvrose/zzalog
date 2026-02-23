#pragma once

#include "cty_element.h"

#include "zc_utils.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <list>
#include <map>
#include<ostream>
#include <string>



class record;

//! \brief This class provides a wrapper for all the callsign exception data.
//! 
//! The data is accumulated from 5 sources.
//! 
//! \par ADIF specification
//! This adds the cty_entity records with just DXCC Entity numbers and names.
//! 
//! \par Clublog.org - cty1_reader.
//! This provides more details for the cty_entity records including:
//! - Nickname (usual prefix)
//! - Geographic data - CQ zones and approximate latitude and logngitude.
//! - Timeliness - timeframe the entity is valid on the DXCC list.
//! 
//! It adds cty_prefix records which allow the parsing of callsigns. This maps
//! the initial characters of a callsign to the entity.
//! 
//! It adds cty_exception records which record any callsigns that are 
//! outwith the decodes using cty_prefix records.
//! 
//! \par country-files.com - cty2_reader
//! This provides additional data in addition to the clublog data.
//! 
//! It updates the cty_entity records with ITU Zones.
//!
//! It updates cty_prefix and cty_exception records with any data that
//! may be more recent than that above.
//! 
//! \par Dxatlas.com - cty3_reader
//! This adds cty_filter entries to the cty_entity records breaking down the
//! DXCC entity into geographic sub-divisions and into usages
//! such as license classes and special event operation.
//! 
//! \par ISO country list - cty4_reader
//! This adds geopolitical countries to the cty_entity records for
//! use in searches.
//! 
class cty_data
{

public:

	//! Source of the data - set by type()
	enum cty_type_t : uint8_t {
		ADIF = 0,          //!< Data from ADIF Specification.
		CLUBLOG,           //!< Data from Clublog.org.
		COUNTRY_FILES,     //!< Data from country-files.com.
		DXATLAS,           //!< Data from dxatlas.com.
		ISO_CODES          //!< Data from iso.csv.
	};


	//! Database structure.
	struct all_data {
		//! All the entities - indexed by dxcc_id.
		std::map < int, cty_entity* > entities;
		//! All the entity level prefixes - indexed by starting string.
		std::map < std::string, std::list<cty_prefix*> > prefixes;
		//! All the exceptions - indexed by callsign.
		std::map < std::string, std::list<cty_exception*> > exceptions;
	};

protected:

	//! Data currently being loaded.
	cty_type_t type_ = ADIF;

public:

	//! Parse source
	enum parse_source_t {
		INVALID,            //!< Callsign marked invalid.
		NO_DECODE,          //!< Not decoded.
		EXCEPTION,          //!< Callsign in entity that is not its normal decode.
		ZONE_EXCEPTION,     //!< Callsign in a zone other than the default for the entity.
		PREVIOUS,           //!< Callsign previously decoded.
		DEFAULT             //!< Parsing based on default for the callsign.
	};

	//! Constructor.
	cty_data(bool reload = false);
	//! Destructor.
	virtual ~cty_data();

	// Return various fields of entity
	std::string nickname(const record* qso);  //!< Returns the nickname for the entity in the \p QSO.
	std::string name(const record* qso);      //!< Returns the name of the entity in the \p QSO.
	std::string continent(const record* qso); //!< Returns the continent of the entity in the \p QSO.
	int cq_zone(const record* qso);      //!< Returns the CQ Zone of the callsign in the \p QSO.
	int itu_zone(const record* qso);     //!< Returns the ITU Zone of the callsign in the \p QSO.
	// Get location
	zc::lat_long_t location(const record* qso); //!< Returns the longitude and latitude of the station in the \p QSO.
	zc::lat_long_t location(int dxcc_id); //!< Returns the longitude and latitude of  the DXCC.
	// Get ISO country code for the callsign
	std::string iso_cc(const std::string& callsign);  //!< Returns country code for \p callsign
	//! Get DXCC ID for \p callsign. If \p allow_exception: look up exceptions.
	int dxcc_id(const std::string& callsign, bool allow_exception);

	
	//! Update record based on parsing
	
	//! \param qso QSO record to update.
	//! \param my_call update the "MY_...." fields rather than the other station's information. 
	//! \return true if successful, false if not.
	bool update_qso(record* qso, bool my_call = false);
	//! Get location details
	
	//! \param qso QSO record to parse.
	//! \return text information about parsing for displaying in a tooltip.
	std::string get_tip(const record* qso);
	//! Parsing source
	
	//! \param qso QSO to parse.
	//! \return indicates how the callsign was parsed. 
	parse_source_t get_source(const record* qso);
	//! Returns the DXCC identifier of the entity worked in \p qso. 
	int entity(const record* qso);
	//! Returns geography information relating to the callsign worked in \p qso.
	std::string geography(const record* qso);
	//! Returns usage information relating to the callsign worked in \p qso.
	std::string usage(const record* qso);
	//! Retunrs geography sub-division nickname
	std::string geo_nick(const record* qso);

	//! Returns the DXCC identifier for the entity with \p nickname.
	int entity(const std::string& nickname);
	//! Returns the entity nickname for the entity with DXCC identifier \p adif_id.
	std::string nickname(int adif_id);

	//! Add the entity \p entry to the database.
	void add_entity(cty_entity* entry);
	//! Add the prefix \p entry mapped by \p pattern to the database.
	void add_prefix(const std::string& pattern, cty_prefix* entry);
	//! Add the exception \p entry mapped by \p pattern to the database.
	void add_exception(const std::string& pattern, cty_exception* entry);
	//! Add the filter \p entry to the specified \p element in the database. 
	void add_filter(cty_element* element, cty_filter* entry);

	//! Returns the recorded timestamp for the data source by \p type.
	std::chrono::system_clock::time_point timestamp(cty_type_t type);
	//! Download the latest data from data source by \p type.
	
	//! Returns true if successful, false if not.
	bool fetch_data(cty_type_t type);
	//! Returns the version of the data source by \p type.
	std::string version(cty_type_t type);

	//! Returns the set of all data.
	all_data* data() { return data_; };

protected:

	//! Load the data from the \p filename specified. 
	bool load_data(std::string* filename = nullptr);
	//! Delete data
	void delete_data(all_data* data);
	//! Merge imported data from latest source.
	void merge_data();
	//! Prepopulate from ADIF Specification.
	void load_adif_data();
	//! Find the entity, pattern and sub-patterns for the supplied QSO: updates internal attributes.
	void parse(record* qso);
	void parse(const record* qso) {
		parse((record*)qso);
	}
	//! Use the attached \p suffix to "mutate" the \p call to parse eg W1ABC/2 type calls.
	void mutate_call(std::string& call, char suffix);
	//! Store json
	void store_json();
	//! Load JSON
	bool load_json();
	//! Load source data
	void load_sources();
	//! Find element that matches the call.
	
	//! \param call Callsign to match.
	//! \param when Date of QSO.
	//! \param matched_call Returns the part of the callsign that matches the element.
	//! \param allow_exception If true look in exception data, otherwise use default parsing.
	//! \return The matching element: either an exception record or an entity.
	cty_element* match_pattern(const std::string& call, const std::string& when, std::string& matched_call, bool allow_exception);
	//! Find specific prefix element that matches call.
	
	//! \param call Callsign to match.
	//! \param when Date of QSO.
	//! \return The matching prefix record.
	cty_element* match_prefix(const std::string& call, const std::string& when);
	//! Find specific secondary filter that matches the call and type.
	
	//! \param element The starting point of the match search - usually an entity element or
	//! a previous filter for multi-layered filters.
	//! \param type Either FT_GEOGRAPHY or FT_USAGE.
	//! \param call The callsign to match.
	//! \param when The date of the QSO.
	cty_filter* match_filter(const cty_element* element, cty_filter::filter_t type, const std::string& call, const std::string& when);

	//! Split \p call into call \p body and \p alt (alternate).
	void split_call(const std::string& call, std::string& alt, std::string& body);

	//! Returns Exception record for current parse result, nullptr if not an exception
	cty_exception* exception() const;
	//! Returns Prefix record for current parse result, nullptr of no prefix.
	cty_prefix* prefix() const;
	
	//! Get the system timestamp for the named \p filename.
	
	//! \param filename Filename.
	//! \return the system timestamp of the file.
	std::chrono::system_clock::time_point get_timestamp(std::string filename);

	//! Check the time stamp
	
	//! \param type The source of the data.
	//! \param days Age in days the filename is considered valid. A warning is raised if the file is older.
	void check_timestamp(cty_type_t type, int days);

	//! \brief Get the URL of the latest big-cty from www.country-files.com.
	//! \param url Returns the URL of the ZIP file containing latest data
	//! \return 0 if successful, -1 if version has not changed, +1 if unsuccessful
	int get_cfile_url(std::string& url);

	//! \brief Download the big-cty zip-file
	//! \param url of the zip-file.
	//! \param local_filename on this system,
	//! \return true if successful
	bool download_cfile_zip(const std::string& url, std::string& local_filename);

	//! \brief Unzip the big-cty cty.csv file.
	//! \return true if successful
	bool unzip_cfile(const std::string& zip_file);
	
	//! The result of a parse request.
	struct {
		//! The entity definition
		cty_entity* entity = nullptr;
		//! Either an exception or prefix
		cty_element* decode_element = nullptr;
		//! Selecetd geographic filter
		cty_geography* geography = nullptr;
		//! Usage filter
		cty_filter* usage = nullptr;
	} parse_result_;

	//! Previous callsign that was parsed, to avoid unnecessary re-parsing.
	std::string current_call_ = "";
	//! Previous QSO that was parsed.
	record* current_qso_ = nullptr;

	//! The country database.
	all_data* data_ = nullptr;

	//! The data being imported
	all_data* import_ = nullptr;

	//! Warnings have been reported during data merge.
	bool report_warnings_ = false;
	//! Errors have been reported during data merge.
	bool report_errors_ = false;

	//! Mapping of data timestamps by data source.
	std::map<cty_type_t, std::chrono::system_clock::time_point> timestamps_;
	//! Time at start of loading.
	std::chrono::system_clock::time_point now_;
	//! Mapping of data versions by data source.
	std::map<cty_type_t, std::string> versions_;

};
//! Json Serilaisation from cty_data::all_data
void to_json(json& j, const cty_data::all_data& d);
//! JSON Serialisation to cty_data::all_data
void from_json(const json& j, cty_data::all_data& d);
