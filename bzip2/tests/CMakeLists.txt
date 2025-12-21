
#
# Paths to pass to our tests via environment variables
#
if(WIN32)
    file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}         SOURCE)
    file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR}         BUILD)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} TMP)

    if(ENABLE_STATIC_LIB)
        file(TO_NATIVE_PATH $<TARGET_FILE:bz2_static> LIBBZ2)
    else()
        file(TO_NATIVE_PATH $<TARGET_FILE:bz2>        LIBBZ2)
    endif()

    file(TO_NATIVE_PATH $<TARGET_FILE:bzip2> BZIP2)

else() # Not WIN32
    if(ENABLE_SHARED_LIB)
        set(LD_LIBRARY_PATH $<TARGET_FILE_DIR:bz2>:$ENV{LD_LIBRARY_PATH})
    endif()

    set(SOURCE ${CMAKE_SOURCE_DIR})
    set(BUILD  ${CMAKE_BINARY_DIR})
    set(TMP    ${CMAKE_CURRENT_BINARY_DIR})

    if(ENABLE_STATIC_LIB)
        set(LIBBZ2 $<TARGET_FILE:bz2_static>)
    else()
        set(LIBBZ2 $<TARGET_FILE:bz2>)
    endif()

    set(BZIP2 $<TARGET_FILE:bzip2>)

endif()

set(ENVIRONMENT
    PYTHONTRACEMALLOC=1 VERSION=${PROJECT_VERSION}
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
    DYLD_LIBRARY_PATH=${LD_LIBRARY_PATH}
    PATH_SOURCE=${SOURCE}
    PATH_BUILD=${BUILD}
    PATH_TMP=${TMP}
    PATH_LIBBZ2=${LIBBZ2}
    PATH_BZIP2=${BZIP2}
)

#
# The Tests
# ~~~~~~~~~
#
# Run all tests with: `ctest`
#                 or: `ctest -V` for verbose output
#
# Run a specific test with the `-R` option, like this:
#                     `ctest -V -R quick
#

add_test(NAME quick COMMAND ${PythonTest_COMMAND};quick_test.py
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
if(Valgrind_FOUND)
    set_property(TEST quick PROPERTY ENVIRONMENT ${ENVIRONMENT} VALGRIND=${Valgrind_EXECUTABLE})
else()
    set_property(TEST quick PROPERTY ENVIRONMENT ${ENVIRONMENT})
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/input/bzip2-testfiles/README.md)
    add_test(NAME large COMMAND ${PythonTest_COMMAND};large_test.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    if(Valgrind_FOUND)
        set_property(TEST large PROPERTY ENVIRONMENT ${ENVIRONMENT} VALGRIND=${Valgrind_EXECUTABLE})
    else()
        set_property(TEST large PROPERTY ENVIRONMENT ${ENVIRONMENT})
    endif()
endif()
