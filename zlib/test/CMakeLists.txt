function(PTLIBZIPPY_findTestEnv testName)
    set(testEnv "PATH=")

    if(MSVC OR MINGW)
        set(separator "\\\;")
    else()
        set(separator ":")
    endif()

    string(APPEND testEnv "$<TARGET_FILE_DIR:PTlibzippy::PTlibzippy>${separator}")
    string(APPEND testEnv "$ENV{PATH}")

    set_tests_properties(${testName} PROPERTIES ENVIRONMENT "${testEnv}")
endfunction()

if(PTLIBZIPPY_BUILD_SHARED)
    add_executable(ptlibzippy_example example.c)
    target_link_libraries(ptlibzippy_example PTlibzippy::PTlibzippy)
    target_compile_definitions(ptlibzippy_example PRIVATE PTLIBZIPPY_BUILD)
    add_test(NAME ptlibzippy_example COMMAND ptlibzippy_example)

    add_executable(minigzip minigzip.c)
    target_compile_definitions(
        minigzip PRIVATE $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>)
    target_link_libraries(minigzip PTlibzippy::PTlibzippy)

    if(MSVC
       OR MSYS
       OR MINGW
       OR CYGWIN)
        ptlibzippy_findtestenv(ptlibzippy_example)
    endif(
        MSVC
        OR MSYS
        OR MINGW
        OR CYGWIN)

    if(HAVE_OFF64_T)
        add_executable(ptlibzippy_example64 example.c)
        target_compile_definitions(
            ptlibzippy_example64
            PRIVATE PTLIBZIPPY_BUILD
                    $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>)
        target_link_libraries(ptlibzippy_example64 PTlibzippy::PTlibzippy)
        add_test(NAME ptlibzippy_example64 COMMAND ptlibzippy_example64)

        if(MSVC
           OR MSYS
           OR MINGW
           OR CYGWIN)
            ptlibzippy_findtestenv(ptlibzippy_example64)
        endif(
            MSVC
            OR MSYS
            OR MINGW
            OR CYGWIN)
    endif(HAVE_OFF64_T)
endif(PTLIBZIPPY_BUILD_SHARED)

