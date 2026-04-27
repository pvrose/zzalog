#pragma once

#include <cstdint>

//! File_holder customisation - debug switches
const uint32_t DEBUG_RESET_ADIF = 1;        //!< Reset all.json (ADIF)
const uint32_t DEBUG_RESET_BAND = 1 << 1;   //!< Reset band_plan.json
const uint32_t DEBUG_RESET_CTY = 1 << 2;    //!< Reset cty.json
const uint32_t DEBUG_RESET_INTL = 1 << 3;   //!< Reset intl_chars.txt
const uint32_t DEBUG_RESET_APPS = 1 << 4;   //!< Reset apps.json
const uint32_t DEBUG_RESET_SETT = 1 << 5;   //!< Reset settings.json
const uint32_t DEBUG_RESET_RIGS = 1 << 6;   //!< Reset rigs.json
const uint32_t DEBUG_RESET_FLDS = 1 << 7;   //!< Reset fields.json
const uint32_t DEBUG_RESET_TEST = 1 << 8;   //!< Reset contests.json and contest algorithms
const uint32_t DEBUG_RESET_ICON = 1 << 9;   //!< Reset Icons
const uint32_t DEBUG_RESET_STN = 1 << 10;   //!< Reset station.json
const uint32_t DEBUG_RESET_CTY1 = 1 << 11;    //!< Reset cty.xml
const uint32_t DEBUG_RESET_CTY2 = 1 << 12;    //!< Reset cty.csv
const uint32_t DEBUG_RESET_CTY3 = 1 << 13;    //!< Reset prefix.lst
const uint32_t DEBUG_RESET_CTY4 = 1 << 14;    //!< Reset iso.csv
const uint32_t DEBUG_RESET_QSL = 1 << 15;     //!< Reset qsl.json
const uint32_t DEBUG_RESET_KEYS = 1 << 16;    //!< Reset keys.json
const uint32_t DEBUG_RESET_ALL = 0x0FFFF;    //!< Reset all (except keys)
const uint32_t DEBUG_RESET_CALL =
DEBUG_RESET_CTY |
DEBUG_RESET_CTY1 |
DEBUG_RESET_CTY2 |
DEBUG_RESET_CTY3 |
DEBUG_RESET_CTY4;                       //!< Reset all country files
const uint32_t DEBUG_RESET_REFS =
DEBUG_RESET_ADIF |
DEBUG_RESET_BAND |
DEBUG_RESET_CALL |
DEBUG_RESET_INTL |
DEBUG_RESET_ICON |
DEBUG_RESET_KEYS;                   //!< Reset all reference data
