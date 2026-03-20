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
    ## Required DLLs need to be copied directory with executable.
      set(HAMLIB_DLLS
        "${HAMLIB_ROOT}/bin/libhamlib-4.dll"
        "${HAMLIB_ROOT}/bin/libusb-1.0.dll"
        "${HAMLIB_ROOT}/bin/libwinpthread-1.dll"
        "${HAMLIB_ROOT}/bin/libgcc_s_seh-1.dll"
      )
      add_custom_target(hamlib_dll ALL)
      foreach(F ${HAMLIB_DLLS})
          add_custom_command(TARGET hamlib_dll POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${F} ${ZZALOG_RUN_DIR}
            COMMENT "Copying ${F} to ${ZZALOG_RUN_DIR}"
          )
      endforeach()
    endif()  
    set(HAMLIB_INCLUDE_DIR "${HAMLIB_INCLUDE_DIR}" PARENT_SCOPE)
    set(HAMLIB_LIBRARY "${HAMLIB_LIBRARY}" PARENT_SCOPE)
    set(HAMLIB_DLLS "${HAMLIB_DLLS}" PARENT_SCOPE)
endfunction()

# Copy hamlib DLLs to the install directory.
function(install_hamlib_dlls)
   install(FILES ${HAMLIB_DLLS} 
    DESTINATION bin
    COMPONENT applications
    )
endfunction()