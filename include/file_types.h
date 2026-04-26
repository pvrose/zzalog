#pragma once

#include <cstdint>
#include "zc_file_holder.h"

//! File holder customisations - file tags
enum file_types : uint8_t {
	FILE_ADIF = file_contents_t::FILE_USER,                  //!< ADIF Specification
	FILE_BANDPLAN,                          //!< Band-plan data
	FILE_COUNTRY_CLUB,                      //!< Country data from Clublog.org
	FILE_COUNTRY_CFILES,                    //!< Country data from country-files.com
	FILE_COUNTRY_DXATLAS,                   //!< Country data from DxAtlas
	FILE_COUNTRY_ISO,                       //!< ISO country data
	FILE_COUNTRY,                           //!< Collated country data
	FILE_INTLCHARS,                         //!< International character set
	FILE_ICON_GMAPS,                        //!< Icon for google maps
	FILE_ICON_PDF,                          //!< Icon for PDF
	FILE_ICON_QRZ,                          //!< Icon for QRZ.com
	FILE_APPS,                              //!< Application configuration file
	FILE_RIGS,                              //!< Rig configuration file
	FILE_FIELDS,                            //!< Field usage configuration file
	FILE_CONTEST,                           //!< Contests configuration file
	FILE_SOLAR,                             //!< Solar data (read every hour at most frequent
	FILE_STATION,                           //!< Station configuration file
	FILE_QSL,                               //!< QSL configuration file
	FILE_KEYS,                              //!< On-line API keys (non-user)
	FILE_CONTESTS,                          //!< Space for several contest algorithms
};

