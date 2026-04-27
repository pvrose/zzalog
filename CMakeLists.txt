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

project(ZZALOG VERSION 3.6.16.5)
set(ZZALOG_SRC_DIR ${ZZALOG_SOURCE_DIR}/src)
set(ZZALOG_INCLUDE_DIR ${ZZALOG_SOURCE_DIR}/include)
set(ZZALOG_RC_VALUE "")

## Set program details - used by zzacommon
set(APP_VENDOR "GM3ZZA")
set(APP_NAME "ZZALOG")
string(TIMESTAMP APP_TIMESTAMP UTC)
set(APP_VERSION "${ZZALOG_VERSION}${ZZALOG_RC_VALUE}")
set(APP_SOURCE_DIR ${ZZALOG_SOURCE_DIR})

## Split version into components for Windows resource file
string(REPLACE "." ";" VERSION_LIST ${ZZALOG_VERSION})
list(GET VERSION_LIST 0 ZZALOG_VERSION_MAJOR)
list(GET VERSION_LIST 1 ZZALOG_VERSION_MINOR)
list(GET VERSION_LIST 2 ZZALOG_VERSION_PATCH)
list(GET VERSION_LIST 3 ZZALOG_VERSION_TWEAK)

## Set target executable
set(TARGET zzalog)
set(BUILD_SHARED_LIBS OFF)

## Set C++ requirements.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Enable compile_commands.json generation for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## Set home directory per OS.
if(MSVC)
  set(USER_HOME_DIR $ENV{USERPROFILE})
else()
  set(USER_HOME_DIR $ENV{HOME})
endif()

# generate documents - add switches for conditional build.
option(ZZALOG_DOX "Generate Documentation" OFF)
option(ZZALOG_PDF "Generate PDF Document" OFF)

# Generate MSVC debugging symbols for all configurations
option(ZZALOG_PDB "Generate PDB files for debugging (MSVC only)" OFF)

