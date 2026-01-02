cmake_minimum_required(VERSION 3.16)

project(ZZALOG_CONTESTS)
set(ZZALOG_CONTESTS_INCLUDE_DIR ${ZZALOG_CONTESTS_SOURCE_DIR}/include)

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

# set compiler options
if (MSVC)
  add_compile_options(/FS)
else()
  add_compile_options(-g)
endif()

set(ZZALOG_CONTESTS_CPPFILES 
  ${ZZALOG_CONTESTS_SOURCE_DIR}/src/basic.cpp
  ${ZZALOG_CONTESTS_SOURCE_DIR}/src/iaru_hf.cpp
)

set(ZZALOG_CONTESTS_HPPFILES
  ${ZZALOG_CONTESTS_INCLUDE_DIR}/basic.h
  ${ZZALOG_CONTESTS_INCLUDE_DIR}/iaru_hf.h
)

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

install(TARGETS ${TARGET})

