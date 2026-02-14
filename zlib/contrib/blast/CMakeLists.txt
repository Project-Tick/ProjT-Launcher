cmake_minimum_required(VERSION 3.12...3.31)

project(
    blast
    VERSION 1.3.0
    LANGUAGES C
    DESCRIPTION "A library for creating zipfiles based in PTlibzippy"
    HOMEPAGE_URL "http://projecttick.org/p/zlib")

option(PTLIBZIPPY_BLAST_BUILD_SHARED "Enable building blast shared library" ON)
option(PTLIBZIPPY_BLAST_BUILD_STATIC "Enable building blast static library" ON)
option(PTLIBZIPPY_BLAST_BUILD_TESTING "Enable building tests for blast" ON)
option(PTLIBZIPPY_BLAST_INSTALL "Enable installation of blast" ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(WIN32 OR CYGWIN)
    set(ptlibzippyblast_static_suffix "s")
    set(CMAKE_DEBUG_POSTFIX "d")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32 OR CYGWIN)

function(blast_findTestEnv testName)
    set(testEnv "PATH=")

    if(MSVC OR MINGW)
        set(separator "\\\;")
    else()
        set(separator ":")
    endif()

    string(APPEND testEnv "$<TARGET_FILE_DIR:BLAST::BLAST>${separator}")
    string(APPEND testEnv "$ENV{PATH}")

    set_tests_properties(${testName} PROPERTIES ENVIRONMENT "${testEnv}")
endfunction(blast_findTestEnv testName)

if(PTLIBZIPPY_BLAST_BUILD_SHARED)
    add_library(ptlibzippy_blast_blast SHARED
                blast.c
                blast.h)

    add_library(BLAST::BLAST ALIAS ptlibzippy_blast_blast)

    if(NOT CYGWIN)
        set_target_properties(ptlibzippy_blast_blast
            PROPERTIES
                SOVERSION ${blast_VERSION_MAJOR}
                VERSION ${blast_VERSION})
    endif(NOT CYGWIN)

    set_target_properties(ptlibzippy_blast_blast
        PROPERTIES
            EXPORT_NAME BLAST
            OUTPUT_NAME blast)

    if(PTLIBZIPPY_BLAST_BUILD_TESTING)
        enable_testing()
        add_executable(ptlibzippy_blast_blast-test blast-test.c)
        target_link_libraries(ptlibzippy_blast_blast-test PRIVATE ptlibzippy_blast_blast)

        add_test(NAME ptlibzippy_blast_blast-test
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/tester.cmake
                    "$<TARGET_FILE:ptlibzippy_blast_blast-test>"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}")

        if(MSVC
           OR MSYS
           OR MINGW
           OR CYGWIN)
            blast_findtestenv(ptlibzippy_blast_blast-test)
        endif(
            MSVC
            OR MSYS
            OR MINGW
            OR CYGWIN)
    endif(PTLIBZIPPY_BLAST_BUILD_TESTING)
endif(PTLIBZIPPY_BLAST_BUILD_SHARED)

if(PTLIBZIPPY_BLAST_BUILD_STATIC)
    add_library(ptlibzippy_blast_blastStatic STATIC
                blast.c
                blast.h)

    add_library(BLAST::BLASTSTATIC ALIAS ptlibzippy_blast_blastStatic)

    set_target_properties(ptlibzippy_blast_blastStatic
        PROPERTIES
            EXPORT_NAME BLASTSTATIC
            OUTPUT_NAME blast${ptlibzippyblast_static_suffix})

    if(PTLIBZIPPY_BLAST_BUILD_TESTING)
        enable_testing()
        add_executable(ptlibzippy_blast_testStatic blast-test.c)
        target_link_libraries(ptlibzippy_blast_testStatic
            PRIVATE ptlibzippy_blast_blastStatic)

        add_test(NAME ptlibzippy_blast_testStatic
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/tester.cmake
                    "$<TARGET_FILE:ptlibzippy_blast_testStatic>"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}")
    endif(PTLIBZIPPY_BLAST_BUILD_TESTING)
endif(PTLIBZIPPY_BLAST_BUILD_STATIC)

if(PTLIBZIPPY_BLAST_BUILD_TESTING)
    add_subdirectory(test)
endif(PTLIBZIPPY_BLAST_BUILD_TESTING)

if(PTLIBZIPPY_BLAST_INSTALL)
    if(PTLIBZIPPY_BLAST_BUILD_SHARED)
        install(
            TARGETS ptlibzippy_blast_blast
            COMPONENT Runtime
            EXPORT ptlibzippyBlastSharedExport
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        install(
            EXPORT ptlibzippyBlastSharedExport
            FILE blast-shared.cmake
            NAMESPACE BLAST::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/blast)

        if(MSVC)
            install(
                FILES $<TARGET_PDB_FILE:ptlibzippy_blast_blast>
                COMPONENT Development
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                CONFIGURATIONS Debug OR RelWithDebInfo
                OPTIONAL)
        endif(MSVC)
    endif(PTLIBZIPPY_BLAST_BUILD_SHARED)

    if(PTLIBZIPPY_BLAST_BUILD_STATIC)
        install(
            TARGETS ptlibzippy_blast_blastStatic
            COMPONENT Development
            EXPORT ptlibzippyBlastStaticExport
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
        install(
            EXPORT ptlibzippyBlastStaticExport
            FILE blast-static.cmake
            NAMESPACE BLAST::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/blast)
    endif(PTLIBZIPPY_BLAST_BUILD_STATIC)

    configure_package_config_file(
        ${blast_SOURCE_DIR}/blastConfig.cmake.in
        ${blast_BINARY_DIR}/blastConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/blast)

    write_basic_package_version_file(
        "${blast_BINARY_DIR}/blastConfigVersion.cmake"
        VERSION "${blast_VERSION}"
        COMPATIBILITY AnyNewerVersion)

    install(FILES ${blast_BINARY_DIR}/blastConfig.cmake
                  ${blast_BINARY_DIR}/blastConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/blast)
    install(
        FILES blast.h
        COMPONENT Development
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif(PTLIBZIPPY_BLAST_INSTALL)