# All the source .cpp files
set(ZZALOG_CPPFILES 
  ${ZZALOG_SRC_DIR}/about_dialog.cpp
  ${ZZALOG_SRC_DIR}/adi_reader.cpp
  ${ZZALOG_SRC_DIR}/adi_writer.cpp
  ${ZZALOG_BINARY_DIR}/src/app.cpp
  ${ZZALOG_SRC_DIR}/band_data.cpp
  ${ZZALOG_SRC_DIR}/band_editor.cpp
  ${ZZALOG_SRC_DIR}/band_widget.cpp
  ${ZZALOG_SRC_DIR}/band_window.cpp
  ${ZZALOG_SRC_DIR}/book.cpp
  ${ZZALOG_SRC_DIR}/bumr_dialog.cpp
  ${ZZALOG_SRC_DIR}/change_dialog.cpp
  ${ZZALOG_SRC_DIR}/club_handler.cpp
  ${ZZALOG_SRC_DIR}/condx_view.cpp
  ${ZZALOG_SRC_DIR}/config.cpp
  ${ZZALOG_SRC_DIR}/contest_algo.cpp
  ${ZZALOG_SRC_DIR}/contest_data.cpp
  ${ZZALOG_SRC_DIR}/contest_dialog.cpp
  ${ZZALOG_SRC_DIR}/contest_scorer.cpp
  ${ZZALOG_SRC_DIR}/corr_dialog.cpp
  ${ZZALOG_SRC_DIR}/cty1_reader.cpp
  ${ZZALOG_SRC_DIR}/cty2_reader.cpp
  ${ZZALOG_SRC_DIR}/cty3_reader.cpp
  ${ZZALOG_SRC_DIR}/cty4_reader.cpp
  ${ZZALOG_SRC_DIR}/cty_data.cpp
  ${ZZALOG_SRC_DIR}/cty_dialog.cpp
  ${ZZALOG_SRC_DIR}/cty_element.cpp
  ${ZZALOG_SRC_DIR}/cty_tree.cpp
  ${ZZALOG_SRC_DIR}/dxcc_table.cpp
  ${ZZALOG_SRC_DIR}/dxcc_view.cpp
  ${ZZALOG_SRC_DIR}/eqsl_handler.cpp
  ${ZZALOG_SRC_DIR}/extract_data.cpp
  ${ZZALOG_SRC_DIR}/field_choice.cpp
  ${ZZALOG_SRC_DIR}/fields.cpp
  ${ZZALOG_SRC_DIR}/fields_dialog.cpp
  ${ZZALOG_SRC_DIR}/fldigi_handler.cpp
  ${ZZALOG_SRC_DIR}/import_data.cpp
  ${ZZALOG_SRC_DIR}/init_dialog.cpp
  ${ZZALOG_SRC_DIR}/intl_dialog.cpp
  ${ZZALOG_SRC_DIR}/intl_widgets.cpp
  ${ZZALOG_SRC_DIR}/keyring.cpp
  ${ZZALOG_SRC_DIR}/log_table.cpp
  ${ZZALOG_SRC_DIR}/lotw_handler.cpp
  ${ZZALOG_SRC_DIR}/main.cpp
  ${ZZALOG_SRC_DIR}/main_window.cpp
  ${ZZALOG_SRC_DIR}/menu_bar.cpp
  ${ZZALOG_SRC_DIR}/page_dialog.cpp
  ${ZZALOG_SRC_DIR}/png_writer.cpp
  ${ZZALOG_SRC_DIR}/printer.cpp
  ${ZZALOG_SRC_DIR}/qrz_handler.cpp
  ${ZZALOG_SRC_DIR}/qsl_dataset.cpp
  ${ZZALOG_SRC_DIR}/qsl_display.cpp
  ${ZZALOG_SRC_DIR}/qsl_editor.cpp
  ${ZZALOG_SRC_DIR}/qsl_emailer.cpp
  ${ZZALOG_SRC_DIR}/qsl_image.cpp
  ${ZZALOG_SRC_DIR}/qsl_widget.cpp
  ${ZZALOG_SRC_DIR}/qso_apps.cpp
  ${ZZALOG_SRC_DIR}/qso_bands.cpp
  ${ZZALOG_SRC_DIR}/qso_buttons.cpp
  ${ZZALOG_SRC_DIR}/qso_clock.cpp
  ${ZZALOG_SRC_DIR}/qso_clocks.cpp
  ${ZZALOG_SRC_DIR}/qso_data.cpp
  ${ZZALOG_SRC_DIR}/qso_details.cpp
  ${ZZALOG_SRC_DIR}/qso_dxcc.cpp
  ${ZZALOG_SRC_DIR}/qso_entry.cpp
  ${ZZALOG_SRC_DIR}/qso_log.cpp
  ${ZZALOG_SRC_DIR}/qso_log_info.cpp
  ${ZZALOG_SRC_DIR}/qso_manager.cpp
  ${ZZALOG_SRC_DIR}/qso_misc.cpp
  ${ZZALOG_SRC_DIR}/qso_net_entry.cpp
  ${ZZALOG_SRC_DIR}/qso_operation.cpp
  ${ZZALOG_SRC_DIR}/qso_qsl.cpp
  ${ZZALOG_SRC_DIR}/qso_qsl_vwr.cpp
  ${ZZALOG_SRC_DIR}/qso_query.cpp
  ${ZZALOG_SRC_DIR}/qso_rig.cpp
  ${ZZALOG_SRC_DIR}/qso_tabbed_rigs.cpp
  ${ZZALOG_SRC_DIR}/qso_wx.cpp
  ${ZZALOG_SRC_DIR}/record.cpp
  ${ZZALOG_SRC_DIR}/record_table.cpp
  ${ZZALOG_SRC_DIR}/report_tree.cpp
  ${ZZALOG_SRC_DIR}/rig_data.cpp
  ${ZZALOG_SRC_DIR}/rig_if.cpp
  ${ZZALOG_SRC_DIR}/scratchpad.cpp
  ${ZZALOG_SRC_DIR}/search_dialog.cpp
  ${ZZALOG_SRC_DIR}/spec_data.cpp
  ${ZZALOG_SRC_DIR}/spec_tree.cpp
  ${ZZALOG_SRC_DIR}/stn_call_dlg.cpp
  ${ZZALOG_SRC_DIR}/stn_data.cpp
  ${ZZALOG_SRC_DIR}/stn_dialog.cpp
  ${ZZALOG_SRC_DIR}/stn_oper_dlg.cpp
  ${ZZALOG_SRC_DIR}/stn_qth_dlg.cpp
  ${ZZALOG_SRC_DIR}/tabbed_forms.cpp
  ${ZZALOG_SRC_DIR}/toolbar.cpp
  ${ZZALOG_SRC_DIR}/user_dialog.cpp
  ${ZZALOG_SRC_DIR}/view.cpp
  ${ZZALOG_SRC_DIR}/web_dialog.cpp
  ${ZZALOG_SRC_DIR}/win_dialog.cpp
  ${ZZALOG_SRC_DIR}/wsjtx_handler.cpp
  ${ZZALOG_SRC_DIR}/wx_handler.cpp

)

