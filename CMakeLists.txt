cmake_minimum_required(VERSION 3.16)

project(zzalog)

set(VENDOR "GM3ZZA")
set(PROGRAMID "ZZALOG")

set(TARGET zzalog)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# set document directory
if(MSVC)
  set(CMAKE_INSTALL_RPATH C:/ProgramData/${VENDOR}/${PROGRAMID})
else()
  set(CMAKE_INSTALL_RPATH /etc/${VENDOR}/${PROGRAMID})
endif()

# ser compiler options
if (MSVC)
#  add_compile_options(/MD)
else()
  add_compile_options(-g)
endif()

#FLTK - work-round for MSVC 
if (0)
  set(FLTK_INCLUDE_DIR "C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\include")
  set(FLTK_LIBRARIES "C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_images.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_jpeg.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_png.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_z.lib;gdiplus.lib;Ws2_32.lib;User32.lib;gdi32.lib;comctl32.lib")
else()
 set(FLTK_DIR "C:\\Users\\pvros\\source\\repos\\fltk\\build")
   set(CMAKE_MODULE_PATH ../ ./)
    find_package(FLTK CONFIG REQUIRED)
    if (FLTK_FOUND)
      message(STATUS "FLTK found: ${FLTK_VERSION}")
      message(STATUS "FLTK include dir: ${FLTK_INCLUDE_DIR}")
      message(STATUS "FLTK libraries: ${FLTK_LIBRARIES}")
    else()
      message(FATAL_ERROR "FLTK not found")
    endif()
endif()

# libcurl
if(MSVC)
find_package(CURL CONFIG REQUIRED)
else()
find_package(CURL REQUIRED)
endif()
if (MSVC)
  set(CURL_INCLUDE_DIR "C:\\Users\\pvros\\source\\repos\\curl\\include")
  set(CURL_LIBRARIES CURL::libcurl)
endif()

# libhamlib
find_package(Hamlib)
if (NOT HAMLIB_FOUND AND MSVC)
  message("Hamlib not found")
  set(HAMLIB_INCLUDE_DIR "C:\\Program Files\\hamlib-w64-4.6.5\\include")
  set(HAMLIB_LIBRARY "C:\\Program Files\\hamlib-w64-4.6.5\\lib\\msvc\\libhamlib-4.lib")
# Work-round for adding hamlib DLLs
  add_custom_target(hamlib_dll ALL)
  message("Using custom Hamlib settings")
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libhamlib-4.dll" ${CMAKE_BINARY_DIR}/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libusb-1.0.dll" ${CMAKE_BINARY_DIR}/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libwinpthread-1.dll" ${CMAKE_BINARY_DIR}/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libgcc_s_seh-1.dll" ${CMAKE_BINARY_DIR}/.
  )


endif()  

include(ExternalProject)

# Add zzacommon
set(ZZACOMMON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/zzacommon)
set(ZZACOMMON_BUILD_DIR ${CMAKE_BINARY_DIR}/zzacommon)
set(ZZACOMMON_INSTALL_DIR ${ZZACOMMON_BUILD_DIR}/install)
if(MSVC)
  set(ZZACOMMON_STATIC_LIB ${ZZACOMMON_INSTALL_DIR}/lib/zzacommon.lib)
else()
  set(ZZACOMMON_STATIC_LIB ${ZZACOMMON_INSTALL_DIR}/lib/libzzacommon.a)
endif()
ExternalProject_Add(zzacommon 
  GIT_REPOSITORY https://github.com/pvrose/zzacommon
#  GIT_TAG f43094e65d55c1afb08e12bd2df8a8d37dcab04e
  PREFIX ${ZZACOMMON_BUILD_DIR}
  SOURCE_DIR ${ZZACOMMON_SOURCE_DIR}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${ZZACOMMON_INSTALL_DIR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS=OFF
  BUILD_BYPRODUCTS
    ${ZZACOMMON_STATIC_LIB}
)



add_subdirectory(contests)
add_subdirectory(pugixml)

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
add_dependencies(${TARGET} zzacommon)

target_include_directories(${TARGET} PRIVATE 
  ${CMAKE_SOURCE_DIR}/include 
  ${CMAKE_SOURCE_DIR}/contests/include
  ${ZZACOMMON_SOURCE_DIR}/include
  ${FLTK_INCLUDE_DIR}
  ${HAMLIB_INCLUDE_DIR}
  ${CURL_INCLUDE_DIR}
)
# pthread
if (MSVC)
  target_include_directories(${TARGET} PRIVATE 
    "C:\\Users\\pvros\\source\\repos\\pthread-win32"
    "C:\\Users\\pvros\\source\\repos\\json\\include"
  )
endif()
# Add QBS_logo.h to ensure it gets generated
target_sources(${TARGET} PRIVATE ${CPPFILES})

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
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# generate docus
option(ZZALOG_USERGUIDE "Generate Userguide" ON)
if (MSVC)
option(ZZALOG_USERGUIDE_PDF "Generate PDF" OFF)
else()
option(ZZALOG_USERGUIDE_PDF "Generate PDF" ON)
endif()
if(ZZALOG_USERGUIDE) 
  message(STATUS "Adding userguide")
  add_subdirectory(userguide)
  add_dependencies(${TARGET} ug_html)
  if (ZZALOG_USERGUIDE_PDF)
    message(STATUS "Adding userguide PDF")
    add_dependencies(${TARGET} pdf)
  endif()
endif()
option(ZZALOG_API_GUIDE "Generate API guide" ON)
if(ZZALOG_API_GUIDE) 
  message(STATUS "Adding API documentation")
  add_subdirectory(api)
  add_dependencies(${TARGET} api_html)
endif()

## Temporary work-round until MSVC issues sorted out - do not allow install with MSVC
if (MSVC)
else()
    # install zzalog_fe
    install(TARGETS ${TARGET})
    # install reference data
    install(FILES ${DATAFILES}
        DESTINATION ${CMAKE_INSTALL_RPATH})
    # install userguide files
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
    # install API documentation files
    if(ZZALOG_API_GUIDE) 
      install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/html
        DESTINATION ${CMAKE_INSTALL_RPATH}/api
        MESSAGE_NEVER
      )
    endif()
endif(MSVC)


