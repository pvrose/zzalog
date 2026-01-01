cmake_minimum_required(VERSION 3.16)

project(ZZALOG)

## Set program details
set(VENDOR "GM3ZZA")
set(PROGRAMID "ZZALOG")

## Set target executable
set(TARGET zzalog)

## Set C++ requirements.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Set home directory per OS.
if(MSVC)
  set(USER_HOME_DIR $ENV{USERPROFILE})
else()
  set(USER_HOME_DIR $ENV{HOME})
endif()

# Set timestamp and version into files (requires to be rebuilt to take effect)
string(TIMESTAMP BUILD_TIMESTAMP UTC)
set(VERSION_STRING "3.6.11+")
configure_file(
  ${CMAKE_SOURCE_DIR}/include/timestamp.h.in
  ${CMAKE_SOURCE_DIR}/include/timestamp.h
  @ONLY
)

# All the source .cpp files
set(CPPFILES 
  ${CMAKE_SOURCE_DIR}/src/about_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/adi_reader.cpp
  ${CMAKE_SOURCE_DIR}/src/adi_writer.cpp
  ${CMAKE_SOURCE_DIR}/src/adx_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/band_data.cpp
  ${CMAKE_SOURCE_DIR}/src/band_editor.cpp
  ${CMAKE_SOURCE_DIR}/src/band_widget.cpp
  ${CMAKE_SOURCE_DIR}/src/band_window.cpp
  ${CMAKE_SOURCE_DIR}/src/book.cpp
  ${CMAKE_SOURCE_DIR}/src/change_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/club_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/condx_view.cpp
  ${CMAKE_SOURCE_DIR}/src/config.cpp
  ${CMAKE_SOURCE_DIR}/src/contest_algorithm.cpp
  ${CMAKE_SOURCE_DIR}/src/contest_data.cpp
  ${CMAKE_SOURCE_DIR}/src/contest_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/contest_scorer.cpp
  ${CMAKE_SOURCE_DIR}/src/corr_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/cty1_reader.cpp
  ${CMAKE_SOURCE_DIR}/src/cty2_reader.cpp
  ${CMAKE_SOURCE_DIR}/src/cty3_reader.cpp
  ${CMAKE_SOURCE_DIR}/src/cty_data.cpp
  ${CMAKE_SOURCE_DIR}/src/cty_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/cty_element.cpp
  ${CMAKE_SOURCE_DIR}/src/dxcc_table.cpp
  ${CMAKE_SOURCE_DIR}/src/dxcc_view.cpp
  ${CMAKE_SOURCE_DIR}/src/eqsl_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/extract_data.cpp
  ${CMAKE_SOURCE_DIR}/src/field_choice.cpp
  ${CMAKE_SOURCE_DIR}/src/fields.cpp
  ${CMAKE_SOURCE_DIR}/src/fields_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/file_viewer.cpp
  ${CMAKE_SOURCE_DIR}/src/fllog_emul.cpp
  ${CMAKE_SOURCE_DIR}/src/font_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/import_data.cpp
  ${CMAKE_SOURCE_DIR}/src/init_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/intl_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/intl_widgets.cpp
  ${CMAKE_SOURCE_DIR}/src/log_table.cpp
  ${CMAKE_SOURCE_DIR}/src/lotw_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/main_window.cpp
  ${CMAKE_SOURCE_DIR}/src/menu_bar.cpp
  ${CMAKE_SOURCE_DIR}/src/page_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/png_writer.cpp
  ${CMAKE_SOURCE_DIR}/src/printer.cpp
  ${CMAKE_SOURCE_DIR}/src/qrz_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/qsl_dataset.cpp
  ${CMAKE_SOURCE_DIR}/src/qsl_display.cpp
  ${CMAKE_SOURCE_DIR}/src/qsl_editor.cpp
  ${CMAKE_SOURCE_DIR}/src/qsl_emailer.cpp
  ${CMAKE_SOURCE_DIR}/src/qsl_image.cpp
  ${CMAKE_SOURCE_DIR}/src/qsl_widget.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_apps.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_bands.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_buttons.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_clock.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_clocks.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_data.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_details.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_dxcc.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_entry.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_log.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_log_info.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_manager.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_misc.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_net_entry.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_operation.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_qsl.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_qsl_vwr.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_query.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_rig.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_tabbed_rigs.cpp
  ${CMAKE_SOURCE_DIR}/src/qso_wx.cpp
  ${CMAKE_SOURCE_DIR}/src/record.cpp
  ${CMAKE_SOURCE_DIR}/src/record_table.cpp
  ${CMAKE_SOURCE_DIR}/src/report_tree.cpp
  ${CMAKE_SOURCE_DIR}/src/rig_data.cpp
  ${CMAKE_SOURCE_DIR}/src/rig_if.cpp
  ${CMAKE_SOURCE_DIR}/src/rpc_data_item.cpp
  ${CMAKE_SOURCE_DIR}/src/rpc_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/search_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/serial.cpp
  ${CMAKE_SOURCE_DIR}/src/socket_server.cpp
  ${CMAKE_SOURCE_DIR}/src/spec_data.cpp
  ${CMAKE_SOURCE_DIR}/src/spec_tree.cpp
  ${CMAKE_SOURCE_DIR}/src/stn_call_dlg.cpp
  ${CMAKE_SOURCE_DIR}/src/stn_data.cpp
  ${CMAKE_SOURCE_DIR}/src/stn_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/stn_oper_dlg.cpp
  ${CMAKE_SOURCE_DIR}/src/stn_qth_dlg.cpp
  ${CMAKE_SOURCE_DIR}/src/tabbed_forms.cpp
  ${CMAKE_SOURCE_DIR}/src/toolbar.cpp
  ${CMAKE_SOURCE_DIR}/src/url_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/user_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/view.cpp
  ${CMAKE_SOURCE_DIR}/src/web_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/win_dialog.cpp
  ${CMAKE_SOURCE_DIR}/src/wsjtx_handler.cpp
  ${CMAKE_SOURCE_DIR}/src/wx_handler.cpp

)

