set(FUZZ_ENGINE "")
if(DEFINED ENV{LIB_FUZZING_ENGINE})
    set(FUZZ_ENGINE $ENV{LIB_FUZZING_ENGINE})
endif()

function(add_fuzzer target)
    add_executable(${target} ${ARGN})
    target_compile_features(${target} PRIVATE cxx_std_20)
    if(FUZZ_ENGINE)
        target_link_libraries(${target} PRIVATE ${FUZZ_ENGINE})
    else()
        target_compile_options(${target} PRIVATE -fsanitize=fuzzer)
        target_link_options(${target} PRIVATE -fsanitize=fuzzer)
    endif()
endfunction()

add_fuzzer(fuzz_nbt_reader fuzz_nbt_reader.cpp)
target_link_libraries(fuzz_nbt_reader PRIVATE nbt++)

# Disabled: Qt6 adds unnecessary glib dependency in fuzzing environment
# add_fuzzer(fuzz_qjson_parse fuzz_qjson_parse.cpp)
# target_link_libraries(fuzz_qjson_parse PRIVATE Qt6::Core)

# fuzz_gzip requires Qt6::Core for QByteArray/QFile usage in GZip.cpp
# Only build if Qt6::Core is available
if(TARGET Qt6::Core)
    add_fuzzer(fuzz_gzip fuzz_gzip.cpp ${PROJECT_SOURCE_DIR}/launcher/GZip.cpp)
    target_include_directories(fuzz_gzip PRIVATE ${PROJECT_SOURCE_DIR}/launcher)
    # Use static ptlibzippy for fuzzer to avoid sanitizer symbol mismatch
    if(TARGET ptlibzippystatic)
        target_link_libraries(fuzz_gzip PRIVATE Qt6::Core ptlibzippystatic)
    else()
        target_link_libraries(fuzz_gzip PRIVATE Qt6::Core PTlibzippy::PTlibzippy)
    endif()

    add_fuzzer(fuzz_separator_prefix_tree fuzz_separator_prefix_tree.cpp)
    target_include_directories(fuzz_separator_prefix_tree PRIVATE ${PROJECT_SOURCE_DIR}/launcher)
    target_link_libraries(fuzz_separator_prefix_tree PRIVATE Qt6::Core)
endif()
