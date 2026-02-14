cmake_minimum_required(VERSION 3.15)
project(javacheck VERSION 0.0.5 LANGUAGES Java)
find_package(Java 1.8 REQUIRED COMPONENTS Development)

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT org.projecttick.javacheck.JavaCheck)
if(Java_VERSION_STRING VERSION_LESS 9)
    set(_PROJT_JAVA_RELEASE_FLAGS -target 8 -source 8)
else()
    set(_PROJT_JAVA_RELEASE_FLAGS --release 8)
endif()
set(CMAKE_JAVA_COMPILE_FLAGS ${_PROJT_JAVA_RELEASE_FLAGS} -Xlint:deprecation -Xlint:unchecked)
unset(_PROJT_JAVA_RELEASE_FLAGS)
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