## All the header files - required for API documentation.
set( HPPFILES
  ${CMAKE_SOURCE_DIR}/include/about_dialog.h
  ${CMAKE_SOURCE_DIR}/include/adi_reader.h
  ${CMAKE_SOURCE_DIR}/include/adi_writer.h
  ${CMAKE_SOURCE_DIR}/include/adx_handler.h
  ${CMAKE_SOURCE_DIR}/include/band_data.h
  ${CMAKE_SOURCE_DIR}/include/band_editor.h
  ${CMAKE_SOURCE_DIR}/include/band.h
  ${CMAKE_SOURCE_DIR}/include/band_widget.h
  ${CMAKE_SOURCE_DIR}/include/band_window.h
  ${CMAKE_SOURCE_DIR}/include/book.h
  ${CMAKE_SOURCE_DIR}/include/change_dialog.h
  ${CMAKE_SOURCE_DIR}/include/club_handler.h
  ${CMAKE_SOURCE_DIR}/include/condx_view.h
  ${CMAKE_SOURCE_DIR}/include/config.h
  ${CMAKE_SOURCE_DIR}/include/contest_algorithm.h
  ${CMAKE_SOURCE_DIR}/include/contest_data.h
  ${CMAKE_SOURCE_DIR}/include/contest_dialog.h
  ${CMAKE_SOURCE_DIR}/include/contest_scorer.h
  ${CMAKE_SOURCE_DIR}/include/corr_dialog.h
  ${CMAKE_SOURCE_DIR}/include/cty1_reader.h
  ${CMAKE_SOURCE_DIR}/include/cty2_reader.h
  ${CMAKE_SOURCE_DIR}/include/cty3_reader.h
  ${CMAKE_SOURCE_DIR}/include/cty_data.h
  ${CMAKE_SOURCE_DIR}/include/cty_dialog.h
  ${CMAKE_SOURCE_DIR}/include/cty_element.h
  ${CMAKE_SOURCE_DIR}/include/dxcc_table.h
  ${CMAKE_SOURCE_DIR}/include/dxcc_view.h
  ${CMAKE_SOURCE_DIR}/include/eqsl_handler.h
  ${CMAKE_SOURCE_DIR}/include/extract_data.h
  ${CMAKE_SOURCE_DIR}/include/field_choice.h
  ${CMAKE_SOURCE_DIR}/include/fields_dialog.h
  ${CMAKE_SOURCE_DIR}/include/fields.h
  ${CMAKE_SOURCE_DIR}/include/file_viewer.h
  ${CMAKE_SOURCE_DIR}/include/fllog_emul.h
  ${CMAKE_SOURCE_DIR}/include/font_dialog.h
  ${CMAKE_SOURCE_DIR}/include/icons.h
  ${CMAKE_SOURCE_DIR}/include/import_data.h
  ${CMAKE_SOURCE_DIR}/include/init_dialog.h
  ${CMAKE_SOURCE_DIR}/include/intl_dialog.h
  ${CMAKE_SOURCE_DIR}/include/intl_widgets.h
  ${CMAKE_SOURCE_DIR}/include/log_table.h
  ${CMAKE_SOURCE_DIR}/include/lotw_handler.h
  ${CMAKE_SOURCE_DIR}/include/main.h
  ${CMAKE_SOURCE_DIR}/include/main_window.h
  ${CMAKE_SOURCE_DIR}/include/menu_bar.h
  ${CMAKE_SOURCE_DIR}/include/objects.h
  ${CMAKE_SOURCE_DIR}/include/page_dialog.h
  ${CMAKE_SOURCE_DIR}/include/png_writer.h
  ${CMAKE_SOURCE_DIR}/include/printer.h
  ${CMAKE_SOURCE_DIR}/include/qrz_handler.h
  ${CMAKE_SOURCE_DIR}/include/qsl_data.h
  ${CMAKE_SOURCE_DIR}/include/qsl_dataset.h
  ${CMAKE_SOURCE_DIR}/include/qsl_display.h
  ${CMAKE_SOURCE_DIR}/include/qsl_editor.h
  ${CMAKE_SOURCE_DIR}/include/qsl_emailer.h
  ${CMAKE_SOURCE_DIR}/include/qsl_image.h
  ${CMAKE_SOURCE_DIR}/include/qsl_widget.h
  ${CMAKE_SOURCE_DIR}/include/qso_apps.h
  ${CMAKE_SOURCE_DIR}/include/qso_bands.h
  ${CMAKE_SOURCE_DIR}/include/qso_buttons.h
  ${CMAKE_SOURCE_DIR}/include/qso_clock.h
  ${CMAKE_SOURCE_DIR}/include/qso_clocks.h
  ${CMAKE_SOURCE_DIR}/include/qso_data.h
  ${CMAKE_SOURCE_DIR}/include/qso_details.h
  ${CMAKE_SOURCE_DIR}/include/qso_dxcc.h
  ${CMAKE_SOURCE_DIR}/include/qso_entry.h
  ${CMAKE_SOURCE_DIR}/include/qso_log.h
  ${CMAKE_SOURCE_DIR}/include/qso_log_info.h
  ${CMAKE_SOURCE_DIR}/include/qso_manager.h
  ${CMAKE_SOURCE_DIR}/include/qso_misc.h
  ${CMAKE_SOURCE_DIR}/include/qso_net_entry.h
  ${CMAKE_SOURCE_DIR}/include/qso_operation.h
  ${CMAKE_SOURCE_DIR}/include/qso_qsl.h
  ${CMAKE_SOURCE_DIR}/include/qso_qsl_vwr.h
  ${CMAKE_SOURCE_DIR}/include/qso_query.h
  ${CMAKE_SOURCE_DIR}/include/qso_rig.h
  ${CMAKE_SOURCE_DIR}/include/qso_tabbed_rigs.h
  ${CMAKE_SOURCE_DIR}/include/qso_wx.h
  ${CMAKE_SOURCE_DIR}/include/record.h
  ${CMAKE_SOURCE_DIR}/include/record_table.h
  ${CMAKE_SOURCE_DIR}/include/regices.h
  ${CMAKE_SOURCE_DIR}/include/report_tree.h
  ${CMAKE_SOURCE_DIR}/include/rig_data.h
  ${CMAKE_SOURCE_DIR}/include/rig_if.h
  ${CMAKE_SOURCE_DIR}/include/rpc_data_item.h
  ${CMAKE_SOURCE_DIR}/include/rpc_handler.h
  ${CMAKE_SOURCE_DIR}/include/search_dialog.h
  ${CMAKE_SOURCE_DIR}/include/search.h
  ${CMAKE_SOURCE_DIR}/include/serial.h
  ${CMAKE_SOURCE_DIR}/include/socket_server.h
  ${CMAKE_SOURCE_DIR}/include/spec_data.h
  ${CMAKE_SOURCE_DIR}/include/spec_tree.h
  ${CMAKE_SOURCE_DIR}/include/stn_call_dlg.h
  ${CMAKE_SOURCE_DIR}/include/stn_data.h
  ${CMAKE_SOURCE_DIR}/include/stn_dialog.h
  ${CMAKE_SOURCE_DIR}/include/stn_oper_dlg.h
  ${CMAKE_SOURCE_DIR}/include/stn_qth_dlg.h
  ${CMAKE_SOURCE_DIR}/include/tabbed_forms.h
  ${CMAKE_SOURCE_DIR}/include/timestamp.h
  ${CMAKE_SOURCE_DIR}/include/toolbar.h
  ${CMAKE_SOURCE_DIR}/include/url_handler.h
  ${CMAKE_SOURCE_DIR}/include/user_dialog.h
  ${CMAKE_SOURCE_DIR}/include/view.h
  ${CMAKE_SOURCE_DIR}/include/web_dialog.h
  ${CMAKE_SOURCE_DIR}/include/win_dialog.h
  ${CMAKE_SOURCE_DIR}/include/wsjtx_handler.h
  ${CMAKE_SOURCE_DIR}/include/wx_handler.h
)

