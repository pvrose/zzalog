#[=[
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

#]=]
cmake_minimum_required(VERSION 3.16)

project(ZZALOG VERSION 3.7.0.1)

# Add shared CMake modules
# First look in sibling path for development, then in installed location for users
include(FetchContent)

# Try to find zzacmake as a sibling project first
set(ZZACMAKE_SIBLING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../zzacmake")
if(EXISTS "${ZZACMAKE_SIBLING_DIR}/cmake")
    set(zzacmake_SOURCE_DIR "${ZZACMAKE_SIBLING_DIR}")
    message(STATUS "Using zzacmake from sibling project: ${zzacmake_SOURCE_DIR}")
else()
    # Fall back to fetching from GitHub
    FetchContent_Declare(
        zzacmake
        GIT_REPOSITORY https://github.com/pvrose/zzacmake.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(zzacmake)
    message(STATUS "Fetched zzacmake from GitHub: ${zzacmake_SOURCE_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH "${zzacmake_SOURCE_DIR}/cmake")

include(Gm3zzaProjectDefaults)
include(Gm3zzaVersioning)
include(Gm3zzaZzacommon)
include(Gm3zzaResources)
include(Gm3zzaWindowsRuntime)
include(Gm3zzaDocs)
include(deps/hamlib_helper)

## Set target executable and programme metadata
set(TARGET zzalog)
set(APP_VENDOR "GM3ZZA")
set(APP_NAME "ZZALOG")
set(APP_RC_VALUE "")
string(TIMESTAMP APP_TIMESTAMP UTC)
set(APP_SOURCE_DIR ${ZZALOG_SOURCE_DIR})

## Initialise defaults and settings
gm3zza_project_defaults()
gm3zza_set_default_install_dirs("${APP_NAME}")
gm3zza_split_version(${PROJECT_NAME})

## Set version string for app.cpp.in
set(APP_VERSION "${ZZALOG_VERSION}${APP_RC_VALUE}")

# generate documents - add switches for conditional build.
option(ZZALOG_DOX "Generate Documentation" OFF)
option(ZZALOG_PDF "Generate PDF Document" OFF)

# Generate MSVC debugging symbols for all configurations
option(ZZALOG_PDB "Generate PDB files for debugging (MSVC only)" OFF)

# All the source .cpp files
set(CPPFILES 
  ${ZZALOG_SOURCE_DIR}/src/about_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/adi_reader.cpp
  ${ZZALOG_SOURCE_DIR}/src/adi_writer.cpp
  ${ZZALOG_SOURCE_DIR}/src/band_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/band_editor.cpp
  ${ZZALOG_SOURCE_DIR}/src/band_widget.cpp
  ${ZZALOG_SOURCE_DIR}/src/band_window.cpp
  ${ZZALOG_SOURCE_DIR}/src/book.cpp
  ${ZZALOG_SOURCE_DIR}/src/bumr_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/change_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/club_handler.cpp
  ${ZZALOG_SOURCE_DIR}/src/condx_view.cpp
  ${ZZALOG_SOURCE_DIR}/src/config.cpp
  ${ZZALOG_SOURCE_DIR}/src/contest_algo.cpp
  ${ZZALOG_SOURCE_DIR}/src/contest_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/contest_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/contest_scorer.cpp
  ${ZZALOG_SOURCE_DIR}/src/corr_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty1_reader.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty2_reader.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty3_reader.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty4_reader.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty_element.cpp
  ${ZZALOG_SOURCE_DIR}/src/cty_tree.cpp
  ${ZZALOG_SOURCE_DIR}/src/dxcc_table.cpp
  ${ZZALOG_SOURCE_DIR}/src/dxcc_view.cpp
  ${ZZALOG_SOURCE_DIR}/src/eqsl_handler.cpp
  ${ZZALOG_SOURCE_DIR}/src/extract_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/field_choice.cpp
  ${ZZALOG_SOURCE_DIR}/src/fields.cpp
  ${ZZALOG_SOURCE_DIR}/src/fields_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/fldigi_handler.cpp
  ${ZZALOG_SOURCE_DIR}/src/import_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/init_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/intl_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/intl_widgets.cpp
  ${ZZALOG_SOURCE_DIR}/src/keyring.cpp
  ${ZZALOG_SOURCE_DIR}/src/log_table.cpp
  ${ZZALOG_SOURCE_DIR}/src/lotw_handler.cpp
  ${ZZALOG_SOURCE_DIR}/src/main.cpp
  ${ZZALOG_SOURCE_DIR}/src/main_window.cpp
  ${ZZALOG_SOURCE_DIR}/src/menu_bar.cpp
  ${ZZALOG_SOURCE_DIR}/src/page_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/png_writer.cpp
  ${ZZALOG_SOURCE_DIR}/src/printer.cpp
  ${ZZALOG_SOURCE_DIR}/src/qrz_handler.cpp
  ${ZZALOG_SOURCE_DIR}/src/qsl_dataset.cpp
  ${ZZALOG_SOURCE_DIR}/src/qsl_display.cpp
  ${ZZALOG_SOURCE_DIR}/src/qsl_editor.cpp
  ${ZZALOG_SOURCE_DIR}/src/qsl_emailer.cpp
  ${ZZALOG_SOURCE_DIR}/src/qsl_image.cpp
  ${ZZALOG_SOURCE_DIR}/src/qsl_widget.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_apps.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_bands.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_buttons.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_clock.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_clocks.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_details.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_dxcc.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_entry.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_log.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_log_info.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_manager.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_misc.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_net_entry.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_operation.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_qsl.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_qsl_vwr.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_query.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_rig.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_tabbed_rigs.cpp
  ${ZZALOG_SOURCE_DIR}/src/qso_wx.cpp
  ${ZZALOG_SOURCE_DIR}/src/record.cpp
  ${ZZALOG_SOURCE_DIR}/src/record_table.cpp
  ${ZZALOG_SOURCE_DIR}/src/report_tree.cpp
  ${ZZALOG_SOURCE_DIR}/src/rig_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/rig_if.cpp
  ${ZZALOG_SOURCE_DIR}/src/scratchpad.cpp
  ${ZZALOG_SOURCE_DIR}/src/search_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/spec_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/spec_tree.cpp
  ${ZZALOG_SOURCE_DIR}/src/stn_call_dlg.cpp
  ${ZZALOG_SOURCE_DIR}/src/stn_data.cpp
  ${ZZALOG_SOURCE_DIR}/src/stn_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/stn_oper_dlg.cpp
  ${ZZALOG_SOURCE_DIR}/src/stn_qth_dlg.cpp
  ${ZZALOG_SOURCE_DIR}/src/tabbed_forms.cpp
  ${ZZALOG_SOURCE_DIR}/src/toolbar.cpp
  ${ZZALOG_SOURCE_DIR}/src/user_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/view.cpp
  ${ZZALOG_SOURCE_DIR}/src/web_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/win_dialog.cpp
  ${ZZALOG_SOURCE_DIR}/src/wsjtx_handler.cpp
  ${ZZALOG_SOURCE_DIR}/src/wx_handler.cpp

)

## All the header files - required for API documentation.
set(HPPFILES
  ${ZZALOG_SOURCE_DIR}/include/about_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/adi_reader.h
  ${ZZALOG_SOURCE_DIR}/include/adi_writer.h
  ${ZZALOG_SOURCE_DIR}/include/band_data.h
  ${ZZALOG_SOURCE_DIR}/include/band_editor.h
  ${ZZALOG_SOURCE_DIR}/include/band.h
  ${ZZALOG_SOURCE_DIR}/include/band_widget.h
  ${ZZALOG_SOURCE_DIR}/include/band_window.h
  ${ZZALOG_SOURCE_DIR}/include/book.h
  ${ZZALOG_SOURCE_DIR}/include/bumr_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/change_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/club_handler.h
  ${ZZALOG_SOURCE_DIR}/include/condx_view.h
  ${ZZALOG_SOURCE_DIR}/include/config.h
  ${ZZALOG_SOURCE_DIR}/include/contest_algo.h
  ${ZZALOG_SOURCE_DIR}/include/contest_data.h
  ${ZZALOG_SOURCE_DIR}/include/contest_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/contest_scorer.h
  ${ZZALOG_SOURCE_DIR}/include/corr_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/cty1_reader.h
  ${ZZALOG_SOURCE_DIR}/include/cty2_reader.h
  ${ZZALOG_SOURCE_DIR}/include/cty3_reader.h
  ${ZZALOG_SOURCE_DIR}/include/cty4_reader.h
  ${ZZALOG_SOURCE_DIR}/include/cty_data.h
  ${ZZALOG_SOURCE_DIR}/include/cty_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/cty_element.h
  ${ZZALOG_SOURCE_DIR}/include/cty_tree.h
  ${ZZALOG_SOURCE_DIR}/include/dxcc_table.h
  ${ZZALOG_SOURCE_DIR}/include/dxcc_view.h
  ${ZZALOG_SOURCE_DIR}/include/eqsl_handler.h
  ${ZZALOG_SOURCE_DIR}/include/extract_data.h
  ${ZZALOG_SOURCE_DIR}/include/field_choice.h
  ${ZZALOG_SOURCE_DIR}/include/fields_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/fields.h
  ${ZZALOG_SOURCE_DIR}/include/fldigi_handler.h
  ${ZZALOG_SOURCE_DIR}/include/icons.h
  ${ZZALOG_SOURCE_DIR}/include/import_data.h
  ${ZZALOG_SOURCE_DIR}/include/init_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/intl_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/intl_widgets.h
  ${ZZALOG_SOURCE_DIR}/include/keyring.h
  ${ZZALOG_SOURCE_DIR}/include/log_table.h
  ${ZZALOG_SOURCE_DIR}/include/lotw_handler.h
  ${ZZALOG_SOURCE_DIR}/include/main.h
  ${ZZALOG_SOURCE_DIR}/include/main_window.h
  ${ZZALOG_SOURCE_DIR}/include/menu_bar.h
  ${ZZALOG_SOURCE_DIR}/include/objects.h
  ${ZZALOG_SOURCE_DIR}/include/page_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/png_writer.h
  ${ZZALOG_SOURCE_DIR}/include/printer.h
  ${ZZALOG_SOURCE_DIR}/include/qrz_handler.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_data.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_dataset.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_display.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_editor.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_emailer.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_image.h
  ${ZZALOG_SOURCE_DIR}/include/qsl_widget.h
  ${ZZALOG_SOURCE_DIR}/include/qso_apps.h
  ${ZZALOG_SOURCE_DIR}/include/qso_bands.h
  ${ZZALOG_SOURCE_DIR}/include/qso_buttons.h
  ${ZZALOG_SOURCE_DIR}/include/qso_clock.h
  ${ZZALOG_SOURCE_DIR}/include/qso_clocks.h
  ${ZZALOG_SOURCE_DIR}/include/qso_data.h
  ${ZZALOG_SOURCE_DIR}/include/qso_details.h
  ${ZZALOG_SOURCE_DIR}/include/qso_dxcc.h
  ${ZZALOG_SOURCE_DIR}/include/qso_entry.h
  ${ZZALOG_SOURCE_DIR}/include/qso_log.h
  ${ZZALOG_SOURCE_DIR}/include/qso_log_info.h
  ${ZZALOG_SOURCE_DIR}/include/qso_manager.h
  ${ZZALOG_SOURCE_DIR}/include/qso_misc.h
  ${ZZALOG_SOURCE_DIR}/include/qso_net_entry.h
  ${ZZALOG_SOURCE_DIR}/include/qso_operation.h
  ${ZZALOG_SOURCE_DIR}/include/qso_qsl.h
  ${ZZALOG_SOURCE_DIR}/include/qso_qsl_vwr.h
  ${ZZALOG_SOURCE_DIR}/include/qso_query.h
  ${ZZALOG_SOURCE_DIR}/include/qso_rig.h
  ${ZZALOG_SOURCE_DIR}/include/qso_tabbed_rigs.h
  ${ZZALOG_SOURCE_DIR}/include/qso_wx.h
  ${ZZALOG_SOURCE_DIR}/include/record.h
  ${ZZALOG_SOURCE_DIR}/include/record_table.h
  ${ZZALOG_SOURCE_DIR}/include/regices.h
  ${ZZALOG_SOURCE_DIR}/include/report_tree.h
  ${ZZALOG_SOURCE_DIR}/include/rig_data.h
  ${ZZALOG_SOURCE_DIR}/include/rig_if.h
  ${ZZALOG_SOURCE_DIR}/include/scratchpad.h
  ${ZZALOG_SOURCE_DIR}/include/search_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/search.h
  ${ZZALOG_SOURCE_DIR}/include/spec_data.h
  ${ZZALOG_SOURCE_DIR}/include/spec_tree.h
  ${ZZALOG_SOURCE_DIR}/include/stn_call_dlg.h
  ${ZZALOG_SOURCE_DIR}/include/stn_data.h
  ${ZZALOG_SOURCE_DIR}/include/stn_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/stn_oper_dlg.h
  ${ZZALOG_SOURCE_DIR}/include/stn_qth_dlg.h
  ${ZZALOG_SOURCE_DIR}/include/tabbed_forms.h
  ${ZZALOG_SOURCE_DIR}/include/toolbar.h
  ${ZZALOG_SOURCE_DIR}/include/user_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/view.h
  ${ZZALOG_SOURCE_DIR}/include/web_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/win_dialog.h
  ${ZZALOG_SOURCE_DIR}/include/wsjtx_handler.h
  ${ZZALOG_SOURCE_DIR}/include/wx_handler.h
)


## Reference data
set(DATAFILES
  ${ZZALOG_SOURCE_DIR}/reference/all.json
  ${ZZALOG_SOURCE_DIR}/reference/band_plan.json
  ${ZZALOG_SOURCE_DIR}/reference/cty.csv
  ${ZZALOG_SOURCE_DIR}/reference/cty.xml
  ${ZZALOG_SOURCE_DIR}/reference/Prefix.lst
  ${ZZALOG_SOURCE_DIR}/reference/iso.csv
  ${ZZALOG_SOURCE_DIR}/reference/intl_chars.txt
  ${ZZALOG_SOURCE_DIR}/reference/google-maps.png
  ${ZZALOG_SOURCE_DIR}/reference/pdf.png
  ${ZZALOG_SOURCE_DIR}/reference/qrz_1.jpg
  ${ZZALOG_SOURCE_DIR}/rose.png
  ${ZZALOG_SOURCE_DIR}/reference/keys.json
)

## Generate Windows icon and resource file
if(MSVC)
  if(NOT DEFINED ZZALOG_ICON_WHITE_THRESHOLD)
    set(ZZALOG_ICON_WHITE_THRESHOLD 240)
  endif()
  
  gm3zza_enable_windows_icon(
    TARGET ${TARGET}
    PNG_FILE "${ZZALOG_SOURCE_DIR}/rose.png"
    RC_TEMPLATE "${ZZALOG_SOURCE_DIR}/zzalog.rc.in"
    WHITE_THRESHOLD ${ZZALOG_ICON_WHITE_THRESHOLD}
    OUTPUT_VAR APP_RC_FILE
  )
endif()

## Source file generation in binary tree
gm3zza_generate_app_source(
  INPUT "${ZZALOG_SOURCE_DIR}/src/app.cpp.in"
  OUTPUT_VAR GENERATED_APP_CPP
)

set(ALL_CPPFILES ${GENERATED_APP_CPP} ${CPPFILES})

gm3zza_find_hamlib()


## Use shared zzacommon (includes zzaf, zzam, zzad, zzab, zzafb)
if(ZZALOG_DOX)
    gm3zza_use_zzacommon(COMPONENTS zzaf zzam zzad zzab zzafb zzax GIT_TAG master ENABLE_DOCS)
else()
    gm3zza_use_zzacommon(COMPONENTS zzaf zzam zzad zzab zzafb zzax GIT_TAG master)
endif()


## Add all contest algorithm files to separate list
file(GLOB CONTEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/reference/contests/*")

## Define the zzalog executable
if (MSVC)
  add_executable(${TARGET} WIN32 ${APP_RC_FILE})
else()
  add_executable(${TARGET} WIN32)
endif()

# Ensure external projects are built before linking zzalog
add_dependencies(${TARGET} 
  zzacommon::zzaf
  zzacommon::zzad
  zzacommon::zzab
  zzacommon::zzax
  zzacommon::zzafb
  zzacommon::zzam
)

## Set header include directoies.
target_include_directories(${TARGET} PRIVATE 
  ${ZZALOG_SOURCE_DIR}/include 
  ${ZZACOMMON_INCLUDE_DIR}
  ${FLTK_INCLUDE_DIR}
  ${HAMLIB_INCLUDE_DIR}
)

## Extra include directories required on MSVC
if (MSVC)
  message(STATUS "Including other libraries for MSVC from ${USER_HOME_DIR}")
  target_include_directories(${TARGET} PRIVATE 
    "${USER_HOME_DIR}\\source\\repos\\json\\include"
    "${USER_HOME_DIR}\\source\\repos\\pthread-win32"
  )
endif()

## Define sources to be compiled.
target_sources(${TARGET} PRIVATE ${ALL_CPPFILES})

## Define libraries to be linked in.
target_link_libraries(${TARGET} PRIVATE 
  ${HAMLIB_LIBRARY}
  zzacommon::zzaf
  zzacommon::zzad
  zzacommon::zzab
  zzacommon::zzax
  zzacommon::zzafb
  zzacommon::zzam
)

## Automatically copy zlib1.dll and Boost DLLs to the output directory
zzacommon_copy_runtime_dlls(${TARGET})

## Copy hamlib DLLs to the output directory for development
gm3zza_copy_hamlib_dlls(TARGET ${TARGET})

## Register hamlib DLLs for installation
gm3zza_register_hamlib_dlls()

## Stage runtime assets for development
gm3zza_stage_runtime_files(TARGET ${TARGET} FILES ${DATAFILES})
gm3zza_stage_runtime_files(TARGET ${TARGET} FILES ${CONTEST_FILES} DEST_SUBDIR contests)

## Copy runtime DLLs for development
gm3zza_copy_runtime_dlls(TARGET ${TARGET})


# Generate documents - userguide and API documentation.
if(ZZALOG_DOX)
  # Paths to the include directories used by api/Doxyfile.in
  set(ZZALOG_INCLUDE_DIR          "${ZZALOG_SOURCE_DIR}/include")

  if(ZZALOG_PDF)
    set(_docs_pdf_flag PDF)
  else()
    set(_docs_pdf_flag "")
  endif()

  gm3zza_enable_docs(
    TARGET        ${TARGET}
    APP_NAME      ${APP_NAME}
    USERGUIDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/userguide"
    API_DIR       "${CMAKE_CURRENT_SOURCE_DIR}/api"
    API_FILES     ${HPPFILES}
    ${_docs_pdf_flag}
  )

  # Copy zzacommon API docs into the api html output tree
  zzacommon_copy_api_docs(${GM3ZZA_API_HTML_DIR}/zzacommon)
endif()

### Install
install(TARGETS ${TARGET} RUNTIME COMPONENT applications)
gm3zza_install_runtime_files(FILES ${DATAFILES} COMPONENT data)

if(MSVC)
  gm3zza_register_hamlib_dlls()
  zzacommon_install_runtime_dlls(DESTINATION bin COMPONENT application)
  gm3zza_install_registered_runtime_dlls()
endif()


# Install userguide and API documentation
if(ZZALOG_DOX)
    if(GM3ZZA_USERGUIDE_HTML_DIR)
      gm3zza_install_runtime_files(
        DIRECTORIES ${GM3ZZA_USERGUIDE_HTML_DIR}
        DEST_SUBDIR userguide
        COMPONENT documentation
      )
    endif()

    if(ZZALOG_PDF AND GM3ZZA_USERGUIDE_PDF_FILE)
      gm3zza_install_runtime_files(
        FILES ${GM3ZZA_USERGUIDE_PDF_FILE}
        DEST_SUBDIR userguide
        COMPONENT documentation
      )
    endif()

    if(GM3ZZA_API_HTML_DIR)
      gm3zza_install_runtime_files(
        DIRECTORIES ${GM3ZZA_API_HTML_DIR}
        DEST_SUBDIR api
        COMPONENT documentation
      )
    endif()
endif()

include(InstallRequiredSystemLibraries)