if(PTLIBZIPPY_BUILD_STATIC)
    add_executable(ptlibzippy_static_example example.c)
    target_link_libraries(ptlibzippy_static_example PTlibzippy::PTlibzippyStatic)
    target_compile_definitions(
        ptlibzippy_static_example
        PRIVATE PTLIBZIPPY_BUILD
                $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>)
    add_test(NAME ptlibzippy_static_example COMMAND ptlibzippy_static_example)

    add_executable(static_minigzip minigzip.c)
    target_link_libraries(static_minigzip PTlibzippy::PTlibzippyStatic)

    if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL
                                                "Clang")
        set(CFLAGS_OLD ${CMAKE_C_FLAGS})
        set(CMAKE_C_FLAGS
            ""
            CACHE STRING "" FORCE)

        if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
            find_program(GCOV_EXECUTABLE gcov)
        endif(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

        if(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
            set(llvm_names llvm_cov)
            list(APPEND llvm_names llvm-cov)

            foreach(ver RANGE 11 99)
                list(APPEND llvm_names llvm-cov-${ver})
            endforeach(ver RANGE 11 99)

            find_program(GCOV_EXECUTABLE NAMES ${llvm_names})
            set(llvm_option "gcov")
        endif(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")

        add_executable(infcover infcover.c)
        target_link_libraries(infcover PTlibzippy::PTlibzippyStatic)
        target_compile_options(infcover PRIVATE -coverage)
        target_link_options(infcover PRIVATE -coverage)
        target_compile_definitions(
            infcover PRIVATE $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>)
        add_test(NAME ptlibzippy_coverage COMMAND infcover)
        if(CMAKE_CONFIGURATION_TYPES)
            set(INFCOVER_DIR
                ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/infcover.dir/$<CONFIG>)
        else()
            set(INFCOVER_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/infcover.dir)
        endif()
        add_test(
            NAME ptlibzippy_coverage-summary
            COMMAND
                ${GCOV_EXECUTABLE} ${llvm_option}
                ${CMAKE_CURRENT_SOURCE_DIR}/infcover.c -o
                ${INFCOVER_DIR}/infcover.c.gcda)
        set_tests_properties(ptlibzippy_coverage-summary PROPERTIES DEPENDS
                                                              ptlibzippy_coverage)
        set(CMAKE_C_FLAGS
            ${CFLAGS_OLD}
            CACHE STRING "" FORCE)
    endif(${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID}
                                                   STREQUAL "Clang")

    if(HAVE_OFF64_T)
        add_executable(ptlibzippy_static_example64 example.c)
        target_compile_definitions(
            ptlibzippy_static_example64
            PRIVATE PTLIBZIPPY_BUILD
                    $<$<BOOL:${HAVE___ATTR__VIS_HIDDEN}>:HAVE_HIDDEN>)
        target_link_libraries(ptlibzippy_static_example64 PTlibzippy::PTlibzippyStatic)
        add_test(NAME ptlibzippy_static_example64 COMMAND ptlibzippy_static_example64)
    endif(HAVE_OFF64_T)
endif(PTLIBZIPPY_BUILD_STATIC)

add_test(
    NAME ptlibzippy_install
    COMMAND ${CMAKE_COMMAND} --install ${PTlibzippy_BINARY_DIR} --prefix
            ${CMAKE_CURRENT_BINARY_DIR}/test_install --config $<CONFIG>
    WORKING_DIRECTORY ${PTlibzippy_BINARY_DIR})

set_tests_properties(ptlibzippy_install PROPERTIES FIXTURES_SETUP ptlibzippy_install)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/findpackage_test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/find_package_no_components_test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/find_package_wrong_components_test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_exclude_test)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/find_package_test.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/findpackage_test/CMakeLists.txt @ONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/find_package_no_components_test.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/findpackage_no_components_test/CMakeLists.txt @ONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/find_package_wrong_components_test.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/findpackage_wrong_components_test/CMakeLists.txt @ONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/add_subdirectory_test.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_test/CMakeLists.txt @ONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/add_subdirectory_exclude_test.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_exclude_test/CMakeLists.txt
    @ONLY)

# CMAKE_GENERATOR_PLATFORM doesn't work in the if
set(GENERATOR ${CMAKE_GENERATOR_PLATFORM})

if(GENERATOR)
    set(PLATFORM "-A ${GENERATOR}")
endif(GENERATOR)
#
# findpackage_test
#
add_test(
    NAME ptlibzippy_find_package_configure
    COMMAND
        ${CMAKE_COMMAND} ${PLATFORM}
        -B${CMAKE_CURRENT_BINARY_DIR}/findpackage_test_build
        -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/test_install --fresh
        -G "${CMAKE_GENERATOR}" -S${CMAKE_CURRENT_BINARY_DIR}/findpackage_test)

add_test(
    NAME ptlibzippy_find_package_build
    COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/findpackage_test_build)

add_test(
    NAME ptlibzippy_find_package_test
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/findpackage_test_build)

set_tests_properties(
    ptlibzippy_find_package_configure PROPERTIES FIXTURES_REQUIRED ptlibzippy_install
                                           FIXTURES_SETUP fp_config)

set_tests_properties(
    ptlibzippy_find_package_build PROPERTIES FIXTURES_REQUIRED fp_config
                                       FIXTURES_SETUP fp_build)

set_tests_properties(
    ptlibzippy_find_package_test PROPERTIES FIXTURES_REQUIRED fp_build ENVIRONMENT
                                      CTEST_OUTPUT_ON_FAILURE=1)

#
# findpackage_no_components_test
#
add_test(
    NAME ptlibzippy_find_package_no_components_configure
    COMMAND
        ${CMAKE_COMMAND} ${PLATFORM}
        -B${CMAKE_CURRENT_BINARY_DIR}/findpackage_no_components_test_build
        -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/test_install --fresh
        -G "${CMAKE_GENERATOR}"
        -S${CMAKE_CURRENT_BINARY_DIR}/findpackage_no_components_test)

set_tests_properties(
    ptlibzippy_find_package_no_components_configure
        PROPERTIES FIXTURES_REQUIRED ptlibzippy_install)

if(NOT PTLIBZIPPY_BUILD_SHARED OR NOT PTLIBZIPPY_BUILD_STATIC)
    set_tests_properties(
        ptlibzippy_find_package_no_components_configure
            PROPERTIES WILL_FAIL TRUE)
endif(NOT PTLIBZIPPY_BUILD_SHARED OR NOT PTLIBZIPPY_BUILD_STATIC)

#
# findpackage_no_component_test
#
add_test(
    NAME ptlibzippy_find_package_wrong_components_configure
    COMMAND
        ${CMAKE_COMMAND} ${PLATFORM}
        -B${CMAKE_CURRENT_BINARY_DIR}/findpackage_wrong_components_test_build
        -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/test_install --fresh
        -G "${CMAKE_GENERATOR}"
        -S${CMAKE_CURRENT_BINARY_DIR}/findpackage_wrong_components_test)

set_tests_properties(
    ptlibzippy_find_package_wrong_components_configure
        PROPERTIES FIXTURES_REQUIRED ptlibzippy_install
                   WILL_FAIL TRUE)

#
# add_subdirectory_test
#
add_test(
    NAME ptlibzippy_add_subdirectory_configure
    COMMAND
        ${CMAKE_COMMAND} ${PLATFORM}
        -B${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_test_build
        -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/test_install --fresh
        -G "${CMAKE_GENERATOR}" -S${CMAKE_CURRENT_BINARY_DIR}/findpackage_test)

add_test(
    NAME ptlibzippy_add_subdirectory_build
    COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_test_build)

add_test(
    NAME ptlibzippy_add_subdirectory_test
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_test_build)

set_tests_properties(
    ptlibzippy_add_subdirectory_configure PROPERTIES FIXTURES_REQUIRED ptlibzippy_install
                                               FIXTURES_SETUP as_config)

set_tests_properties(
    ptlibzippy_add_subdirectory_build PROPERTIES FIXTURES_REQUIRED as_config
                                           FIXTURES_SETUP as_build)

set_tests_properties(
    ptlibzippy_add_subdirectory_test PROPERTIES FIXTURES_REQUIRED as_build
                                          ENVIRONMENT CTEST_OUTPUT_ON_FAILURE=1)

#
# add_subdirectory_exclude_test
#
add_test(
    NAME ptlibzippy_add_subdirectory_exclude_configure
    COMMAND
        ${CMAKE_COMMAND} ${PLATFORM}
        -B${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_exclude_test_build
        -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/test_install --fresh
        -G "${CMAKE_GENERATOR}" -S${CMAKE_CURRENT_BINARY_DIR}/findpackage_test)

add_test(
    NAME ptlibzippy_add_subdirectory_exclude_build
    COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_exclude_test_build)

add_test(
    NAME ptlibzippy_add_subdirectory_exclude_test
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory_exclude_test_build)

set_tests_properties(
    ptlibzippy_add_subdirectory_exclude_configure
    PROPERTIES FIXTURES_REQUIRED ptlibzippy_install FIXTURES_SETUP asx_config)

set_tests_properties(
    ptlibzippy_add_subdirectory_exclude_build PROPERTIES FIXTURES_REQUIRED as_config
                                                   FIXTURES_SETUP asx_build)

set_tests_properties(
    ptlibzippy_add_subdirectory_exclude_test
    PROPERTIES FIXTURES_REQUIRED asx_build ENVIRONMENT
               CTEST_OUTPUT_ON_FAILURE=1)