## All the header files - required for API documentation.
set( ZZALOG_HPPFILES
  ${ZZALOG_INCLUDE_DIR}/about_dialog.h
  ${ZZALOG_INCLUDE_DIR}/adi_reader.h
  ${ZZALOG_INCLUDE_DIR}/adi_writer.h
  ${ZZALOG_INCLUDE_DIR}/band_data.h
  ${ZZALOG_INCLUDE_DIR}/band_editor.h
  ${ZZALOG_INCLUDE_DIR}/band.h
  ${ZZALOG_INCLUDE_DIR}/band_widget.h
  ${ZZALOG_INCLUDE_DIR}/band_window.h
  ${ZZALOG_INCLUDE_DIR}/book.h
  ${ZZALOG_INCLUDE_DIR}/bumr_dialog.h
  ${ZZALOG_INCLUDE_DIR}/change_dialog.h
  ${ZZALOG_INCLUDE_DIR}/club_handler.h
  ${ZZALOG_INCLUDE_DIR}/condx_view.h
  ${ZZALOG_INCLUDE_DIR}/config.h
  ${ZZALOG_INCLUDE_DIR}/contest_algo.h
  ${ZZALOG_INCLUDE_DIR}/contest_data.h
  ${ZZALOG_INCLUDE_DIR}/contest_dialog.h
  ${ZZALOG_INCLUDE_DIR}/contest_scorer.h
  ${ZZALOG_INCLUDE_DIR}/corr_dialog.h
  ${ZZALOG_INCLUDE_DIR}/cty1_reader.h
  ${ZZALOG_INCLUDE_DIR}/cty2_reader.h
  ${ZZALOG_INCLUDE_DIR}/cty3_reader.h
  ${ZZALOG_INCLUDE_DIR}/cty4_reader.h
  ${ZZALOG_INCLUDE_DIR}/cty_data.h
  ${ZZALOG_INCLUDE_DIR}/cty_dialog.h
  ${ZZALOG_INCLUDE_DIR}/cty_element.h
  ${ZZALOG_INCLUDE_DIR}/cty_tree.h
  ${ZZALOG_INCLUDE_DIR}/dxcc_table.h
  ${ZZALOG_INCLUDE_DIR}/dxcc_view.h
  ${ZZALOG_INCLUDE_DIR}/eqsl_handler.h
  ${ZZALOG_INCLUDE_DIR}/extract_data.h
  ${ZZALOG_INCLUDE_DIR}/field_choice.h
  ${ZZALOG_INCLUDE_DIR}/fields_dialog.h
  ${ZZALOG_INCLUDE_DIR}/fields.h
  ${ZZALOG_INCLUDE_DIR}/fldigi_handler.h
  ${ZZALOG_INCLUDE_DIR}/icons.h
  ${ZZALOG_INCLUDE_DIR}/import_data.h
  ${ZZALOG_INCLUDE_DIR}/init_dialog.h
  ${ZZALOG_INCLUDE_DIR}/intl_dialog.h
  ${ZZALOG_INCLUDE_DIR}/intl_widgets.h
  ${ZZALOG_INCLUDE_DIR}/keyring.h
  ${ZZALOG_INCLUDE_DIR}/log_table.h
  ${ZZALOG_INCLUDE_DIR}/lotw_handler.h
  ${ZZALOG_INCLUDE_DIR}/main.h
  ${ZZALOG_INCLUDE_DIR}/main_window.h
  ${ZZALOG_INCLUDE_DIR}/menu_bar.h
  ${ZZALOG_INCLUDE_DIR}/objects.h
  ${ZZALOG_INCLUDE_DIR}/page_dialog.h
  ${ZZALOG_INCLUDE_DIR}/png_writer.h
  ${ZZALOG_INCLUDE_DIR}/printer.h
  ${ZZALOG_INCLUDE_DIR}/qrz_handler.h
  ${ZZALOG_INCLUDE_DIR}/qsl_data.h
  ${ZZALOG_INCLUDE_DIR}/qsl_dataset.h
  ${ZZALOG_INCLUDE_DIR}/qsl_display.h
  ${ZZALOG_INCLUDE_DIR}/qsl_editor.h
  ${ZZALOG_INCLUDE_DIR}/qsl_emailer.h
  ${ZZALOG_INCLUDE_DIR}/qsl_image.h
  ${ZZALOG_INCLUDE_DIR}/qsl_widget.h
  ${ZZALOG_INCLUDE_DIR}/qso_apps.h
  ${ZZALOG_INCLUDE_DIR}/qso_bands.h
  ${ZZALOG_INCLUDE_DIR}/qso_buttons.h
  ${ZZALOG_INCLUDE_DIR}/qso_clock.h
  ${ZZALOG_INCLUDE_DIR}/qso_clocks.h
  ${ZZALOG_INCLUDE_DIR}/qso_data.h
  ${ZZALOG_INCLUDE_DIR}/qso_details.h
  ${ZZALOG_INCLUDE_DIR}/qso_dxcc.h
  ${ZZALOG_INCLUDE_DIR}/qso_entry.h
  ${ZZALOG_INCLUDE_DIR}/qso_log.h
  ${ZZALOG_INCLUDE_DIR}/qso_log_info.h
  ${ZZALOG_INCLUDE_DIR}/qso_manager.h
  ${ZZALOG_INCLUDE_DIR}/qso_misc.h
  ${ZZALOG_INCLUDE_DIR}/qso_net_entry.h
  ${ZZALOG_INCLUDE_DIR}/qso_operation.h
  ${ZZALOG_INCLUDE_DIR}/qso_qsl.h
  ${ZZALOG_INCLUDE_DIR}/qso_qsl_vwr.h
  ${ZZALOG_INCLUDE_DIR}/qso_query.h
  ${ZZALOG_INCLUDE_DIR}/qso_rig.h
  ${ZZALOG_INCLUDE_DIR}/qso_tabbed_rigs.h
  ${ZZALOG_INCLUDE_DIR}/qso_wx.h
  ${ZZALOG_INCLUDE_DIR}/record.h
  ${ZZALOG_INCLUDE_DIR}/record_table.h
  ${ZZALOG_INCLUDE_DIR}/regices.h
  ${ZZALOG_INCLUDE_DIR}/report_tree.h
  ${ZZALOG_INCLUDE_DIR}/rig_data.h
  ${ZZALOG_INCLUDE_DIR}/rig_if.h
  ${ZZALOG_INCLUDE_DIR}/scratchpad.h
  ${ZZALOG_INCLUDE_DIR}/search_dialog.h
  ${ZZALOG_INCLUDE_DIR}/search.h
  ${ZZALOG_INCLUDE_DIR}/spec_data.h
  ${ZZALOG_INCLUDE_DIR}/spec_tree.h
  ${ZZALOG_INCLUDE_DIR}/stn_call_dlg.h
  ${ZZALOG_INCLUDE_DIR}/stn_data.h
  ${ZZALOG_INCLUDE_DIR}/stn_dialog.h
  ${ZZALOG_INCLUDE_DIR}/stn_oper_dlg.h
  ${ZZALOG_INCLUDE_DIR}/stn_qth_dlg.h
  ${ZZALOG_INCLUDE_DIR}/tabbed_forms.h
  ${ZZALOG_INCLUDE_DIR}/toolbar.h
  ${ZZALOG_INCLUDE_DIR}/user_dialog.h
  ${ZZALOG_INCLUDE_DIR}/view.h
  ${ZZALOG_INCLUDE_DIR}/web_dialog.h
  ${ZZALOG_INCLUDE_DIR}/win_dialog.h
  ${ZZALOG_INCLUDE_DIR}/wsjtx_handler.h
  ${ZZALOG_INCLUDE_DIR}/wx_handler.h
)

