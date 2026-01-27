# Custom FindZLIB module that handles bundled zlib in this project
# This takes precedence over CMake's standard FindZLIB

if(ZLIB_FOUND)
    return()
endif()

# Check if we already have the ZLIB::ZLIB target (from bundled zlib)
if(TARGET ZLIB::ZLIB)
    set(ZLIB_FOUND TRUE)
    # Ensure all required variables are set
    if(NOT DEFINED ZLIB_INCLUDE_DIR)
        get_target_property(_zlib_inc ZLIB::ZLIB INTERFACE_INCLUDE_DIRECTORIES)
        if(_zlib_inc)
            set(ZLIB_INCLUDE_DIR "${_zlib_inc}")
            set(ZLIB_INCLUDE_DIRS "${ZLIB_INCLUDE_DIR}")
        endif()
    endif()
    if(NOT DEFINED ZLIB_LIBRARY)
        set(ZLIB_LIBRARY "ZLIB::ZLIB")
    endif()
    if(NOT DEFINED ZLIB_LIBRARIES)
        set(ZLIB_LIBRARIES "ZLIB::ZLIB")
    endif()
    return()
endif()

message(FATAL_ERROR "Bundled ZLIB target not found. Ensure zlib is included via add_subdirectory(zlib) before find_package(ZLIB).")