# set directory to install executable, reference data and documents.
if(MSVC)
  set(CMAKE_INSTALL_RPATH C:/ProgramData/${VENDOR}/${PROGRAMID})
else()
  set(CMAKE_INSTALL_RPATH /etc/${VENDOR}/${PROGRAMID})
endif()

# Set target directory for .dlls and development-mode data
if (MSVC)
  set (ZZALOG_RUN_DIR ${ZZALOG_BINARY_DIR}/${CMAKE_BUILD_TYPE})
else()
  set (XXALOG_RUN_DIR ${ZZALOG_BINARY_DIR})
endif()

# set compiler options
if (MSVC)
  add_compile_options(/FS)
else()
  add_compile_options(-g)
endif()


# FLTK - 
# TODO: Should this be an ExternalProject?
# MSVC work-round as FLTK built in user.
if (MSVC AND NOT FLTK_DIR)
  set(FLTK_DIR "${USER_HOME_DIR}\\source\\repos\\fltk\\build")
endif()
# Find FLTK.
set(CMAKE_MODULE_PATH ../ ./)
find_package(FLTK CONFIG REQUIRED)
if (FLTK_FOUND)
  message(STATUS "FLTK found: ${FLTK_VERSION}")
  message(STATUS "FLTK include dir: ${FLTK_INCLUDE_DIR}")
  message(STATUS "FLTK libraries: ${FLTK_LIBRARIES}")
