cmake_minimum_required(VERSION 3.12...3.31)

project(
    iostreamV3
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "A library for using C++ IOStreams with PTlibzippy V3"
    HOMEPAGE_URL "http://projecttick.org/p/zlib")

option(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED "Enable building blast shared library" ON)
option(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC "Enable building blast static library" ON)
option(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING "Enable building tests for blast" ON)
option(PTLIBZIPPY_IOSTREAM3_INSTALL "Enable installation of iostream" ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DEFINED PTLIBZIPPY_BUILD_IOSTREAM3)
    if(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED)
        list(APPEND REQUIRED_COMPONENTS "shared")
    endif(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED)

    if(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC)
        list(APPEND REQUIRED_COMPONENTS "static")
    endif(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC)

    find_package(PTlibzippy REQUIRED COMPONENTS ${REQUIRED_COMPONENTS} CONFIG)
endif(NOT DEFINED PTLIBZIPPY_BUILD_IOSTREAM3)

if(WIN32 OR CYGWIN)
    set(ptlibzippyIOStream3_static_suffix "s")
    set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32 OR CYGWIN)


function(iostreamv3_findTestEnv testName)
    set(testEnv "PATH=")

    if(MSVC OR MINGW)
        set(separator "\\\;")
    else()
        set(separator ":")
    endif()

    string(APPEND testEnv "$<TARGET_FILE_DIR:ptlibzippy_iostream3_iostreamv3>${separator}")
    string(APPEND testEnv "$<TARGET_FILE_DIR:PTlibzippy::PTlibzippy>${separator}")
    string(APPEND testEnv "$ENV{PATH}")

    set_tests_properties(${testName} PROPERTIES ENVIRONMENT "${testEnv}")
endfunction(iostreamv3_findTestEnv testName)

if(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED)
    add_library(ptlibzippy_iostream3_iostreamv3 SHARED
                zfstream.cc
                zfstream.h)

    add_library(IOSTREAMV3::IOSTREAMV3 ALIAS ptlibzippy_iostream3_iostreamv3)

    if(NOT CYGWIN)
        set_target_properties(ptlibzippy_iostream3_iostreamv3
            PROPERTIES
                SOVERSION ${iostreamV3_VERSION_MAJOR}
                VERSION ${iostreamV3_VERSION})
    endif(NOT CYGWIN)

    set_target_properties(ptlibzippy_iostream3_iostreamv3
        PROPERTIES
            EXPORT_NAME IOSTREAMV3
            OUTPUT_NAME iostream3)

    target_link_libraries(ptlibzippy_iostream3_iostreamv3
        PUBLIC PTlibzippy::PTlibzippy)

    if(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING)
        enable_testing()

        add_executable(ptlibzippy_iostream3_test test.cc zfstream.h)

        target_link_libraries(ptlibzippy_iostream3_test
            PRIVATE ptlibzippy_iostream3_iostreamv3)

        add_test(NAME ptlibzippy_iostream3_test COMMAND ptlibzippy_iostream3_test)

        set_tests_properties(ptlibzippy_iostream3_test
            PROPERTIES
                FIXTURES_REQUIRED ptlibzippy_iostream3_cleanup)

        if(MSVC
           OR MSYS
           OR MINGW
           OR CYGWIN)
            iostreamv3_findtestenv(ptlibzippy_iostream3_test)
        endif(
            MSVC
            OR MSYS
            OR MINGW
            OR CYGWIN)
    endif(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING)
endif(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED)

if(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC)
    add_library(ptlibzippy_iostream3_iostreamv3Static STATIC
                zfstream.cc
                zfstream.h)

    add_library(IOSTREAMV3::IOSTREAMV3STATIC
        ALIAS ptlibzippy_iostream3_iostreamv3Static)

    target_link_libraries(ptlibzippy_iostream3_iostreamv3Static
        PUBLIC PTlibzippy::PTlibzippyStatic)

    set_target_properties(ptlibzippy_iostream3_iostreamv3Static
        PROPERTIES
            EXPORT_NAME IOSTREAMV3STATIC
            OUTPUT_NAME iostream3${ptlibzippy_IOStream3_static_suffix})

    if(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING)
        enable_testing()

        add_executable(ptlibzippy_iostream3_testStatic test.cc zfstream.h)

        target_link_libraries(ptlibzippy_iostream3_testStatic
            PRIVATE ptlibzippy_iostream3_iostreamv3Static)

        add_test(NAME ptlibzippy_iostream3_testStatic
                 COMMAND ptlibzippy_iostream3_testStatic)

        set_tests_properties(ptlibzippy_iostream3_testStatic
            PROPERTIES
                FIXTURES_REQUIRED ptlibzippy_iostream3_cleanup)
    endif(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING)
endif(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC)

if(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING)
    add_test(NAME ptlibzippy_iostream3_cleanup COMMAND ${CMAKE_COMMAND} -E rm
        ${CMAKE_CURRENT_BINARY_DIR}/test1.txt.gz
        ${CMAKE_CURRENT_BINARY_DIR}/test2.txt.gz)

    set_tests_properties(ptlibzippy_iostream3_cleanup
        PROPERTIES
            FIXTURES_CLEANUP ptlibzippy_iostream3_cleanup)

        add_subdirectory(test)
endif(PTLIBZIPPY_IOSTREAM3_BUILD_TESTING)

if(PTLIBZIPPY_IOSTREAM3_INSTALL)
    if(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED)
        install(
            TARGETS ptlibzippy_iostream3_iostreamv3
            COMPONENT Runtime
            EXPORT ptlibzippyiostream3SharedExport
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        install(
            EXPORT ptlibzippyiostream3SharedExport
            FILE iostreamv3-shared.cmake
            NAMESPACE IOSTREAMV3::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iostreamv3)

        if(MSVC)
            install(
                FILES $<TARGET_PDB_FILE:ptlibzippy_iostream3_iostreamv3>
                COMPONENT Development
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                CONFIGURATIONS Debug OR RelWithDebInfo
                OPTIONAL)
        endif(MSVC)
    endif(PTLIBZIPPY_IOSTREAM3_BUILD_SHARED)

    if(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC)
        install(
            TARGETS ptlibzippy_iostream3_iostreamv3Static
            COMPONENT Development
            EXPORT iostream3StaticExport
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        install(
            EXPORT iostream3StaticExport
            FILE iostreamv3-static.cmake
            NAMESPACE IOSTREAMV3::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iostreamv3)
    endif(PTLIBZIPPY_IOSTREAM3_BUILD_STATIC)

    configure_package_config_file(
        ${iostreamV3_SOURCE_DIR}/iostream3Config.cmake.in
        ${iostreamV3_BINARY_DIR}/iostreamv3Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iostreamv3)

    write_basic_package_version_file(
        "${iostreamV3_BINARY_DIR}/iostreamv3ConfigVersion.cmake"
        VERSION "${iostream3_VERSION}"
        COMPATIBILITY AnyNewerVersion)

    install(FILES ${iostreamV3_BINARY_DIR}/iostreamv3Config.cmake
                  ${iostreamV3_BINARY_DIR}/iostreamv3ConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/iostreamv3)
    install(
        FILES zfstream.h
        COMPONENT Development
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif(PTLIBZIPPY_IOSTREAM3_INSTALL)
