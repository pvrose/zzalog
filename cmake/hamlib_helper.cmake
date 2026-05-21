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

# CMake helper function for hamlib integration.
# It augments find_package(HAMLIB) with additional handling for MSVC.
function (find_hamlib)
    find_package(HAMLIB)
    if (NOT HAMLIB_FOUND AND MSVC)
      message(STATUS "HAMLIB not found")
    ## Externally set hamlib install directory: find_package(HAMLIB) not part of installation
      if (NOT HAMLIB_ROOT)
        message(FATAL_ERROR "HAMLIB_ROOT not set, set it to the hamlib installation directory.")
      else()
        message(STATUS "Using HAMLIB_ROOT ${HAMLIB_ROOT}")
      endif()
      set(HAMLIB_INCLUDE_DIR "${HAMLIB_ROOT}/include")
      set(HAMLIB_LIBRARY "${HAMLIB_ROOT}/lib/msvc/libhamlib-4.lib")
    ## Required DLLs to be copied to target directory
      set(HAMLIB_DLLS
        "${HAMLIB_ROOT}/bin/libhamlib-4.dll"
        "${HAMLIB_ROOT}/bin/libusb-1.0.dll"
        "${HAMLIB_ROOT}/bin/libwinpthread-1.dll"
        "${HAMLIB_ROOT}/bin/libgcc_s_seh-1.dll"
      )
    endif()  
    set(HAMLIB_INCLUDE_DIR "${HAMLIB_INCLUDE_DIR}" PARENT_SCOPE)
    set(HAMLIB_LIBRARY "${HAMLIB_LIBRARY}" PARENT_SCOPE)
    set(HAMLIB_DLLS "${HAMLIB_DLLS}" PARENT_SCOPE)
endfunction()

# Copy hamlib DLLs to target output directory during build.
# Usage: copy_hamlib_dlls(target_name)
function(copy_hamlib_dlls TARGET_NAME)
    if(NOT MSVC)
        return()
    endif()

    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target ${TARGET_NAME} does not exist")
        return()
    endif()

    if(HAMLIB_DLLS)
        foreach(DLL_FILE ${HAMLIB_DLLS})
            if(EXISTS "${DLL_FILE}")
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${DLL_FILE}"
                        "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                    COMMENT "Copying hamlib DLL: ${DLL_FILE}"
                )
            endif()
        endforeach()
        message(STATUS "hamlib: Will copy DLLs to ${TARGET_NAME} output directory")
    else()
        message(WARNING "hamlib: No DLL paths available")
    endif()
endfunction()

# Copy hamlib DLLs to the install directory.
function(install_hamlib_dlls)
   install(FILES ${HAMLIB_DLLS} 
    DESTINATION bin
    COMPONENT applications
    )
endfunction()