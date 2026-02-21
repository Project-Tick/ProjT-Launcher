cmake_minimum_required(VERSION 3.12...3.31)

project(
    PTlibzippy
    LANGUAGES C
    VERSION 0.0.5.1
    HOMEPAGE_URL "https://projecttick.org/p/zlib"
    DESCRIPTION "PTlibzippy - a general-purpose lossless data-compression library")

# ============================================================================
# CPack
# ============================================================================
set(CPACK_PACKAGE_VENDOR "PTlibzippy-Project")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PTlibzippy_SOURCE_DIR}/README)
set(CPACK_RESOURCE_FILE_LICENSE ${PTlibzippy_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${PTlibzippy_SOURCE_DIR}/README)

# ============================================================================
# configuration
# ============================================================================

option(PTLIBZIPPY_BUILD_TESTING "Enable Zlib Examples as tests" ON)
option(PTLIBZIPPY_BUILD_SHARED "Enable building zlib shared library" ON)
option(PTLIBZIPPY_BUILD_STATIC "Enable building zlib static library" ON)
option(PTLIBZIPPY_INSTALL "Enable installation of zlib" ON)
option(PTLIBZIPPY_PREFIX "prefix for all types and library functions, see ptzippyconf.h.in"
       OFF)
mark_as_advanced(PTLIBZIPPY_PREFIX)

get_property(IS_MULTI GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT IS_MULTI)
    message(STATUS "No CMAKE_BUILD_TYPE set -- using Release")
    set(CMAKE_BUILD_TYPE Release)
endif(NOT DEFINED CMAKE_BUILD_TYPE AND NOT IS_MULTI)

include(CheckCSourceCompiles)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CMakePackageConfigHelpers)
include(CheckTypeSize)
include(CPack)
include(GNUInstallDirs)

set(CPACK_INCLUDED TRUE)

if(NOT PTLIBZIPPY_CONF_WRITTEN)
    set(PT_PREFIX ${PTLIBZIPPY_PREFIX})
    set(PTCONF_OUT_FILE ${PTlibzippy_BINARY_DIR}/ptzippyconf.h.cmakein)
    file(READ ${PTlibzippy_SOURCE_DIR}/ptzippyconf.h PTZIPPYCONF_CONTENT)
    string(FIND "${PTZIPPYCONF_CONTENT}" "#ifndef PTZIPPYCONF_H" PTZIPPYCONF_POS)
    if(PTZIPPYCONF_POS EQUAL -1)
        message(FATAL_ERROR "Failed to find '#ifndef PTZIPPYCONF_H' in ptzippyconf.h")
    endif()
    string(SUBSTRING "${PTZIPPYCONF_CONTENT}" 0 ${PTZIPPYCONF_POS} PTZIPPYCONF_HEAD)
    string(SUBSTRING "${PTZIPPYCONF_CONTENT}" ${PTZIPPYCONF_POS} -1 PTZIPPYCONF_TAIL)
    file(WRITE ${PTCONF_OUT_FILE} "${PTZIPPYCONF_HEAD}")
    file(APPEND ${PTCONF_OUT_FILE} "#cmakedefine PT_PREFIX 1\n")
    file(APPEND ${PTCONF_OUT_FILE} "#cmakedefine HAVE_STDARG_H 1\n")
    file(APPEND ${PTCONF_OUT_FILE} "#cmakedefine HAVE_UNISTD_H 1\n")
    file(APPEND ${PTCONF_OUT_FILE} "${PTZIPPYCONF_TAIL}")
    set(PTLIBZIPPY_CONF_WRITTEN
        TRUE
        CACHE BOOL "ptzippyconf.h.cmakein was created")
    mark_as_advanced(PTLIBZIPPY_CONF_WRITTEN)
endif(NOT PTLIBZIPPY_CONF_WRITTEN)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
check_type_size(off64_t OFF64_T)
unset(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)

#
# Check for stdarg.h
#
check_include_file(stdarg.h HAVE_STDARG_H)

#
# Check for unistd.h
#
check_include_file(unistd.h HAVE_UNISTD_H)

#
# Check visibility attribute is supported
#
if(MSVC)
    set(CMAKE_REQUIRED_FLAGS "-WX")
else(MSVC)
    set(CMAKE_REQUIRED_FLAGS "-Werror")
endif(MSVC)

