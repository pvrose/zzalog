cmake_minimum_required(VERSION 3.16)

project(zzalog)

set(TARGET zzalog)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# All the source .cpp files
set(CPPFILES 
  src/about_dialog.cpp
  src/adi_reader.cpp
  src/adi_writer.cpp
  src/adx_handler.cpp
  src/band_data.cpp
  src/band_editor.cpp
  src/band_widget.cpp
  src/band_window.cpp
  src/banner.cpp
  src/book.cpp
  src/calendar.cpp
  src/calendar_input.cpp
  src/calendar_table.cpp
  src/change_dialog.cpp
  src/club_handler.cpp
  src/condx_view.cpp
  src/config.cpp
  src/contest_algorithm.cpp
  src/contest_data.cpp
  src/contest_dialog.cpp
  src/contest_scorer.cpp
  src/corr_dialog.cpp
  src/cty1_reader.cpp
  src/cty2_reader.cpp
  src/cty3_reader.cpp
  src/cty_data.cpp
  src/cty_dialog.cpp
  src/cty_element.cpp
  src/dxcc_table.cpp
  src/dxcc_view.cpp
  src/eqsl_handler.cpp
  src/extract_data.cpp
  src/field_choice.cpp
  src/fields.cpp
  src/fields_dialog.cpp
  src/file_holder.cpp
  src/file_viewer.cpp
  src/fllog_emul.cpp
  src/font_dialog.cpp
  src/import_data.cpp
  src/init_dialog.cpp
  src/intl_dialog.cpp
  src/intl_widgets.cpp
  src/log_table.cpp
  src/lotw_handler.cpp
  src/main.cpp
  src/main_window.cpp
  src/menu.cpp
  src/page_dialog.cpp
  src/password_input.cpp
  src/png_writer.cpp
  src/printer.cpp
  src/qrz_handler.cpp
  src/qsl_dataset.cpp
  src/qsl_display.cpp
  src/qsl_editor.cpp
  src/qsl_emailer.cpp
  src/qsl_image.cpp
  src/qsl_widget.cpp
  src/qso_apps.cpp
  src/qso_bands.cpp
  src/qso_buttons.cpp
  src/qso_clock.cpp
  src/qso_clocks.cpp
  src/qso_data.cpp
  src/qso_details.cpp
  src/qso_dxcc.cpp
  src/qso_entry.cpp
  src/qso_log.cpp
  src/qso_log_info.cpp
  src/qso_manager.cpp
  src/qso_misc.cpp
  src/qso_net_entry.cpp
  src/qso_operation.cpp
  src/qso_qsl.cpp
  src/qso_qsl_vwr.cpp
  src/qso_query.cpp
  src/qso_rig.cpp
  src/qso_tabbed_rigs.cpp
  src/qso_wx.cpp
  src/record.cpp
  src/record_table.cpp
  src/report_tree.cpp
  src/rig_data.cpp
  src/rig_if.cpp
  src/rpc_data_item.cpp
  src/rpc_handler.cpp
  src/search_dialog.cpp
  src/serial.cpp
  src/settings.cpp
  src/socket_server.cpp
  src/spec_data.cpp
  src/spec_tree.cpp
  src/status.cpp
  src/stn_call_dlg.cpp
  src/stn_data.cpp
  src/stn_dialog.cpp
  src/stn_oper_dlg.cpp
  src/stn_qth_dlg.cpp
  src/symbols.cpp
  src/tabbed_forms.cpp
  src/ticker.cpp
  src/toolbar.cpp
  src/url_handler.cpp
  src/user_dialog.cpp
  src/view.cpp
  src/web_dialog.cpp
  src/win_dialog.cpp
  src/wsjtx_handler.cpp
  src/wx_handler.cpp

)

set( HPPFILES
include/about_dialog.h
include/adi_reader.h
include/adi_writer.h
include/adx_handler.h
include/band_data.h
include/band_editor.h
include/band.h
include/band_widget.h
include/band_window.h
include/banner.h
include/book.h
include/calendar.h
include/calendar_input.h
include/calendar_table.h
include/change_dialog.h
include/club_handler.h
include/condx_view.h
include/config.h
include/contest_algorithm.h
include/contest_data.h
include/contest_dialog.h
include/contest_scorer.h
include/corr_dialog.h
include/cty1_reader.h
include/cty2_reader.h
include/cty3_reader.h
include/cty_data.h
include/cty_dialog.h
include/cty_element.h
include/dxcc_table.h
include/dxcc_view.h
include/eqsl_handler.h
include/extract_data.h
include/field_choice.h
include/fields_dialog.h
include/fields.h
include/file_holder.h
include/file_viewer.h
include/fllog_emul.h
include/font_dialog.h
include/icons.h
include/import_data.h
include/init_dialog.h
include/intl_dialog.h
include/intl_widgets.h
include/logo.h
include/log_table.h
include/lotw_handler.h
include/main.h
include/main_window.h
include/menu.h
include/page_dialog.h
include/password_input.h
include/png_writer.h
include/printer.h
include/qrz_handler.h
include/qsl_data.h
include/qsl_dataset.h
include/qsl_display.h
include/qsl_editor.h
include/qsl_emailer.h
include/qsl_image.h
include/qsl_widget.h
include/qso_apps.h
include/qso_bands.h
include/qso_buttons.h
include/qso_clock.h
include/qso_clocks.h
include/qso_data.h
include/qso_details.h
include/qso_dxcc.h
include/qso_entry.h
include/qso_log.h
include/qso_log_info.h
include/qso_manager.h
include/qso_misc.h
include/qso_net_entry.h
include/qso_operation.h
include/qso_qsl.h
include/qso_qsl_vwr.h
include/qso_query.h
include/qso_rig.h
include/qso_tabbed_rigs.h
include/qso_wx.h
include/record.h
include/record_table.h
include/regices.h
include/report_tree.h
include/rig_data.h
include/rig_if.h
include/rpc_data_item.h
include/rpc_handler.h
include/search_dialog.h
include/search.h
include/serial.h
include/settings.h
include/socket_server.h
include/spec_data.h
include/spec_tree.h
include/status.h
include/stn_call_dlg.h
include/stn_data.h
include/stn_dialog.h
include/stn_oper_dlg.h
include/stn_qth_dlg.h
include/symbols.h
include/tabbed_forms.h
include/ticker.h
include/timestamp.h
include/toolbar.h
include/url_handler.h
include/user_dialog.h
include/view.h
include/web_dialog.h
include/win_dialog.h
include/wsjtx_handler.h
include/wx_handler.h

)

set(CMAKE_BUILD_RPATH ${CMAKE_CURRENT_BINARY_DIR})

# Define the zzalog executable
add_executable(${TARGET} WIN32 MACOSX_BUNDLE)

target_include_directories(${TARGET} PRIVATE 
  ./include 
  ../common/include 
  ../contests/include
  ../widgets/include
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

if (MSVC)
else()
target_link_directories(${TARGET} PRIVATE
  /usr/local/lib
)
endif()

target_link_libraries(${TARGET} PRIVATE 
  zzalog_common
  zzalog_contests
  zzalog_widgets
  pugixml
  ${FLTK_LIBRARIES}
  ${CURL_LIBRARIES}
  ${HAMLIB_LIBRARY}
  
)

