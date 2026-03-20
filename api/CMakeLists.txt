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
  set(DOXY_API_INDEX_FILE index.html)
  set(DOXY_API_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXY_API_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  # Set source files
  set(DOXY_API_FILES 
    ${ZZALOG_CONTESTS_HPPFILES}
    ${ZZALOG_HPPFILES}
  )

  # Copy Doxyfile
  configure_file(${DOXY_API_IN} ${DOXY_API_OUT} @ONLY)
 
  # Command for generating dox from config file
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
    DEPENDS ${DOXY_API_FILES} ${DOXY_API_OUT}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_API_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "ZZALOG: Generating API documentation"
    VERBATIM
  )
  add_custom_target(api_html ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
  )

endif()
