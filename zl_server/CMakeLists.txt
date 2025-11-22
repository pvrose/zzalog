cmake_minimum_required(VERSION 3.16)

project(zl_server)

set (TARGET zl_server)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VENDOR "GM3ZZA")
set(PROGRAMID "ZZALOG")

set(CPPFILES 
  src/zl_server.cpp
)

set(HPPFILES
  include/zl_server.h
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
  ${CMAKE_SOURCE_DIR}/common/include
  ${CMAKE_SOURCE_DIR}/zlip/include
  ${FLTK_INCLUDE_DIR}
)
# External includes for MSVC
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
    FILE_SET zls_headers
    TYPE HEADERS
    BASE_DIRS ./include
    FILES ${HPPFILES}
)

install(TARGETS ${TARGET})

