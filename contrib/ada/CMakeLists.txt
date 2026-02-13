cmake_minimum_required(VERSION 3.12...3.31)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

project(
    ptlibzippyAda
    VERSION 1.0.0
    LANGUAGES C ADA
    DESCRIPTION "A library for creating zipfiles based in PTlibzippy"
    HOMEPAGE_URL "http://projecttick.org/p/zlib")

option(PTLIBZIPPY_ADA_BUILD_SHARED "Enable building ada bindings shared library" ON)
option(PTLIBZIPPY_ADA_BUILD_STATIC "Enable building ada bindings static library" ON)
option(PTLIBZIPPY_ADA_BUILD_TESTING "Enable building tests for ada bindings library" ON)

if(WIN32 OR CYGWIN)
    set(ptlibzippy_Ada_static_suffix "s")
    set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32 OR CYGWIN)

if(NOT DEFINED PTLIBZIPPY_BUILD_ADA)
    if(PTLIBZIPPY_ADA_BUILD_SHARED)
        list(APPEND REQUIRED_COMPONENTS "shared")
    endif(PTLIBZIPPY_ADA_BUILD_SHARED)

    if(PTLIBZIPPY_ADA_BUILD_STATIC)
        list(APPEND REQUIRED_COMPONENTS "static")
    endif(PTLIBZIPPY_ADA_BUILD_STATIC)

    find_package(PTlibzippy REQUIRED COMPONENTS ${REQUIRED_COMPONENTS} CONFIG)
endif(NOT DEFINED PTLIBZIPPY_BUILD_ADA)

function(PTLIBZIPPY_ADA_findTestEnv testName)
    set(testEnv "PATH=")

    if(MSVC OR MINGW)
        set(separator "\\\;")
    else()
        set(separator ":")
    endif()

    string(APPEND testEnv "$<TARGET_FILE_DIR:PTlibzippy::PTlibzippy>${separator}")
    string(APPEND testEnv "$ENV{PATH}")

    set_tests_properties(${testName} PROPERTIES ENVIRONMENT "${testEnv}")
endfunction(PTLIBZIPPY_ADA_findTestEnv testName)

if(PTLIBZIPPY_ADA_BUILD_SHARED)
    ada_add_library(ptlibzippy_ada_Ada SHARED
        ptlibzippy-thin.adb
        ptlibzippy.adb)

    set_target_properties(ptlibzippy_ada_Ada
        PROPERTIES OUTPUT_NAME ptlibzippy-ada)

    target_link_libraries(ptlibzippy_ada_Ada
        INTERFACE PTlibzippy::PTlibzippy)

    ada_add_library(ptlibzippy_ada_streams SHARED
        ptlibzippy-streams.adb)

    target_link_libraries(ptlibzippy_ada_streams
        PUBLIC
            ptlibzippy_ada_Ada)

    ada_find_ali(ptlibzippy_ada_streams)

    if(PTLIBZIPPY_ADA_BUILD_TESTING)
        enable_testing()
        ada_add_executable(ptlibzippy_ada_test test.adb)

        target_link_libraries(ptlibzippy_ada_test
            PRIVATE
                ptlibzippy_ada_Ada
                ptlibzippy_ada_streams)

        ada_find_ali(ptlibzippy_ada_test)

        add_test(NAME ptlibzippy_ada_ada-test COMMAND ptlibzippy_ada_test)
        set_tests_properties(ptlibzippy_ada_ada-test PROPERTIES FIXTURES_REQUIRED ptlibzippy_ada_cleanup)

        if(MSVC
           OR MSYS
           OR MINGW
           OR CYGWIN)
            ptlibzippy_ada_findtestenv(ptlibzippy_ada_ada-test)
        endif(
            MSVC
            OR MSYS
            OR MINGW
            OR CYGWIN)

        ada_add_executable(ptlibzippy_ada_buffer_demo buffer_demo.adb)

        target_link_libraries(ptlibzippy_ada_buffer_demo
            PRIVATE
                ptlibzippy_ada_Ada)

        ada_find_ali(ptlibzippy_ada_buffer_demo)

        add_test(NAME ptlibzippy_ada_buffer-demo COMMAND ptlibzippy_ada_buffer_demo)

        if(MSVC
           OR MSYS
           OR MINGW
           OR CYGWIN)
            ptlibzippy_ada_findtestenv(ptlibzippy_ada_buffer-demo)
        endif(
            MSVC
            OR MSYS
            OR MINGW
            OR CYGWIN)

        ada_add_executable(ptlibzippy_ada_mtest mtest.adb)

        target_link_libraries(ptlibzippy_ada_mtest
            PRIVATE
                ptlibzippy_ada_Ada)

        ada_find_ali(ptlibzippy_ada_mtest)

        #Not adding test as this is an endless-loop

        ada_add_executable(ptlibzippy_ada_read read.adb)

        target_link_libraries(ptlibzippy_ada_read
            PRIVATE
                ptlibzippy_ada_Ada)

        ada_find_ali(ptlibzippy_ada_read)

        add_test(NAME ptlibzippy_ada_read COMMAND ptlibzippy_ada_read)

        if(MSVC
           OR MSYS
           OR MINGW
           OR CYGWIN)
            ptlibzippy_ada_findtestenv(ptlibzippy_ada_read)
        endif(
            MSVC
            OR MSYS
            OR MINGW
            OR CYGWIN)
    endif(PTLIBZIPPY_ADA_BUILD_TESTING)
