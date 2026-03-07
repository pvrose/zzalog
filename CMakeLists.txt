cmake_minimum_required(VERSION 3.16)

project(ZZALOG VERSION 3.6.15)
set(ZZALOG_SRC_DIR ${ZZALOG_SOURCE_DIR}/src)
set(ZZALOG_INCLUDE_DIR ${ZZALOG_SOURCE_DIR}/include)
set(ZZALOG_RC_VALUE "-rc1")

## Set program details - used by zzacommon
set(APP_VENDOR "GM3ZZA")
set(APP_NAME "ZZALOG")
string(TIMESTAMP APP_TIMESTAMP UTC)
set(APP_VERSION "${ZZALOG_VERSION}${ZZALOG_RC_VALUE}")
set(APP_SOURCE_DIR ${ZZALOG_SOURCE_DIR})
configure_file(
  ${ZZALOG_SOURCE_DIR}/src/ident.cpp.in
  ${ZZALOG_SOURCE_DIR}/src/ident.cpp
  @ONLY
)

## Set target executable
set(TARGET zzalog)
if (NOT MSVC)
  set(BUILD_SHARED_LIBS OFF)
endif()

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

# generate documents - add switches for conditional build.
option(ZZALOG_DOX "Generate Documentation" OFF)
option(ZZALOG_PDF "Generate PDF Document" OFF)

# All the source .cpp files
set(ZZALOG_CPPFILES 
  ${ZZALOG_SRC_DIR}/about_dialog.cpp
  ${ZZALOG_SRC_DIR}/adi_reader.cpp
  ${ZZALOG_SRC_DIR}/adi_writer.cpp
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
  ${ZZALOG_SRC_DIR}/contest_algorithm.cpp
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
  ${ZZALOG_SRC_DIR}/file_viewer.cpp
  ${ZZALOG_SRC_DIR}/fldigi_handler.cpp
  ${ZZALOG_SRC_DIR}/font_dialog.cpp
  ${ZZALOG_SRC_DIR}/ident.cpp
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
  ${ZZALOG_SRC_DIR}/rpc_data_item.cpp
  ${ZZALOG_SRC_DIR}/rpc_handler.cpp
  ${ZZALOG_SRC_DIR}/search_dialog.cpp
  ${ZZALOG_SRC_DIR}/serial.cpp
  ${ZZALOG_SRC_DIR}/socket_server.cpp
  ${ZZALOG_SRC_DIR}/spec_data.cpp
  ${ZZALOG_SRC_DIR}/spec_tree.cpp
  ${ZZALOG_SRC_DIR}/stn_call_dlg.cpp
  ${ZZALOG_SRC_DIR}/stn_data.cpp
  ${ZZALOG_SRC_DIR}/stn_dialog.cpp
  ${ZZALOG_SRC_DIR}/stn_oper_dlg.cpp
  ${ZZALOG_SRC_DIR}/stn_qth_dlg.cpp
  ${ZZALOG_SRC_DIR}/tabbed_forms.cpp
  ${ZZALOG_SRC_DIR}/toolbar.cpp
  ${ZZALOG_SRC_DIR}/url_handler.cpp
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
  ${ZZALOG_INCLUDE_DIR}/contest_algorithm.h
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
  ${ZZALOG_INCLUDE_DIR}/file_viewer.h
  ${ZZALOG_INCLUDE_DIR}/fldigi_handler.h
  ${ZZALOG_INCLUDE_DIR}/font_dialog.h
  ${ZZALOG_INCLUDE_DIR}/icons.h
  ${ZZALOG_INCLUDE_DIR}/ident.h
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
  ${ZZALOG_INCLUDE_DIR}/rpc_data_item.h
  ${ZZALOG_INCLUDE_DIR}/rpc_handler.h
  ${ZZALOG_INCLUDE_DIR}/search_dialog.h
  ${ZZALOG_INCLUDE_DIR}/search.h
  ${ZZALOG_INCLUDE_DIR}/serial.h
  ${ZZALOG_INCLUDE_DIR}/socket_server.h
  ${ZZALOG_INCLUDE_DIR}/spec_data.h
  ${ZZALOG_INCLUDE_DIR}/spec_tree.h
  ${ZZALOG_INCLUDE_DIR}/stn_call_dlg.h
  ${ZZALOG_INCLUDE_DIR}/stn_data.h
  ${ZZALOG_INCLUDE_DIR}/stn_dialog.h
  ${ZZALOG_INCLUDE_DIR}/stn_oper_dlg.h
  ${ZZALOG_INCLUDE_DIR}/stn_qth_dlg.h
  ${ZZALOG_INCLUDE_DIR}/tabbed_forms.h
  ${ZZALOG_INCLUDE_DIR}/toolbar.h
  ${ZZALOG_INCLUDE_DIR}/url_handler.h
  ${ZZALOG_INCLUDE_DIR}/user_dialog.h
  ${ZZALOG_INCLUDE_DIR}/view.h
  ${ZZALOG_INCLUDE_DIR}/web_dialog.h
  ${ZZALOG_INCLUDE_DIR}/win_dialog.h
  ${ZZALOG_INCLUDE_DIR}/wsjtx_handler.h
  ${ZZALOG_INCLUDE_DIR}/wx_handler.h
)

# set directory to install executable, reference data and documents.
if (MSVC)
  set(CMAKE_INSTALL_RPATH etc)