check_c_source_compiles(
    "
        #include <stdlib.h>
        void f(void) __attribute__ ((visibility(\"hidden\")));
        int main(void) {return 0;}
    "
    HAVE___ATTR__VIS_HIDDEN)

unset(CMAKE_COMPILE_FLAGS)
set(PTLIBZIPPY_PC ${PTlibzippy_BINARY_DIR}/ptlibzippy.pc)
configure_file(${PTlibzippy_SOURCE_DIR}/ptlibzippy.pc.cmakein ${PTLIBZIPPY_PC} @ONLY)
configure_file(${PTlibzippy_BINARY_DIR}/ptzippyconf.h.cmakein ${PTlibzippy_BINARY_DIR}/ptzippyconf.h)

# ============================================================================
# zlib
# ============================================================================

set(PTLIBZIPPY_PUBLIC_HDRS ${PTlibzippy_BINARY_DIR}/ptzippyconf.h ptlibzippy.h)

set(PTLIBZIPPY_PRIVATE_HDRS
    crc32.h
    deflate.h
    ptzippyguts.h
    inffast.h
    inffixed.h
    inflate.h
    inftrees.h
    trees.h
    ptzippyutil.h)

set(PTLIBZIPPY_SRCS
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    ptzippyutil.c)

if(WIN32)
    set(ptlibzippy_static_suffix "s")
    set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

if(PTLIBZIPPY_BUILD_SHARED)
    add_library(
        ptlibzippy SHARED ${PTLIBZIPPY_SRCS} ${PTLIBZIPPY_PUBLIC_HDRS} ${PTLIBZIPPY_PRIVATE_HDRS}
                    $<$<OR:$<BOOL:${WIN32}>,$<BOOL:${CYGWIN}>>:win32/zlib1.rc>)
    add_library(PTlibzippy::PTlibzippy ALIAS ptlibzippy)
    target_include_directories(
        ptlibzippy
        PUBLIC $<BUILD_INTERFACE:${PTlibzippy_BINARY_DIR}>
               $<BUILD_INTERFACE:${PTlibzippy_SOURCE_DIR}>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_compile_definitions(
        ptlibzippy
        PRIVATE PTLIBZIPPY_BUILD
                $<$<BOOL:NOT:${HAVE_FSEEKO}>:NO_FSEEKO>
                $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>
                $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_DEPRECATE>
                $<$<BOOL:${MSVC}>:_CRT_NONSTDC_NO_DEPRECATE>
        PUBLIC $<$<BOOL:${HAVE_OFF64_T}>:_LARGEFILE64_SOURCE=1>)
    set(INSTALL_VERSION ${PTlibzippy_VERSION})

    if(NOT CYGWIN)
        set_target_properties(ptlibzippy PROPERTIES SOVERSION ${PTlibzippy_VERSION_MAJOR}
                                              VERSION ${INSTALL_VERSION})
    endif(NOT CYGWIN)

    set_target_properties(
        ptlibzippy
        PROPERTIES DEFINE_SYMBOL PTLIBZIPPY_DLL
                   EXPORT_NAME PTlibzippy
                   OUTPUT_NAME ptlibzippy)
    if(UNIX
        AND NOT APPLE
        AND NOT (CMAKE_SYSTEM_NAME STREQUAL AIX)
        AND NOT (CMAKE_SYSTEM_NAME STREQUAL SunOS))
        # On unix-like platforms the library is almost always called libz
        set_target_properties(
            ptlibzippy
            PROPERTIES LINK_FLAGS
                       "-Wl,--version-script,\"${PTlibzippy_SOURCE_DIR}/ptlibzippy.map\"")
    endif(
        UNIX
        AND NOT APPLE
        AND NOT (CMAKE_SYSTEM_NAME STREQUAL AIX)
        AND NOT (CMAKE_SYSTEM_NAME STREQUAL SunOS))
endif(PTLIBZIPPY_BUILD_SHARED)

if(PTLIBZIPPY_BUILD_STATIC)
    add_library(ptlibzippystatic STATIC ${PTLIBZIPPY_SRCS} ${PTLIBZIPPY_PUBLIC_HDRS}
                                  ${PTLIBZIPPY_PRIVATE_HDRS})
    add_library(PTlibzippy::PTlibzippyStatic ALIAS ptlibzippystatic)
    target_include_directories(
        ptlibzippystatic
        PUBLIC $<BUILD_INTERFACE:${PTlibzippy_BINARY_DIR}>
               $<BUILD_INTERFACE:${PTlibzippy_SOURCE_DIR}>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_compile_definitions(
        ptlibzippystatic
        PRIVATE PTLIBZIPPY_BUILD
                $<$<BOOL:NOT:${HAVE_FSEEKO}>:NO_FSEEKO>
                $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>
                $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_DEPRECATE>
                $<$<BOOL:${MSVC}>:_CRT_NONSTDC_NO_DEPRECATE>
        PUBLIC $<$<BOOL:${HAVE_OFF64_T}>:_LARGEFILE64_SOURCE=1>)
    set_target_properties(
        ptlibzippystatic PROPERTIES EXPORT_NAME PTlibzippyStatic OUTPUT_NAME
                                                     ptlibzippy${ptlibzippy_static_suffix})
endif(PTLIBZIPPY_BUILD_STATIC)

if(PTLIBZIPPY_INSTALL)
    if(PTLIBZIPPY_BUILD_SHARED)
        install(
            TARGETS ptlibzippy
            COMPONENT Runtime
            EXPORT ptlibzippySharedExport
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        install(
            EXPORT ptlibzippySharedExport
            FILE PTlibzippy-shared.cmake
            NAMESPACE PTlibzippy::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ptlibzippy)
        if(PTLIBZIPPY_INSTALL_COMPAT_DLL)
            install(
                FILES $<TARGET_FILE:ptlibzippy>
                COMPONENT Development
                RENAME ptlibzippy1.dll
                DESTINATION "${CMAKE_INSTALL_BINDIR}")
        endif(PTLIBZIPPY_INSTALL_COMPAT_DLL)

        if(MSVC)
            install(
                FILES $<TARGET_PDB_FILE:ptlibzippy>
                COMPONENT Runtime
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                CONFIGURATIONS Debug OR RelWithDebInfo
                OPTIONAL)
        endif(MSVC)
    endif(PTLIBZIPPY_BUILD_SHARED)

    if(PTLIBZIPPY_BUILD_STATIC)
        install(
            TARGETS ptlibzippystatic
            COMPONENT Development
            EXPORT ptlibzippyStaticExport
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        install(
            EXPORT ptlibzippyStaticExport
            FILE PTlibzippy-static.cmake
            NAMESPACE PTlibzippy::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ptlibzippy)

        if(PTLIBZIPPY_INSTALL_COMPAT_DLL AND MINGW)
            install(
                FILES $<TARGET_FILE:ptlibzippystatic>
                COMPONENT Development
                RENAME libptlibzippy.dll.a
                DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        endif(PTLIBZIPPY_INSTALL_COMPAT_DLL AND MINGW)
    endif(PTLIBZIPPY_BUILD_STATIC)

    configure_package_config_file(
        ${PTlibzippy_SOURCE_DIR}/ptlibzippyConfig.cmake.in
        ${PTlibzippy_BINARY_DIR}/PTlibzippyConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ptlibzippy)

    write_basic_package_version_file(
        "${PTlibzippy_BINARY_DIR}/PTlibzippyConfigVersion.cmake"
        VERSION "${PTlibzippy_VERSION}"
        COMPATIBILITY AnyNewerVersion)

    install(FILES ${PTlibzippy_BINARY_DIR}/PTlibzippyConfig.cmake
                  ${PTlibzippy_BINARY_DIR}/PTlibzippyConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ptlibzippy)
    install(
        FILES ${PTLIBZIPPY_PUBLIC_HDRS}
        COMPONENT Development
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(
        FILES ptlibzippy.3
        COMPONENT Docs
        DESTINATION "${CMAKE_INSTALL_MANDIR}/man3")
    install(
        FILES LICENSE
              doc/algorithm.txt
              doc/crc-doc.1.0.pdf
              doc/rfc1950.txt
              doc/rfc1951.txt
              doc/rfc1952.txt
              doc/txtvsbin.txt
        COMPONENT Docs
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/ptlibzippy")
    install(
        FILES ${PTLIBZIPPY_PC}
        COMPONENT Development
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif(PTLIBZIPPY_INSTALL)

# ============================================================================
# Tests
# ============================================================================
if(PTLIBZIPPY_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif(PTLIBZIPPY_BUILD_TESTING)

add_subdirectory(contrib)
