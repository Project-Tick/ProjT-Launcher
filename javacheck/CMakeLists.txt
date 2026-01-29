cmake_minimum_required(VERSION 3.15)
project(javacheck VERSION 0.0.5 LANGUAGES Java)
find_package(Java 1.8 REQUIRED COMPONENTS Development)

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT org.projecttick.javacheck.JavaCheck)
set(CMAKE_JAVA_COMPILE_FLAGS -target 8 -source 8 -Xlint:deprecation -Xlint:unchecked)
set(CMAKE_JAVA_TARGET_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/jars")
file(MAKE_DIRECTORY "${CMAKE_JAVA_TARGET_OUTPUT_DIR}")

# Set default JARS_DEST_DIR if not already defined by parent CMakeLists.txt
if(NOT DEFINED JARS_DEST_DIR)
    set(JARS_DEST_DIR "${CMAKE_BINARY_DIR}/jars")
endif()

set(SRC
    src/main/java/org/projecttick/javacheck/JavaCheck.java
)

add_jar(JavaCheck ${SRC})
install_jar(JavaCheck "${JARS_DEST_DIR}")
