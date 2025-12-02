cmake_minimum_required(VERSION 3.16)

project(zzalog)

set(VENDOR "GM3ZZA")
set(PROGRAMID "ZZALOG")

set(TARGET zzalog_all)

string(TIMESTAMP BUILD_TIMESTAMP UTC)
set(VERSION_STRING "3.6.11+")

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
find_package(CURL CONFIG REQUIRED)
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
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libhamlib-4.dll" ${CMAKE_CURRENT_BINARY_DIR}/zzalog/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libusb-1.0.dll" ${CMAKE_CURRENT_BINARY_DIR}/zzalog/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libwinpthread-1.dll" ${CMAKE_CURRENT_BINARY_DIR}/zzalog/.
  )
  add_custom_command(TARGET hamlib_dll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "C:\\Program Files\\hamlib-w64-4.6.5\\bin\\libgcc_s_seh-1.dll" ${CMAKE_CURRENT_BINARY_DIR}/zzalog/.
  )


endif()  


add_subdirectory(common)
add_subdirectory(contests)
add_subdirectory(pugixml)
add_subdirectory(widgets)
add_subdirectory(zzalog)

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

# Add dummy target
add_custom_target(${TARGET})
add_dependencies(${TARGET} zzalog)

# copy reference data
file(COPY ${DATAFILES}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# generate docus
option(ZZALOG_USERGUIDE "Generate Userguide" ON)
option(ZZALOG_USERGUIDE_PDF "Generate PDF" ON)
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
    install(TARGETS zzalog)
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