# Generate PDB files for all configurations on MSVC
if(MSVC AND ZZALOG_PDB)
  # For RelWithDebInfo and Release builds
  string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " /Zi")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " /Zi")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO " /DEBUG /OPT:REF /OPT:ICF")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE " /DEBUG /OPT:REF /OPT:ICF")
endif()


# set directory to install executable, reference data and documents.
if (MSVC)
  set(CMAKE_INSTALL_RPATH etc)
else()
  set(CMAKE_INSTALL_RPATH etc/${TARGET})
endif()
message(STATUS "Reference data will be copied to ${CMAKE_INSTALL_RPATH}")

# Set target directory for .dlls and development-mode data
set (ZZALOG_RUN_DIR ${ZZALOG_BINARY_DIR})

# set compiler options
if (MSVC)
else()
  add_compile_options(-g)
endif()

# Set CMake module path to pick up local CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Set CMake prefix path to pick up vcpkg-installed packages
list(APPEND CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-windows/share")
include(hamlib_helper)

find_hamlib()

# Add ZZACOMMON
if (ZZALOG_DOX)
  # If generating documentation, ensure zzacommon API docs are generated first
  set(ZZACOMMON_DOX ON CACHE BOOL "Generate documentation for zzacommon" FORCE)
endif()
# Try to find zzacommon locally first (sibling directory), fall back to downloading
if(CMAKE_BUILD_TYPE)
  # Single-config generator (Unix Makefiles, Ninja): use specific build type
  string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
  list(APPEND CMAKE_PREFIX_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/build/x64-${BUILD_TYPE_LOWER}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/install/x64-${BUILD_TYPE_LOWER}"
  )
endif()
# Multi-config generator (Visual Studio) or fallback: include both
list(APPEND CMAKE_PREFIX_PATH 
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/build/x64-debug"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/build/x64-release"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/install/x64-debug"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/install/x64-release"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/build"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/install"
)

include(FetchContent)

# Ensure FetchContent fallback builds the same zzacommon components expected by this project.
set(ZZACOMMON_COMPONENTS "zzaf;zzab;zzax;zzam;zzafb;zzad")

FetchContent_Declare(
  zzacommon
  GIT_REPOSITORY https://github.com/pvrose/zzacommon.git
  GIT_TAG v1.1.2
  FIND_PACKAGE_ARGS NAMES zzacommon COMPONENTS zzaf zzam zzafb zzad zzab zzax
)
FetchContent_MakeAvailable(zzacommon)

configure_file(
  ${ZZALOG_SOURCE_DIR}/src/app.cpp.in
  ${ZZALOG_BINARY_DIR}/src/app.cpp
  @ONLY
)

## Reference data
set(DATAFILES
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/all.json
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/band_plan.json
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/cty.csv
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/cty.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/Prefix.lst
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/iso.csv
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/intl_chars.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/google-maps.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/pdf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/qrz_1.jpg
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/rose.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/keys.json
)

## Add all contest algorithm files to separate list
file(GLOB CONTEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/reference/contests/*")

# Generate Windows icon and resource file
if (MSVC)
  # Generate ICO file from PNG logo
  set(ZZALOG_LOGO_PNG "${CMAKE_CURRENT_SOURCE_DIR}/rose.png")
  set(ZZALOG_ICO_FILE "${ZZALOG_BINARY_DIR}/zzalog.ico")

  if(EXISTS "${ZZALOG_LOGO_PNG}")
    message(STATUS "Generating Windows icon from ${ZZALOG_LOGO_PNG}")
    execute_process(
      COMMAND ${CMAKE_COMMAND}
        -DPNG_FILE=${ZZALOG_LOGO_PNG}
        -DICO_FILE=${ZZALOG_ICO_FILE}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/convert_png_to_ico.cmake
      RESULT_VARIABLE ICO_RESULT
      ERROR_VARIABLE ICO_ERROR
    )
    if(NOT ICO_RESULT EQUAL 0)
      message(WARNING "Failed to generate ICO file: ${ICO_ERROR}")
    endif()
  else()
    message(WARNING "Logo file not found: ${ZZALOG_LOGO_PNG}")
  endif()

  # Configure Windows resource file with version information
  set(ZZALOG_RC_FILE "${ZZALOG_BINARY_DIR}/zzalog.rc")
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/zzalog.rc.in
    ${ZZALOG_RC_FILE}
    @ONLY
  )
  message(STATUS "Generated Windows resource file: ${ZZALOG_RC_FILE}")
endif()

# Define the zzalog executable
if (MSVC)
  add_executable(${TARGET} WIN32 ${ZZALOG_RC_FILE})
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
  ${ZZALOG_INCLUDE_DIR} 
  ${ZZALOG_SOURCE_DIR}/contests/include
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
target_sources(${TARGET} PRIVATE ${ZZALOG_CPPFILES})

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


# Copy runtime DLLs to build directory for MSVC
if (MSVC)
  # Get the parent project's binary directory
  set(RUNTIME_OUTPUT_DIR ${CMAKE_BINARY_DIR})

  # Use CMake's automatic runtime DLL detection
  add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:${TARGET}>
      ${RUNTIME_OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    COMMENT "Copying runtime DLLs to ${RUNTIME_OUTPUT_DIR}"
  )


endif()



# copy reference data
add_custom_target(reference_data ALL)
message(STATUS "Reference data: ${DATAFILES}")
message(STATUS "Contest files: ${CONTEST_FILES}")

# Copy main reference files to root
file(COPY ${DATAFILES}
  DESTINATION ${ZZALOG_RUN_DIR}
  USE_SOURCE_PERMISSIONS
)

# Copy contest algorithm files to separate subdirectory
file(COPY ${CONTEST_FILES}
  DESTINATION ${ZZALOG_RUN_DIR}/contests
  USE_SOURCE_PERMISSIONS
)

# Generate documents - generate userguide.
if(ZZALOG_DOX) 
  message(STATUS "ZZALOG: Adding userguide")
  add_subdirectory(userguide)
  add_dependencies(${TARGET} ug_html)
  if (ZZALOG_PDF)
      message(STATUS "ZZALOG: Adding userguide PDF")
      add_dependencies(${TARGET} pdf)
  endif()
  message(STATUS "ZZALOG: Adding API documentation")
  add_subdirectory(api)
  add_dependencies(${TARGET} api_html)
  zzacommon_copy_api_docs(${ZZALOG_API_HTML_DIR}/zzacommon)
endif()

# install executable
install(TARGETS ${TARGET} 
  RUNTIME
  COMPONENT applications
)

# install PDB file for debugging (MSVC only)
if(MSVC)
  install(FILES $<TARGET_PDB_FILE:${TARGET}>
    DESTINATION bin
    COMPONENT applications
    OPTIONAL
  )
endif()
# install reference data
install(FILES ${DATAFILES}
  DESTINATION ${CMAKE_INSTALL_RPATH}
  COMPONENT data
)
# install DLLs
if (MSVC)
    install_hamlib_dlls()
    zzacommon_install_runtime_dlls(DESTINATION bin COMPONENT application)
endif()

# install userguide HTML files and PDF
if(ZZALOG_DOX)
    install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/userguide/html
    DESTINATION ${CMAKE_INSTALL_RPATH}/userguide
    COMPONENT documentation
    MESSAGE_NEVER
    )
    if (ZZALOG_PDF)
        install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/userguide/${APP_NAME}.pdf
        DESTINATION ${CMAKE_INSTALL_RPATH}/userguide
        COMPONENT documentation
        )
    endif(ZZALOG_PDF)
    # install API documentation HTML filesn
    install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/html
    DESTINATION ${CMAKE_INSTALL_RPATH}/api
    COMPONENT documentation
    MESSAGE_NEVER
    )
endif()

include(InstallRequiredSystemLibraries)

