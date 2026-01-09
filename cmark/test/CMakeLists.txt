# To get verbose output: cmake --build build --target "test" -- ARGS='-V'

# By default, we run the spec tests only if python3 is available.
# To require the spec tests, compile with -DSPEC_TESTS=1

if(SPEC_TESTS)
  set(PYTHON_REQUIRED REQUIRED)
else()
  set(PYTHON_REQUIRED)
endif()

find_package(Python3 ${PYTHON_REQUIRED} COMPONENTS Interpreter)

IF (Python3_Interpreter_FOUND)

  # Detect Python architecture
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -c "import struct; print(struct.calcsize('P') * 8)"
    OUTPUT_VARIABLE PYTHON_BITS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "Python architecture: ${PYTHON_BITS}-bit")
  message(STATUS "Target architecture: ${CMAKE_SYSTEM_PROCESSOR}")
  message(STATUS "C compiler target: ${CMAKE_C_COMPILER_TARGET}")
  message(STATUS "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
  message(STATUS "Compiler ID: ${CMAKE_C_COMPILER_ID}")
  
  # Check if Python architecture matches target architecture
  set(PYTHON_ARCH_MATCHES FALSE)
  
  # MinGW cross-compilation: Python library tests often fail due to arch mismatch
  # Disable library tests for MinGW to avoid "not a valid Win32 application" errors
  if(MINGW OR CMAKE_C_COMPILER_ID MATCHES "GNU" AND WIN32)
    message(WARNING "MinGW detected - Python library tests will be skipped (arch mismatch likely)")
    set(PYTHON_ARCH_MATCHES FALSE)
  else()
    # For cross-compilation scenarios, check CMAKE_SIZEOF_VOID_P (more reliable)
    if(CMAKE_SIZEOF_VOID_P)
      set(TARGET_BITS_FROM_SIZEOF ${CMAKE_SIZEOF_VOID_P})
      math(EXPR TARGET_BITS "${TARGET_BITS_FROM_SIZEOF} * 8")
    else()
      set(TARGET_BITS 0)
    endif()
    
    # Match by bitness first (most reliable for cross-compile)
    if(PYTHON_BITS EQUAL TARGET_BITS AND TARGET_BITS GREATER 0)
      set(PYTHON_ARCH_MATCHES TRUE)
      message(STATUS "Architecture match by bitness: ${PYTHON_BITS}-bit = ${TARGET_BITS}-bit")
    # Fallback to CMAKE_SYSTEM_PROCESSOR matching (less reliable in cross-compile)
    elseif(PYTHON_BITS EQUAL 64 AND (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|x64"))
      set(PYTHON_ARCH_MATCHES TRUE)
    elseif(PYTHON_BITS EQUAL 64 AND (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64|arm64"))
      set(PYTHON_ARCH_MATCHES TRUE)
    elseif(PYTHON_BITS EQUAL 32 AND (CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86"))
      set(PYTHON_ARCH_MATCHES TRUE)
    endif()
    
    if(PYTHON_ARCH_MATCHES)
      message(STATUS "Python architecture matches target - library tests will run")
    else()
      message(WARNING "Python architecture (${PYTHON_BITS}-bit) does not match target (${TARGET_BITS}-bit / ${CMAKE_SYSTEM_PROCESSOR}) - library tests will be skipped")
    endif()
  endif()
  
  add_test(NAME html_normalization
           COMMAND "$<TARGET_FILE:Python3::Interpreter>" -m doctest "${CMAKE_CURRENT_SOURCE_DIR}/normalize.py")

  if(BUILD_SHARED_LIBS AND PYTHON_ARCH_MATCHES)
    add_test(NAME spectest_library
             COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/spec_tests.py"
                                                           --no-normalize
                                                           --spec "${CMAKE_CURRENT_SOURCE_DIR}/spec.txt"
                                                           --library-dir "$<TARGET_FILE_DIR:cmark>")

    add_test(NAME pathological_tests_library
             COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/pathological_tests.py"
                                                           --library-dir "$<TARGET_FILE_DIR:cmark>")

    add_test(NAME roundtriptest_library
             COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/roundtrip_tests.py"
                                                           --spec "${CMAKE_CURRENT_SOURCE_DIR}/spec.txt"
                                                           --library-dir "$<TARGET_FILE_DIR:cmark>")

    add_test(NAME entity_library
             COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/entity_tests.py"
                                                           --library-dir "$<TARGET_FILE_DIR:cmark>")
  endif()

  add_test(NAME spectest_executable
           COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/spec_tests.py"
                                                         --no-normalize
                                                         --spec "${CMAKE_CURRENT_SOURCE_DIR}/spec.txt"
                                                         --program "$<TARGET_FILE:cmark_exe>")

  add_test(NAME smartpuncttest_executable
           COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/spec_tests.py"
                                                         --no-normalize
                                                         --spec "${CMAKE_CURRENT_SOURCE_DIR}/smart_punct.txt"
                                                         --program "$<TARGET_FILE:cmark_exe> --smart")

  add_test(NAME regressiontest_executable
           COMMAND "$<TARGET_FILE:Python3::Interpreter>" "${CMAKE_CURRENT_SOURCE_DIR}/spec_tests.py"
                                                         --no-normalize
                                                         --spec "${CMAKE_CURRENT_SOURCE_DIR}/regression.txt"
                                                         --program "$<TARGET_FILE:cmark_exe>")

ELSE(Python3_Interpreter_FOUND)

  message(WARNING "A Python 3 Interpreter is required to run the spec tests")

ENDIF(Python3_Interpreter_FOUND)
