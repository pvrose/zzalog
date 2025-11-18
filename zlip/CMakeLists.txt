cmake_minimum_required(VERSION 3.16)

project(zlip)

set (TARGET zlip)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VENDOR "GM3ZZA")
set(PROGRAMID "ZZALOG")

set(CPPFILES 
  src/zlip.cpp
)

include(adif.cmake)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/adif.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/adif.h @ONLY)

set(HPPFILES
  include/zlip.h
  include/adif.h
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
  ./include
  ${FLTK_INCLUDE_DIR}
)
# Add NLohmann/json
if (MSVC)
  target_include_directories(${TARGET} PRIVATE 
    "C:\\Users\\pvros\\source\\repos\\json\\include"
  )
endif()
# Add QBS_logo.h to ensure it gets generated
target_sources(${TARGET} 
  PRIVATE 
    ${CPPFILES}
  PUBLIC 
    FILE_SET zlip_headers
    TYPE HEADERS
    BASE_DIRS ./include
    FILES ${HPPFILES}
)

install(TARGETS ${TARGET})