else()
  set(CMAKE_INSTALL_RPATH etc/${TARGET})
endif()
message(STATUS "Reference data will be copied to ${CMAKE_INSTALL_RPATH}")

# Set target directory for .dlls and development-mode data
message(STATUS "ZZALOG_BINARY_DIR = ${ZZALOG_BINARY_DIR}")
message(STATUS "VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")
#if (VCPKG_TARGET_TRIPLET)
#  set (ZZALOG_RUN_DIR ${ZZALOG_BINARY_DIR}/${VCPKG_TARGET_TRIPLET})
#else()
set (ZZALOG_RUN_DIR ${ZZALOG_BINARY_DIR})
#endif()

# set compiler options
if (MSVC)
  add_compile_options(/FS)
else()
  add_compile_options(-g)
endif()

# libcurl
if (VCPKG_TARGET_TRIPLET)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};"${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
else()
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};"${_VCPKG_INSTALLED_DIR}")
endif() 
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
set(CMAKE_MODULE_PATH ../ ./)
find_package(HAMLIB)
if (NOT HAMLIB_FOUND AND MSVC)
  message(STATUS "HAMLIB not found")
## Externally set hamlib install directory: find_package(HAMLIB) not part of installation
  if (NOT HAMLIB_INSTALL_DIR)
    set(HAMLIB_INSTALL_DIR "C:/Program Files/hamlib-w64-4.7.0")
  endif()
  set(HAMLIB_INCLUDE_DIR "${HAMLIB_INSTALL_DIR}/include")
  set(HAMLIB_LIBRARY "${HAMLIB_INSTALL_DIR}/lib/msvc/libhamlib-4.lib")
## Required DLLs need to be copied directory with executable.
  set(HAMLIB_DLLS
    "${HAMLIB_INSTALL_DIR}/bin/libhamlib-4.dll"
    "${HAMLIB_INSTALL_DIR}/bin/libusb-1.0.dll"
    "${HAMLIB_INSTALL_DIR}/bin/libwinpthread-1.dll"
    "${HAMLIB_INSTALL_DIR}/bin/libgcc_s_seh-1.dll"
  )
  add_custom_target(hamlib_dll ALL)
  foreach(F ${HAMLIB_DLLS})
      add_custom_command(TARGET hamlib_dll POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${F} ${ZZALOG_RUN_DIR}
        COMMENT "Copying ${F} to ${ZZALOG_RUN_DIR}"
      )
  endforeach()
endif()  

# Add ZZACOMMON
set(ZZACOMMON_DOX ${ZZALOG_DOX})
add_subdirectory(zzacommon)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/iso.csv
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/intl_chars.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/google-maps.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/pdf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/qrz_1.jpg
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/rose.png
  ${CMAKE_CURRENT_SOURCE_DIR}/reference/keys.json
)

# Define the zzalog executable
add_executable(${TARGET} WIN32)
# Ensure external projects are built before linking zzalog
add_dependencies(${TARGET} zzacommon)
add_dependencies(zzalog_contests zzacommon)

## Set header include directoies.
target_include_directories(${TARGET} PRIVATE 
  ${ZZALOG_INCLUDE_DIR} 
  ${ZZALOG_SOURCE_DIR}/contests/include
  ${ZZACOMMON_INCLUDE_DIR}
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
target_sources(${TARGET} PRIVATE ${ZZALOG_CPPFILES})

## Define libraries to be linked in.
target_link_libraries(${TARGET} PRIVATE 
  ${CURL_LIBRARIES}
  ${HAMLIB_LIBRARY}
  zzalog_contests
  pugixml
  ${ZZACOMMON_LIBRARIES}
)

# copy reference data
add_custom_target(reference_data ALL)
message(STATUS "Reference data: ${DATAFILES}")
#foreach(F ${DATAFILES})
  file(COPY ${DATAFILES}
    DESTINATION ${ZZALOG_RUN_DIR}
    USE_SOURCE_PERMISSIONS
  )
#  add_custom_command(TARGET reference_data POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${F} ${ZZALOG_RUN_DIR}
#    COMMENT "Copying ${F} to ${ZZALOG_RUN_DIR}"
#  )
#endforeach()

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
endif()

## Temporary work-round until MSVC issues sorted out - do not allow install with MSVC
#if (MSVC)
#else()
    # install executable
    install(TARGETS ${TARGET} RUNTIME)
    # install reference data
    install(FILES ${DATAFILES}
        DESTINATION ${CMAKE_INSTALL_RPATH})
    # install hamlib DLLs
    if (MSVC)
      install(FILES ${HAMLIB_DLLS} 
        DESTINATION bin)
    endif()
    # install userguide HTML files and PDF
    if(ZZALOG_DOX)
      install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/userguide/html
        DESTINATION ${CMAKE_INSTALL_RPATH}/userguide
        MESSAGE_NEVER
      )
      install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/userguide/${APP_NAME}.pdf
        DESTINATION ${CMAKE_INSTALL_RPATH}/userguide
      )
      # install API documentation HTML filesn
      install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/html
        DESTINATION ${CMAKE_INSTALL_RPATH}/api
        MESSAGE_NEVER
      )
      # install ZZACOMMON API documentation HTML files
      install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/zzacommon/api/html
        DESTINATION ${CMAKE_INSTALL_RPATH}/zzacommon/api
        MESSAGE_NEVER
      )
    endif()
#endif(MSVC)


