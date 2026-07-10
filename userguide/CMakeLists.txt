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
find_package(Doxygen)

if(DOXYGEN_FOUND)

  message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")

  # Set project files
  set(DOXY_UG_INDEX_FILE index.html)
  set(DOXY_UG_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXY_UG_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_ug)

  set(DOXY_UG_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adding.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/analysis.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/banner.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/basic1.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/basic2.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bumr_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/change_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cli.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/condx_view.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/contest_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/contest_scorer.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cty_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cty_tree.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dxcc_view.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fields_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/index.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/installation.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/intl_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/local_data.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_table.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main_window.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/menu.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/new_app.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/new_rig.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/operating.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qsl_editor.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qsls.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_apps.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_bands.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_buttons.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_clocks.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_details.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_dxcc.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_entry.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_log_info.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_manager.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_net_entry.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_operation.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_qsl.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_qsl_vwr.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_query.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_rig.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qso_wx.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/quick_start.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reference.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/release_notes.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/report_tree.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/search_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/searching.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stn_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbar.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/user_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/web_dialog.dox
    ${CMAKE_CURRENT_SOURCE_DIR}/new_header.tex
  )

  set(DOXY_UG_IMAGES 
    ${CMAKE_CURRENT_SOURCE_DIR}/images/adding_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/adding_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/adding_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/adding_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/analysis_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/analysis_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/analysis_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/analysis_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/analysis_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/banner_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/banner_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/banner_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/banner_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/banner_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic1_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic1_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic1_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic1_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic2_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic2_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/basic2_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/bumr_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/change_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/cli_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/condx_view_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/condx_view_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/condx_view_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/condx_view_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/condx_view_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/config_tree_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/contest_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/contest_scorer_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/contest_scorer_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/cty_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/cty_dialog_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/cty_dialog_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/cty_tree_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/dxcc_view_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/dxcc_view_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/dxcc_view_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/fields_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/index_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/init_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/init_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/init_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/init_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/init_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/init_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/intl_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/log_table_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/log_table_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/log_table_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/log_table_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/log_table_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/log_table_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/main_window_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/menu_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_apps_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_apps_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_apps_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_apps_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_10.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_11.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_12.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_8.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/new_rig_9.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsl_editor_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsl_editor_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsl_editor_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsl_editor_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsl_editor_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_10.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_8.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qsls_9.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_apps_8.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_bands_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_bands_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_bands_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_bands_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_buttons_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_clocks_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_details_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_details_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_dxcc_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_dxcc_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_dxcc_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_entry_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_entry_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_entry_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_entry_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_entry_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_log_info_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_log_info_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_manager_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_net_entry_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_operation_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_operation_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_operation_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_operation_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_vwr_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_vwr_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_vwr_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_vwr_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_qsl_vwr_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_query_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_query_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_query_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_query_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_query_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_10.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_8.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_rig_9.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/qso_wx_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/quick_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/report_tree_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/report_tree_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/report_tree_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/search_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/search_dialog_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/search_dialog_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/search_dialog_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_8.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/searching_9.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/spec_tree_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/spec_tree_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/spec_tree_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/stn_dialog_0.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/stn_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/stn_dialog_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/stn_dialog_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_set1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_set2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_set3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_set4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_set5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/toolbar_set6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/user_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_2a85bca3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_414f707a.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_4a79518e.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_557fd7e5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_5e16a7e3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_ab32828.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m112f565d.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m26e6c1cd.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m4911387.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m49d19ab4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m4c6cfb94.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m6ae6db20.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_m75dc8f06.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_mba730c9.jpg
    ${CMAKE_CURRENT_SOURCE_DIR}/images/UserGuide_html_me4bb4f5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/warm_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/warm_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/warm_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/warm_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_1.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_2.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_3.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_4.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_5.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_6.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_7.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_8.png
    ${CMAKE_CURRENT_SOURCE_DIR}/images/web_dialog_9.png
  )

  setup_ug_documentation(
      ZZALOG
      ${DOXY_UG_FILES}
      ${DOXY_UG_IMAGES}
  )

  set(ZZALOG_USERGUIDE_HTML_DIR ${CMAKE_CURRENT_BINARY_DIR}/html PARENT_SCOPE)
  set(ZZALOG_USERGUIDE_PDF_FILE ${CMAKE_CURRENT_BINARY_DIR}/ZZALOG.pdf PARENT_SCOPE)

endif()