else()
  message(FATAL_ERROR "FLTK not found")
endif()

# libcurl
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};"${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
if(MSVC)
  message(STATUS "Looking for CURL at ${CURL_DIR}")
  find_package(CURL CONFIG REQUIRED)
else()
  find_package(CURL REQUIRED)
endif()
if (CURL_FOUND)
  # CURL under vcpkg on Windows does not set CURL_INCLUDE_DIR.
  if (NOT CURL_INCLUDE_DIR) 
    if (EXISTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/curl")
      set(CURL_INCLUDE_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/")
    endif()
  endif()
  message(STATUS "CURL found: ${CURL_VERSION}")
  message(STATUS "CURL include dir: ${CURL_INCLUDE_DIR}")
  message(STATUS "CURL libraries: ${CURL_LIBRARIES}")
else()
  message(STATUS "CURL not found")
endif()

# libhamlib
find_package(Hamlib)
if (NOT HAMLIB_FOUND AND MSVC)
  message(STATUS "Hamlib not found")
## Externally set hamlib install directory: find_package(Hamlib) not part of installation
  if (NOT HAMLIB_INSTALL_DIR)
    set(HAMLIB_INSTALL_DIR "C:\\Program Files\\hamlib-w64-4.6.5")
  endif()
  set(HAMLIB_INCLUDE_DIR "${HAMLIB_INSTALL_DIR}\\include")
  set(HAMLIB_LIBRARY "${HAMLIB_INSTALL_DIR}\\lib\\msvc\\libhamlib-4.lib")
## Required DLLs need to be copied directory with executable.
  add_custom_target(hamlib_dll ALL)
  message(STATUS "Using custom Hamlib settings - copying .dlls to ${ZZALOG_RUN_DIR}")
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${HAMLIB_INSTALL_DIR}\\bin\\libhamlib-4.dll" ${ZZALOG_RUN_DIR}/.
  COMMENT "Copying libhamlib-4.dll and others to ${ZZALOG_RUN_DIR}"
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${HAMLIB_INSTALL_DIR}\\bin\\libusb-1.0.dll" ${ZZALOG_RUN_DIR}/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${HAMLIB_INSTALL_DIR}\\bin\\libwinpthread-1.dll" ${ZZALOG_RUN_DIR}/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${HAMLIB_INSTALL_DIR}\\bin\\libgcc_s_seh-1.dll" ${ZZALOG_RUN_DIR}/.
  )
endif()  

## Needed for all external projects
include(ExternalProject)

# Add zzacommon as an ExternalProject
## Set the directories to build zzacommon in.
set(ZZACOMMON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/zzacommon)
set(ZZACOMMON_BUILD_DIR ${CMAKE_BINARY_DIR}/zzacommon)
set(ZZACOMMON_INSTALL_DIR ${ZZACOMMON_BUILD_DIR}/install)
if(MSVC)
  set(ZZACOMMON_STATIC_LIB ${ZZACOMMON_INSTALL_DIR}/lib/zzacommon.lib)
else()
  set(ZZACOMMON_STATIC_LIB ${ZZACOMMON_INSTALL_DIR}/lib/libzzacommon.a)
endif()
## Clone git repository (under source directory)
ExternalProject_Add(zzacommon 
  GIT_REPOSITORY https://github.com/pvrose/zzacommon
  GIT_TAG 9da82e0d471d4b7767a6a2eba72c9b2ccf469d46 # Release_1.0.0
  PREFIX ${ZZACOMMON_BUILD_DIR}
  SOURCE_DIR ${ZZACOMMON_SOURCE_DIR}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${ZZACOMMON_INSTALL_DIR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS=OFF
  BUILD_BYPRODUCTS
    ${ZZACOMMON_STATIC_LIB}
)

## Subdirectories as part of ZZALOG
add_subdirectory(contests)
# TODO Make External Project?
add_subdirectory(pugixml)

## Reference data
set(DATAFILES
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/all.json
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/band_plan.json
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/cty.csv
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/cty.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/Prefix.lst
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/intl_chars.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/google-maps.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/pdf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/qrz_1.jpg
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/rose.png
)

# Define the zzalog executable
add_executable(${TARGET} WIN32)
# Ensure external projects are built before linking zzalog
add_dependencies(${TARGET} zzacommon)

## Set header include directoies.
target_include_directories(${TARGET} PRIVATE 
  ${CMAKE_SOURCE_DIR}/include 
  ${CMAKE_SOURCE_DIR}/contests/include
  ${ZZACOMMON_SOURCE_DIR}/include
  ${FLTK_INCLUDE_DIR}
  ${HAMLIB_INCLUDE_DIR}
  ${CURL_INCLUDE_DIR}
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
target_sources(${TARGET} PRIVATE ${CPPFILES})

## Define libraries to be linked in.
target_link_libraries(${TARGET} PRIVATE 
  ${FLTK_LIBRARIES}
  ${CURL_LIBRARIES}
  ${HAMLIB_LIBRARY}
  zzalog_contests
  pugixml
  ${ZZACOMMON_STATIC_LIB}
)

# copy reference data
file(COPY ${DATAFILES}
  DESTINATION ${ZZALOG_RUN_DIR})

# generate documents - add switches for conditional build.
option(ZZALOG_USERGUIDE "Generate Userguide" ON)
if (MSVC)
  option(ZZALOG_USERGUIDE_PDF "Generate PDF" OFF)
else()
  option(ZZALOG_USERGUIDE_PDF "Generate PDF" ON)
endif()
# Generate documents - generate userguide.
if(ZZALOG_USERGUIDE) 
  message(STATUS "Adding userguide")
  add_subdirectory(userguide)
  add_dependencies(${TARGET} ug_html)
  if (ZZALOG_USERGUIDE_PDF)
    message(STATUS "Adding userguide PDF")
    add_dependencies(${TARGET} pdf)
  endif()
endif()
# Switch to generate API documentation.
option(ZZALOG_API_GUIDE "Generate API guide" ON)
# Generate API documentation.
if(ZZALOG_API_GUIDE) 
  message(STATUS "Adding API documentation")
  add_subdirectory(api)
  add_dependencies(${TARGET} api_html)
endif()

## Temporary work-round until MSVC issues sorted out - do not allow install with MSVC
if (MSVC)
else()
    # install executable
    install(TARGETS ${TARGET})
    # install reference data
    install(FILES ${DATAFILES}
        DESTINATION ${CMAKE_INSTALL_RPATH})
    # install userguide HTML files and PDF
    if(ZZALOG_USERGUIDE)
      install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/userguide/html
        DESTINATION ${CMAKE_INSTALL_RPATH}/userguide
        MESSAGE_NEVER
      )
      if (ZZALOG_USERGUIDE_PDF)
        install(
          FILES ${CMAKE_CURRENT_BINARY_DIR}/userguide/${PROGRAMID}.pdf
          DESTINATION ${CMAKE_INSTALL_RPATH}/userguide
        )
      endif()
    endif()
    # install API documentation HTML filesn
    if(ZZALOG_API_GUIDE) 
      install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/html
        DESTINATION ${CMAKE_INSTALL_RPATH}/api
        MESSAGE_NEVER
      )
    endif()
endif(MSVC)


