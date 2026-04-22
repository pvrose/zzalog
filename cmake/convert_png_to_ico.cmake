# convert_png_to_ico.cmake
# CMake script to convert PNG to ICO format on Windows
# Usage: cmake -DPNG_FILE=input.png -DICO_FILE=output.ico -P convert_png_to_ico.cmake

if(NOT PNG_FILE OR NOT ICO_FILE)
    message(FATAL_ERROR "PNG_FILE and ICO_FILE must be specified")
endif()

if(NOT EXISTS "${PNG_FILE}")
    message(FATAL_ERROR "PNG file not found: ${PNG_FILE}")
endif()

message(STATUS "Converting ${PNG_FILE} to ${ICO_FILE}")

# Use PowerShell to convert PNG to ICO
execute_process(
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "
        Add-Type -AssemblyName System.Drawing
        $png = [System.Drawing.Image]::FromFile('${PNG_FILE}')
        $icon = [System.Drawing.Icon]::FromHandle($png.GetHicon())
        $stream = [System.IO.FileStream]::new('${ICO_FILE}', [System.IO.FileMode]::Create)
        $icon.Save($stream)
        $stream.Close()
        $png.Dispose()
    "
    RESULT_VARIABLE RESULT
    ERROR_VARIABLE ERROR_OUTPUT
)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to convert PNG to ICO: ${ERROR_OUTPUT}")
endif()

message(STATUS "Successfully created ${ICO_FILE}")
