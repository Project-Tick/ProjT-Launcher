cmake_minimum_required(VERSION 3.12...3.31)

if(NOT DEFINED PROJECT_NAME)
    project(PTlibzippy-contrib LANGUAGES C CXX)
endif()

function(ptlibzippy_add_contrib_lib name description dir)
    option(PTLIBZIPPY_BUILD_${name} "Enable building of ${description}" OFF)

    if(PTLIBZIPPY_BUILD_${name})
        if(NOT DEFINED PTLIBZIPPY_${name}_BUILD_SHARED)
            set(PTLIBZIPPY_${name}_BUILD_SHARED ${PTLIBZIPPY_BUILD_SHARED} CACHE BOOL "")
        endif(NOT DEFINED PTLIBZIPPY_${name}_BUILD_SHARED)

        if(NOT DEFINED PTLIBZIPPY_${name}_BUILD_STATIC)
            set(PTLIBZIPPY_${name}_BUILD_STATIC ${PTLIBZIPPY_BUILD_STATIC} CACHE BOOL "")
        endif(NOT DEFINED PTLIBZIPPY_${name}_BUILD_STATIC)

        if(NOT DEFINED PTLIBZIPPY_${name}_BUILD_TESTING)
            set(PTLIBZIPPY_${name}_BUILD_TESTING ${PTLIBZIPPY_BUILD_TESTING} CACHE BOOL "")
        endif(NOT DEFINED PTLIBZIPPY_${name}_BUILD_TESTING)

        if(NOT DEFINED PTLIBZIPPY_${name}_INSTALL)
            set(PTLIBZIPPY_${name}_INSTALL ${PTLIBZIPPY_INSTALL} CACHE BOOL "")
        endif(NOT DEFINED PTLIBZIPPY_${name}_INSTALL)

        add_subdirectory(${dir}/)
    endif(PTLIBZIPPY_BUILD_${name})
endfunction(ptlibzippy_add_contrib_lib name description dir)

function(ptlibzippy_add_contrib_feature name description dir)
    if(ARGC EQUAL 4)
        set(default_on ${ARGV3})
    else()
        set(default_on Off)
    endif()

    option(PTLIBZIPPY_WITH_${name}
           "Enable build ${description}"
           ${default_on})

    if(PTLIBZIPPY_WITH_${name})
        add_subdirectory(${dir}/)
    endif(PTLIBZIPPY_WITH_${name})
endfunction(ptlibzippy_add_contrib_feature name description dir)

ptlibzippy_add_contrib_feature("GVMAT64"
    "of an optimized longest_match for 32 bits x86_64"
    gcc_gvmat64)

ptlibzippy_add_contrib_feature(INFBACK9 "with support for method 9 deflate" infback9)
ptlibzippy_add_contrib_feature(CRC32VX "with S390X-CRC32VX implementation" crc32vx)
ptlibzippy_add_contrib_lib(ADA "Ada bindings" ada)
ptlibzippy_add_contrib_lib(BLAST "blast binary" blast)
ptlibzippy_add_contrib_lib(IOSTREAM3 "IOStream C++ bindings V3" iostream3)
ptlibzippy_add_contrib_lib(MINIZIP "minizip library" minizip)
ptlibzippy_add_contrib_lib(PUFF "puff decompress library" puff)

if(WIN32)
    ptlibzippy_add_contrib_lib(TESTZLIB "testptlibzippy binary" testptlibzippy)

    if (ZLIB_BUILD_ZLIB1_DLL)
        add_subdirectory(zlib1-dll/)
    endif (ZLIB_BUILD_ZLIB1_DLL)

    option(ZLIB_BUILD_ZLIB1_DLL "Build the legacy zlib + minizip DLL" OFF)
endif(WIN32)
