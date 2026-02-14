cmake_minimum_required(VERSION 3.12...3.31)

project(
    testptlibzippy
    VERSION 1.0.0
    LANGUAGES C
    DESCRIPTION "A little program to test PTlibzippy"
    HOMEPAGE_URL "https://www.PTlibzippy.net")

option(PTLIBZIPPY_TESTPTLIBZIPPY_BUILD_SHARED "Enable building testptlibzippy" ON)
option(PTLIBZIPPY_TESTPTLIBZIPPY_BUILD_STATIC "Enable building static linked testptlibzippy" ON)
option(PTLIBZIPPY_TESTPTLIBZIPPY_INSTALL "Enable installation of testptlibzippy" ON)

if(PTLIBZIPPY_TESTPTLIBZIPPY_BUILD_SHARED)
    add_executable(ptlibzippy_testptlibzippy testptlibzippy.c)
    target_link_libraries(ptlibzippy_testptlibzippy PRIVATE PTlibzippy::PTlibzippy)

    set_target_properties(ptlibzippy_testptlibzippy
        PROPERTIES
            OUTPUT_NAME testptlibzippy)

    if(PTLIBZIPPY_TESTPTLIBZIPPY_INSTALL)
        install(
            TARGETS ptlibzippy_testptlibzippy
            COMPONENT Runtime
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif(PTLIBZIPPY_TESTPTLIBZIPPY_INSTALL)
endif(PTLIBZIPPY_TESTPTLIBZIPPY_BUILD_SHARED)

if(PTLIBZIPPY_TESTPTLIBZIPPY_BUILD_STATIC)
    add_executable(ptlibzippy_testptlibzippyStatic testptlibzippy.c)
    target_link_libraries(ptlibzippy_testptlibzippyStatic PRIVATE PTlibzippy::PTlibzippyStatic)

    set_target_properties(ptlibzippy_testptlibzippyStatic
        PROPERTIES
            OUTPUT_NAME testptlibzippyStatic)

    if(PTLIBZIPPY_TESTPTLIBZIPPY_INSTALL)
        install(
            TARGETS ptlibzippy_testptlibzippyStatic
            COMPONENT Runtime
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif(PTLIBZIPPY_TESTPTLIBZIPPY_INSTALL)
endif(PTLIBZIPPY_TESTPTLIBZIPPY_BUILD_STATIC)
