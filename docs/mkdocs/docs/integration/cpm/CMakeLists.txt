cmake_minimum_required(VERSION 3.15)
project(json_example)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:nlohmann/json@0.0.5-1")

add_executable(json_example example.cpp)
target_link_libraries(json_example PRIVATE nlohmann_json::nlohmann_json)