endif(PTLIBZIPPY_ADA_BUILD_SHARED)

if(PTLIBZIPPY_ADA_BUILD_STATIC)
    ada_add_library(ptlibzippy_ada_AdaStatic STATIC
        ptlibzippy-thin.adb
        ptlibzippy.adb)

    target_link_libraries(ptlibzippy_ada_AdaStatic
        INTERFACE PTlibzippy::PTlibzippyStatic)

    set_target_properties(ptlibzippy_ada_AdaStatic
        PROPERTIES OUTPUT_NAME ptlibzippy-ada${ptlibzippy_Ada_static_suffix})

    ada_add_library(ptlibzippy_ada_streamsStatic STATIC
        ptlibzippy-streams.adb)

    target_link_libraries(ptlibzippy_ada_streamsStatic
        PUBLIC
            ptlibzippy_ada_AdaStatic)

    ada_find_ali(ptlibzippy_ada_streamsStatic)

    if(PTLIBZIPPY_ADA_BUILD_TESTING)
        enable_testing()
        ada_add_executable(ptlibzippy_ada_testStatic test.adb)

        target_link_libraries(ptlibzippy_ada_testStatic
            PRIVATE
                ptlibzippy_ada_AdaStatic
                ptlibzippy_ada_streamsStatic)

        ada_find_ali(ptlibzippy_ada_testStatic)

        add_test(NAME ptlibzippy_ada_testStatic COMMAND ptlibzippy_ada_testStatic)
        set_tests_properties(ptlibzippy_ada_testStatic PROPERTIES FIXTURES_REQUIRED ptlibzippy_ada_cleanup)

        ada_add_executable(ptlibzippy_ada_buffer-demoStatic buffer_demo.adb)

        target_link_libraries(ptlibzippy_ada_buffer-demoStatic
            PRIVATE
                ptlibzippy_ada_AdaStatic)

        ada_find_ali(ptlibzippy_ada_buffer-demoStatic)

        add_test(NAME ptlibzippy_ada_buffer-demoStatic COMMAND ptlibzippy_ada_buffer-demoStatic)

        ada_add_executable(ptlibzippy_ada_mtestStatic mtest.adb)

        target_link_libraries(ptlibzippy_ada_mtestStatic
            PRIVATE
                ptlibzippy_ada_AdaStatic)

        ada_find_ali(ptlibzippy_ada_mtestStatic)

        # Not adding test as this is an endless-loop

        ada_add_executable(ptlibzippy_ada_readStatic read.adb)

        target_link_libraries(ptlibzippy_ada_readStatic
            PRIVATE
                ptlibzippy_ada_AdaStatic)

        ada_find_ali(ptlibzippy_ada_readStatic)

        add_test(NAME ptlibzippy_ada_readStatic COMMAND ptlibzippy_ada_readStatic)
    endif(PTLIBZIPPY_ADA_BUILD_TESTING)
endif(PTLIBZIPPY_ADA_BUILD_STATIC)

if(PTLIBZIPPY_ADA_BUILD_TESTING)
    add_test(NAME ptlibzippy_ada_cleanup COMMAND ${CMAKE_COMMAND} -E rm ${CMAKE_CURRENT_BINARY_DIR}/testptlibzippy.in
        ${CMAKE_CURRENT_BINARY_DIR}/testptlibzippy.out ${CMAKE_CURRENT_BINARY_DIR}/testptlibzippy.zlb)
    set_tests_properties(ptlibzippy_ada_cleanup PROPERTIES FIXTURES_CLEANUP ptlibzippy_ada_cleanup)
endif(PTLIBZIPPY_ADA_BUILD_TESTING)
