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

project(ZZALOG_CONTESTS)
set(ZZALOG_CONTESTS_INCLUDE_DIR ${ZZALOG_CONTESTS_SOURCE_DIR}/include)
set(ZZALOG_CONTESTS_INCLUDE_DIR ${ZZALOG_CONTESTS_INCLUDE_DIR} PARENT_SCOPE)

set (TARGET zzalog_contests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VENDOR "GM3ZZA")
set(PROGRAMID "ZZALOG")

## Set home directory per OS.
if(MSVC)
  set(USER_HOME_DIR $ENV{USERPROFILE})
else()
  set(USER_HOME_DIR $ENV{HOME})
endif()

set(ZZALOG_CONTESTS_CPPFILES 
  ${ZZALOG_CONTESTS_SOURCE_DIR}/src/basic.cpp
  ${ZZALOG_CONTESTS_SOURCE_DIR}/src/iaru_hf.cpp
)

set(ZZALOG_CONTESTS_HPPFILES
  ${ZZALOG_CONTESTS_INCLUDE_DIR}/basic.h
  ${ZZALOG_CONTESTS_INCLUDE_DIR}/iaru_hf.h
)
set(ZZALOG_CONTESTS_HPPFILES ${ZZALOG_CONTESTS_HPPFILES} PARENT_SCOPE)

# set document directory
if(MSVC)
  set(CMAKE_INSTALL_RPATH C:/ProgramData/${VENDOR}/${PROGRAMID})
else()
  set(CMAKE_INSTALL_RPATH /etc/${VENDOR}/${PROGRAMID})
endif()

set(CMAKE_MODULE_PATH ../)

set(CMAKE_BUILD_RPATH ${CMAKE_CURRENT_BINARY_DIR})
add_library(${TARGET})
target_include_directories(${TARGET} PRIVATE 
  ${ZZALOG_CONTESTS_INCLUDE_DIR}
  ${ZZALOG_INCLUDE_DIR}
  ${ZZACOMMON_INCLUDE_DIR}
  ${FLTK_INCLUDE_DIR}
)
if (MSVC)
  target_include_directories(${TARGET} PRIVATE 
    "${USER_HOME_DIR}\\source\\repos\\json\\include"
  )
endif()
# Component sources
target_sources(${TARGET} 
  PRIVATE 
    ${ZZALOG_CONTESTS_CPPFILES}
  PUBLIC 
    FILE_SET contests_headers
    TYPE HEADERS
    BASE_DIRS ./include
    FILES ${ZZALOG_CONTESTS_HPPFILES}
)

if (BUILD_SHARED_OBJECTS)
install(TARGETS ${TARGET})
endif()
