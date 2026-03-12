cmake_minimum_required(VERSION 3.22)  # minimum version required by Qt

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()

project(Launcher)

# Force bzip2 to build static library (required for quazip)
set(ENABLE_STATIC_LIB ON CACHE BOOL "Enable static lib for bzip2" FORCE)

# Set default build type for single-config generators only.
if(CMAKE_CONFIGURATION_TYPES)
    if(DEFINED CMAKE_BUILD_TYPE)
        unset(CMAKE_BUILD_TYPE CACHE)
    endif()
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    endif()
endif()

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BUILD_DIR}" IS_IN_SOURCE_BUILD)
if(IS_IN_SOURCE_BUILD)
    message(FATAL_ERROR "You are building the Launcher in-source. Please separate the build tree from the source tree.")
endif()



##################################### Set CMake options ####################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

# Output layout
set(Launcher_OUTPUT_ROOT "${PROJECT_BINARY_DIR}/out" CACHE PATH "Root output directory for build artifacts")
if(DEFINED ENV{GITHUB_ACTIONS})
    file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}" _projt_src_dir)
    file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}" _projt_bin_dir)
    string(REGEX REPLACE "([][+.*^$()\\\\|?])" "\\\\\\1" _projt_src_dir_esc "${_projt_src_dir}")
    string(REGEX REPLACE "([][+.*^$()\\\\|?])" "\\\\\\1" _projt_bin_dir_esc "${_projt_bin_dir}")
    if(NOT Launcher_OUTPUT_ROOT MATCHES "^${_projt_src_dir_esc}(/|$)"
       AND NOT Launcher_OUTPUT_ROOT MATCHES "^${_projt_bin_dir_esc}(/|$)")
        set(Launcher_OUTPUT_ROOT "${PROJECT_BINARY_DIR}/out" CACHE PATH "Root output directory for build artifacts" FORCE)
    endif()
    unset(_projt_src_dir)
    unset(_projt_bin_dir)
    unset(_projt_src_dir_esc)
    unset(_projt_bin_dir_esc)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${Launcher_OUTPUT_ROOT}/root/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${Launcher_OUTPUT_ROOT}/root/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${Launcher_OUTPUT_ROOT}/root/$<CONFIG>")
set(CMAKE_JAVA_TARGET_OUTPUT_DIR "${Launcher_OUTPUT_ROOT}/root/jars")

macro(projt_push_output_dirs name)
    set(_PROJT_PREV_RUNTIME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(_PROJT_PREV_LIBRARY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    set(_PROJT_PREV_ARCHIVE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    set(_PROJT_PREV_JAVA "${CMAKE_JAVA_TARGET_OUTPUT_DIR}")
    set(_projt_dir "${Launcher_OUTPUT_ROOT}/${name}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${_projt_dir}/$<CONFIG>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${_projt_dir}/$<CONFIG>")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${_projt_dir}/$<CONFIG>")
    set(CMAKE_JAVA_TARGET_OUTPUT_DIR "${_projt_dir}/jars")
endmacro()

macro(projt_pop_output_dirs)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${_PROJT_PREV_RUNTIME}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${_PROJT_PREV_LIBRARY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${_PROJT_PREV_ARCHIVE}")
    set(CMAKE_JAVA_TARGET_OUTPUT_DIR "${_PROJT_PREV_JAVA}")
    unset(_projt_dir)
endmacro()

macro(projt_push_install_libdir new_libdir)
    set(_PROJT_PREV_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
    set(_PROJT_PREV_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_FULL_LIBDIR}")
    set(CMAKE_INSTALL_LIBDIR "${new_libdir}")
    if(IS_ABSOLUTE "${new_libdir}")
        set(CMAKE_INSTALL_FULL_LIBDIR "${new_libdir}")
    else()
        set(CMAKE_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${new_libdir}")
    endif()
endmacro()

macro(projt_pop_install_libdir)
    set(CMAKE_INSTALL_LIBDIR "${_PROJT_PREV_INSTALL_LIBDIR}")
    set(CMAKE_INSTALL_FULL_LIBDIR "${_PROJT_PREV_INSTALL_FULL_LIBDIR}")
    unset(_PROJT_PREV_INSTALL_LIBDIR)
    unset(_PROJT_PREV_INSTALL_FULL_LIBDIR)
endmacro()

macro(projt_push_install_includedir new_includedir)
    set(_PROJT_PREV_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
    set(_PROJT_PREV_INSTALL_FULL_INCLUDEDIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
    set(CMAKE_INSTALL_INCLUDEDIR "${new_includedir}")
    if(IS_ABSOLUTE "${new_includedir}")
        set(CMAKE_INSTALL_FULL_INCLUDEDIR "${new_includedir}")
    else()
        set(CMAKE_INSTALL_FULL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${new_includedir}")
    endif()
endmacro()

macro(projt_pop_install_includedir)
    set(CMAKE_INSTALL_INCLUDEDIR "${_PROJT_PREV_INSTALL_INCLUDEDIR}")
    set(CMAKE_INSTALL_FULL_INCLUDEDIR "${_PROJT_PREV_INSTALL_FULL_INCLUDEDIR}")
    unset(_PROJT_PREV_INSTALL_INCLUDEDIR)
    unset(_PROJT_PREV_INSTALL_FULL_INCLUDEDIR)
endmacro()

macro(projt_push_install_libexecdir new_libexecdir)
    set(_PROJT_PREV_INSTALL_LIBEXECDIR "${CMAKE_INSTALL_LIBEXECDIR}")
    set(_PROJT_PREV_INSTALL_FULL_LIBEXECDIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}")
    set(CMAKE_INSTALL_LIBEXECDIR "${new_libexecdir}")
    if(IS_ABSOLUTE "${new_libexecdir}")
        set(CMAKE_INSTALL_FULL_LIBEXECDIR "${new_libexecdir}")
    else()
        set(CMAKE_INSTALL_FULL_LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${new_libexecdir}")
    endif()
endmacro()

macro(projt_pop_install_libexecdir)
    set(CMAKE_INSTALL_LIBEXECDIR "${_PROJT_PREV_INSTALL_LIBEXECDIR}")
    set(CMAKE_INSTALL_FULL_LIBEXECDIR "${_PROJT_PREV_INSTALL_FULL_LIBEXECDIR}")
    unset(_PROJT_PREV_INSTALL_LIBEXECDIR)
    unset(_PROJT_PREV_INSTALL_FULL_LIBEXECDIR)
endmacro()

macro(projt_push_autogen_disabled)
    set(_PROJT_PREV_AUTOMOC "${CMAKE_AUTOMOC}")
    set(_PROJT_PREV_AUTORCC "${CMAKE_AUTORCC}")
    set(_PROJT_PREV_AUTOUIC "${CMAKE_AUTOUIC}")
    set(CMAKE_AUTOMOC OFF)
    set(CMAKE_AUTORCC OFF)
    set(CMAKE_AUTOUIC OFF)
endmacro()

macro(projt_pop_autogen_disabled)
    set(CMAKE_AUTOMOC "${_PROJT_PREV_AUTOMOC}")
    set(CMAKE_AUTORCC "${_PROJT_PREV_AUTORCC}")
    set(CMAKE_AUTOUIC "${_PROJT_PREV_AUTOUIC}")
    unset(_PROJT_PREV_AUTOMOC)
    unset(_PROJT_PREV_AUTORCC)
    unset(_PROJT_PREV_AUTOUIC)
endmacro()

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    set(_launcher_default_bundled_libdir "${CMAKE_INSTALL_LIBDIR}/projtlauncher")
    set(Launcher_BUNDLED_LIBDIR "${_launcher_default_bundled_libdir}" CACHE STRING "Install directory for bundled libraries (relative to prefix). Set empty to disable.")
    set(_launcher_default_bundled_includedir "include/projtlauncher")
    set(Launcher_BUNDLED_INCLUDEDIR "${_launcher_default_bundled_includedir}" CACHE STRING "Install directory for bundled headers (relative to prefix). Set empty to disable.")
    set(_launcher_default_bundled_libexecdir "libexec/projtlauncher")
    set(Launcher_BUNDLED_LIBEXECDIR "${_launcher_default_bundled_libexecdir}" CACHE STRING "Install directory for bundled libexec tools (relative to prefix). Set empty to disable.")
    set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME Runtime)
    unset(_launcher_default_bundled_libdir)
    unset(_launcher_default_bundled_includedir)
    unset(_launcher_default_bundled_libexecdir)
endif()

######## Set compiler flags ########
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 23)

if(MSVC)
set(CMAKE_C_STANDARD 11)
else()
set(CMAKE_C_STANDARD 23)
endif()

include(GenerateExportHeader)
if(MSVC)
    # /GS Adds buffer security checks, default on but incuded anyway to mirror gcc's fstack-protector flag
    # /permissive- specify standards-conforming compiler behavior, also enabled by Qt6, default on with std:c++20
    # Use /W4 as /Wall includes unnesserey warnings such as added padding to structs
    set(CMAKE_CXX_FLAGS "/GS /permissive- /W4 ${CMAKE_CXX_FLAGS}")

    # /EHs Enables stack unwind semantics for standard C++ exceptions to ensure stackframes are unwound
    # and object deconstructors are called when an exception is caught.
    # without it memory leaks and a warning is printed
    # /EHc tells the compiler to assume that functions declared as extern "C" never throw a C++ exception
    # This appears to not always be a defualt compiler option in CMAKE
    set(CMAKE_CXX_FLAGS "/EHsc ${CMAKE_CXX_FLAGS}")

    # LINK accepts /SUBSYSTEM whics sets if we are a WINDOWS (gui) or a CONSOLE programs
    # This implicitly selects an entrypoint specific to the subsystem selected
    # qtmain/QtEntryPointLib provides the correct entrypoint (wWinMain) for gui programs
    # Additinaly LINK autodetects we use a GUI so we can omit /SUBSYSTEM
    # This allows tests to still use have console without using seperate linker flags
    # /LTCG allows for linking wholy optimizated programs
    # /MANIFEST:NO disables generating a manifest file, we instead provide our own
    # /STACK sets the stack reserve size, ATL's pack list needs 3-4 MiB as of November 2022, provide 8 MiB
    set(CMAKE_EXE_LINKER_FLAGS "/LTCG /MANIFEST:NO /STACK:8388608 ${CMAKE_EXE_LINKER_FLAGS}")

    # /GL enables whole program optimizations
    # /Gw helps reduce binary size
    # /Gy allows the compiler to package individual functions
    # /guard:cf enables control flow guard
    foreach(lang C CXX)
        set("CMAKE_${lang}_FLAGS_RELEASE" "/GL /Gw /Gy /guard:cf")
    endforeach()

    # See https://github.com/ccache/ccache/issues/1040
    # Note, CMake 3.25 replaces this with CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
    # See https://cmake.org/cmake/help/v3.25/variable/CMAKE_MSVC_DEBUG_INFORMATION_FORMAT.html
    foreach(config DEBUG RELWITHDEBINFO)
        foreach(lang C CXX)
            set(flags_var "CMAKE_${lang}_FLAGS_${config}")
            string(REGEX REPLACE "/Z[Ii]" "/Z7" ${flags_var} "${${flags_var}}")
        endforeach()
    endforeach()

    if(CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDLL")
        set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release "")
        set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release "")
    endif()
else()
    set(CMAKE_CXX_FLAGS "-Wall -pedantic -fstack-protector-strong --param=ssp-buffer-size=4 ${CMAKE_CXX_FLAGS}")

    # ATL's pack list needs more than the default 1 Mib stack on windows
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,8388608 ${CMAKE_EXE_LINKER_FLAGS}")

        # Emit PDBs for WinDbg, etc.
        if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            set(CMAKE_EXE_LINKER_FLAGS "-Wl,--pdb= ${CMAKE_EXE_LINKER_FLAGS}")

            foreach(lang C CXX)
                set("CMAKE_${lang}_FLAGS" "-gcodeview ${CMAKE_${lang}_FLAGS}")

                # Force-enabling this to use generator expressions like TARGET_PDB_FILE
                # (and because we can actually emit PDBs)
                set("CMAKE_${lang}_LINKER_SUPPORTS_PDB" ON)
            endforeach()
        endif()

        # -ffunction-sections and -fdata-sections help reduce binary size
        # -mguard=cf enables Control Flow Guard
        # -Wl,--gc-sections removes unused sections during linking
        foreach(lang C CXX)
            set("CMAKE_${lang}_FLAGS_RELEASE" "-ffunction-sections -fdata-sections -mguard=cf")
        endforeach()
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-Wl,--gc-sections ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    elseif(APPLE)
        # macOS specific dead code stripping
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,-dead_strip ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-Wl,-dead_strip ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    elseif(UNIX)
        # Linux/BSD dead code stripping
        foreach(lang C CXX)
            set("CMAKE_${lang}_FLAGS_RELEASE" "-ffunction-sections -fdata-sections")
        endforeach()
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-Wl,--gc-sections ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_WARN_DEPRECATED_UP_TO=0x060200")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DISABLE_DEPRECATED_UP_TO=0x060000")

# Fix aarch64 build for toml++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOML_ENABLE_FLOAT16=0")

# set CXXFLAGS for build targets
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -D_FORTIFY_SOURCE=2 ${CMAKE_CXX_FLAGS_RELEASE}")

# Export compile commands for debug builds if we can (useful in LSPs like clangd)
# https://cmake.org/cmake/help/v3.31/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
if(CMAKE_GENERATOR STREQUAL "Unix Makefiles" OR CMAKE_GENERATOR MATCHES "^Ninja")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

if(UNIX AND NOT APPLE)
    # Keep install RPATHs relative so RPM check-rpaths doesn't reject build-root paths.
    set(_launcher_rpath_entries "")
    if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
        list(APPEND _launcher_rpath_entries "$ORIGIN/../${Launcher_BUNDLED_LIBDIR}")
    endif()
    list(APPEND _launcher_rpath_entries "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}" "$ORIGIN")
    string(JOIN ":" CMAKE_INSTALL_RPATH ${_launcher_rpath_entries})
    unset(_launcher_rpath_entries)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
endif()

option(DEBUG_ADDRESS_SANITIZER "Enable Address Sanitizer in Debug builds" OFF)

# If this is a Debug build turn on address sanitiser
if ((CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND DEBUG_ADDRESS_SANITIZER)
    message(STATUS "Address Sanitizer enabled for Debug builds, Turn it off with -DDEBUG_ADDRESS_SANITIZER=off")
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
            # using clang with clang-cl front end
            message(STATUS "Address Sanitizer available on Clang MSVC frontend")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /Oy-")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address /Oy-")
        else()
            # AppleClang and Clang
            message(STATUS "Address Sanitizer available on Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=null")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=null")
        endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # GCC
        message(STATUS "Address Sanitizer available on GCC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover")
        link_libraries("asan")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message(STATUS "Address Sanitizer available on MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /Oy-")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address /Oy-")
    else()
        message(STATUS "Address Sanitizer not available on compiler ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()


option(ENABLE_LTO "Enable Link Time Optimization" off)

if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
        if(CMAKE_BUILD_TYPE)
            if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
                message(STATUS "IPO / LTO enabled")
            else()
                message(STATUS "Not enabling IPO / LTO on debug builds")
            endif()
        else()
            message(STATUS "IPO / LTO will only be enabled for release builds")
        endif()
    else()
        message(STATUS "IPO / LTO not supported: <${ipo_error}>")
    endif()
endif()

option(BUILD_TESTING "Build the testing tree." ON)
option(BUILD_FUZZERS "Build fuzzing targets." OFF)
option(Launcher_ALLOW_FETCHCONTENT "Allow FetchContent fallback for tomlplusplus" OFF)
option(Launcher_ALLOW_ECM_FETCHCONTENT "Allow FetchContent fallback for ECM" OFF)
option(Launcher_SHOW_CMAKE_WARNINGS "Show CMake developer/deprecated warnings from dependencies" ON)

include(cmake/versions.cmake)

if(Launcher_SHOW_CMAKE_WARNINGS)
    set(CMAKE_WARN_DEPRECATED ON)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF)
endif()

macro(launcher_fetchcontent_makeavailable content_name)
    if(Launcher_SHOW_CMAKE_WARNINGS)
        FetchContent_MakeAvailable(${content_name})
    else()
        set(launcher_warn_deprecated_set FALSE)
        if(DEFINED CMAKE_WARN_DEPRECATED)
            set(launcher_warn_deprecated_value "${CMAKE_WARN_DEPRECATED}")
            set(launcher_warn_deprecated_set TRUE)
        endif()
        set(launcher_suppress_dev_set FALSE)
        if(DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
            set(launcher_suppress_dev_value "${CMAKE_SUPPRESS_DEVELOPER_WARNINGS}")
            set(launcher_suppress_dev_set TRUE)
        endif()

        set(CMAKE_WARN_DEPRECATED OFF)
        set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
        FetchContent_MakeAvailable(${content_name})

        if(launcher_warn_deprecated_set)
            set(CMAKE_WARN_DEPRECATED "${launcher_warn_deprecated_value}")
        else()
            unset(CMAKE_WARN_DEPRECATED)
        endif()
        if(launcher_suppress_dev_set)
            set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS "${launcher_suppress_dev_value}")
        else()
            unset(CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
        endif()
        unset(launcher_warn_deprecated_set)
        unset(launcher_warn_deprecated_value)
        unset(launcher_suppress_dev_set)
        unset(launcher_suppress_dev_value)
    endif()
endmacro()

# extra-cmake-modules (ECM) - Using bundled fork
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extra-cmake-modules/modules")
    message(STATUS "Using bundled extra-cmake-modules")
    set(ECM_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extra-cmake-modules/modules")
    set(ECM_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extra-cmake-modules/modules")
    # Add kde-modules for KDEInstallDirs and other KDE modules
    list(APPEND ECM_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extra-cmake-modules/kde-modules")
else()
    message(FATAL_ERROR "ECM not found. extra-cmake-modules/ directory is missing.")
endif()
set(CMAKE_MODULE_PATH "${ECM_MODULE_PATH};${CMAKE_MODULE_PATH}")
include(CTest)
include(ECMAddTests)
if(BUILD_TESTING)
    enable_testing()
endif()

##################################### Set Application options #####################################

######## Set URLs ########
set(Launcher_NEWS_RSS_URL "https://projecttick.org/product/projt-launcher/feed.xml" CACHE STRING "URL to fetch ProjT Launcher's news RSS feed from.")
set(Launcher_NEWS_OPEN_URL "https://projecttick.org/product/projt-launcher/news" CACHE STRING "URL that gets opened when the user clicks 'More News'")
set(Launcher_HELP_URL "https://projecttick.org/handbook/help-pages/%1" CACHE STRING "URL (with arg %1 to be substituted with page-id) that gets opened when the user requests help")
set(Launcher_HUB_HOME_URL "https://projecttick.org/p/projt-launcher/" CACHE STRING "Launcher Hub home URL")
set(Launcher_HUB_COMMUNITY_URL "https://projecttick.org/projtlauncher/discord" CACHE STRING "Launcher Hub community URL")
set(Launcher_HUB_SEARCH_URL "https://www.google.com/search?q=%1" CACHE STRING "Search URL (with %1 placeholder) used by the Launcher Hub address bar")
set(Launcher_LOGIN_CALLBACK_URL "https://projecttick.org/p/projt-launcher" CACHE STRING "URL that gets opened when the user successfully logins.")
set(Launcher_FMLLIBS_BASE_URL "https://files.projecttick.org/fmllibs/" CACHE STRING "URL for FML Libraries.")

######## Set version numbers ########
set(Launcher_VERSION_MAJOR 0)
set(Launcher_VERSION_MINOR 0)
set(Launcher_VERSION_PATCH 5)
set(Launcher_VERSION_TWEAK 1)

set(Launcher_VERSION_NAME "${Launcher_VERSION_MAJOR}.${Launcher_VERSION_MINOR}.${Launcher_VERSION_PATCH}.${Launcher_VERSION_TWEAK}")
set(Launcher_VERSION_NAME4 "${Launcher_VERSION_MAJOR}.${Launcher_VERSION_MINOR}.${Launcher_VERSION_PATCH}.${Launcher_VERSION_TWEAK}")
set(Launcher_VERSION_NAME4_COMMA "${Launcher_VERSION_MAJOR},${Launcher_VERSION_MINOR},${Launcher_VERSION_PATCH},${Launcher_VERSION_TWEAK}")

# Build platform.
set(Launcher_BUILD_PLATFORM "unknown" CACHE STRING "A short string identifying the platform that this build was built for. Only used to display in the about dialog.")

# CI fuzz builds use a reduced target set; skip heavy components and installers when enabled.
option(LAUNCHER_FUZZ_ONLY "Build only fuzzing targets (skips installers, Java launcher, and desktop integration)" OFF)

# Optional bundled toolchain / Qt builds (off by default).
option(Launcher_ENABLE_BUNDLED_QT_TARGETS "Expose Qt build targets using sources under qt/" OFF)
option(Launcher_ENABLE_BUNDLED_LLVM_TARGETS "Expose LLVM/Clang build targets using sources under toolchain/llvm" OFF)
option(Launcher_ENABLE_BUNDLED_GCC_TARGETS "Expose GCC build targets using sources under toolchain/gcc" OFF)
set(Launcher_BUNDLED_BUILD_ROOT "${PROJECT_BINARY_DIR}/bundled-build" CACHE PATH "Build root for bundled Qt/toolchain targets")
set(Launcher_BUNDLED_INSTALL_ROOT "${PROJECT_BINARY_DIR}/bundled-install" CACHE PATH "Install root for bundled Qt/toolchain targets")
set(Launcher_BUNDLED_BUILD_CONFIG "Release" CACHE STRING "Build configuration for bundled Qt/toolchain targets")
set_property(CACHE Launcher_BUNDLED_BUILD_CONFIG PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
set(Launcher_BUNDLED_GCC_MAKE_PROGRAM "make" CACHE STRING "Make program used for bundled GCC build")

set(_projt_bundled_targets_enabled FALSE)
if(Launcher_ENABLE_BUNDLED_QT_TARGETS OR Launcher_ENABLE_BUNDLED_LLVM_TARGETS OR Launcher_ENABLE_BUNDLED_GCC_TARGETS)
    set(_projt_bundled_targets_enabled TRUE)
endif()

if(_projt_bundled_targets_enabled)
    include(ProcessorCount)
    ProcessorCount(_projt_host_cpu_count)
    if(NOT _projt_host_cpu_count OR _projt_host_cpu_count LESS 1)
        set(_projt_host_cpu_count 1)
    endif()
endif()

if(Launcher_ENABLE_BUNDLED_QT_TARGETS)
    set(_projt_qtbase_src "${CMAKE_CURRENT_SOURCE_DIR}/qt/qtbase")
    set(_projt_qtbase_build "${Launcher_BUNDLED_BUILD_ROOT}/qtbase")
    set(_projt_qtbase_install "${Launcher_BUNDLED_INSTALL_ROOT}/qt")
    if(EXISTS "${_projt_qtbase_src}/CMakeLists.txt")
        add_custom_target(qt-bundled-configure
            COMMAND "${CMAKE_COMMAND}" -S "${_projt_qtbase_src}" -B "${_projt_qtbase_build}"
                -G "${CMAKE_GENERATOR}"
                -DCMAKE_INSTALL_PREFIX="${_projt_qtbase_install}"
                -DCMAKE_PREFIX_PATH="${_projt_qtbase_install}"
                -DQT_BUILD_TESTS=OFF
                -DQT_BUILD_EXAMPLES=OFF
            COMMENT "Configuring bundled Qt (qtbase) from ${_projt_qtbase_src}"
            USES_TERMINAL
        )
        add_custom_target(qt-bundled-build
            COMMAND "${CMAKE_COMMAND}" --build "${_projt_qtbase_build}" --config "${Launcher_BUNDLED_BUILD_CONFIG}" --parallel "${_projt_host_cpu_count}"
            DEPENDS qt-bundled-configure
            COMMENT "Building bundled Qt (qtbase)"
            USES_TERMINAL
        )
        add_custom_target(qt-bundled-install
            COMMAND "${CMAKE_COMMAND}" --build "${_projt_qtbase_build}" --target install --config "${Launcher_BUNDLED_BUILD_CONFIG}" --parallel "${_projt_host_cpu_count}"
            DEPENDS qt-bundled-build
            COMMENT "Installing bundled Qt into ${_projt_qtbase_install}"
            USES_TERMINAL
        )
    else()
        message(WARNING "Bundled Qt sources not found at ${_projt_qtbase_src}; qt-bundled-* targets will not be created.")
    endif()
endif()

if(Launcher_ENABLE_BUNDLED_LLVM_TARGETS)
    set(_projt_llvm_src "${CMAKE_CURRENT_SOURCE_DIR}/toolchain/llvm/llvm")
    set(_projt_llvm_build "${Launcher_BUNDLED_BUILD_ROOT}/llvm")
    set(_projt_llvm_install "${Launcher_BUNDLED_INSTALL_ROOT}/llvm")
    if(EXISTS "${_projt_llvm_src}/CMakeLists.txt")
        add_custom_target(toolchain-llvm-configure
            COMMAND "${CMAKE_COMMAND}" -S "${_projt_llvm_src}" -B "${_projt_llvm_build}"
                -G "${CMAKE_GENERATOR}"
                -DCMAKE_INSTALL_PREFIX="${_projt_llvm_install}"
                "-DLLVM_ENABLE_PROJECTS='clang;clang-tools-extra;lld'"
                -DLLVM_INCLUDE_TESTS=OFF
                -DLLVM_INCLUDE_BENCHMARKS=OFF
                -DLLVM_INCLUDE_EXAMPLES=OFF
            COMMENT "Configuring bundled LLVM/Clang from ${_projt_llvm_src}"
            USES_TERMINAL
        )
        add_custom_target(toolchain-llvm-build
            COMMAND "${CMAKE_COMMAND}" --build "${_projt_llvm_build}" --config "${Launcher_BUNDLED_BUILD_CONFIG}" --parallel "${_projt_host_cpu_count}"
            DEPENDS toolchain-llvm-configure
            COMMENT "Building bundled LLVM/Clang"
            USES_TERMINAL
        )
        add_custom_target(toolchain-llvm-install
            COMMAND "${CMAKE_COMMAND}" --build "${_projt_llvm_build}" --target install --config "${Launcher_BUNDLED_BUILD_CONFIG}" --parallel "${_projt_host_cpu_count}"
            DEPENDS toolchain-llvm-build
            COMMENT "Installing bundled LLVM/Clang into ${_projt_llvm_install}"
            USES_TERMINAL
        )
    else()
        message(WARNING "Bundled LLVM sources not found at ${_projt_llvm_src}; toolchain-llvm-* targets will not be created.")
    endif()
endif()

if(Launcher_ENABLE_BUNDLED_GCC_TARGETS)
    if(WIN32)
        message(WARNING "Bundled GCC targets are disabled on Windows in this CMake setup.")
    else()
        set(_projt_gcc_src "${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc")
        set(_projt_gcc_build "${Launcher_BUNDLED_BUILD_ROOT}/gcc")
        set(_projt_gcc_install "${Launcher_BUNDLED_INSTALL_ROOT}/gcc")
        if(EXISTS "${_projt_gcc_src}/configure")
            add_custom_target(toolchain-gcc-configure
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${_projt_gcc_build}"
                COMMAND "${_projt_gcc_src}/configure" --prefix="${_projt_gcc_install}" --disable-multilib --enable-languages=c,c++
                WORKING_DIRECTORY "${_projt_gcc_build}"
                COMMENT "Configuring bundled GCC from ${_projt_gcc_src}"
                USES_TERMINAL
            )
            add_custom_target(toolchain-gcc-build
                COMMAND "${Launcher_BUNDLED_GCC_MAKE_PROGRAM}" -j"${_projt_host_cpu_count}"
                WORKING_DIRECTORY "${_projt_gcc_build}"
                DEPENDS toolchain-gcc-configure
                COMMENT "Building bundled GCC"
                USES_TERMINAL
            )
            add_custom_target(toolchain-gcc-install
                COMMAND "${Launcher_BUNDLED_GCC_MAKE_PROGRAM}" install
                WORKING_DIRECTORY "${_projt_gcc_build}"
                DEPENDS toolchain-gcc-build
                COMMENT "Installing bundled GCC into ${_projt_gcc_install}"
                USES_TERMINAL
            )
        else()
            message(WARNING "Bundled GCC sources not found at ${_projt_gcc_src}; toolchain-gcc-* targets will not be created.")
        endif()
    endif()
endif()

if(TARGET qt-bundled-install OR TARGET toolchain-llvm-install OR TARGET toolchain-gcc-install)
    add_custom_target(bundled-all)
    if(TARGET qt-bundled-install)
        add_dependencies(bundled-all qt-bundled-install)
    endif()
    if(TARGET toolchain-llvm-install)
        add_dependencies(bundled-all toolchain-llvm-install)
    endif()
    if(TARGET toolchain-gcc-install)
        add_dependencies(bundled-all toolchain-gcc-install)
    endif()
endif()

# Release feed URL for updater
set(Launcher_UPDATER_RELEASES_URL "https://projecttick.org/product/projt-launcher/feed.xml" CACHE STRING "URL for the updater release feed manifest.")

# Name to help updater identify valid artifacts
set(Launcher_BUILD_ARTIFACT "" CACHE STRING "Artifact name to help the updater identify valid artifacts.")

# The metadata server
set(Launcher_META_URL "https://meta.projecttick.org/" CACHE STRING "URL to fetch Launcher's meta files from.")

# Imgur API Client ID
set(Launcher_IMGUR_CLIENT_ID "5b97b0713fba4a3" CACHE STRING "Client ID you can get from Imgur when you register an application")

# Bug tracker URL
set(Launcher_BUG_TRACKER_URL "https://github.com/Project-Tick/ProjT-Launcher/issues" CACHE STRING "URL for the bug tracker.")

# Translations Platform URL
set(Launcher_TRANSLATIONS_URL "https://crowdin.com/project/projtlauncher" CACHE STRING "URL for the translations platform.")
set(Launcher_TRANSLATION_FILES_URL "https://i18n.projecttick.org/" CACHE STRING "URL for the translations files.")

# Matrix Space
set(Launcher_MATRIX_URL "https://projecttick.org/projtlauncher/matrix" CACHE STRING "URL to the Matrix Space")

# Discord URL
set(Launcher_DISCORD_URL "https://projecttick.org/projtlauncher/discord" CACHE STRING "URL for the Discord guild.")

# Subreddit URL
set(Launcher_SUBREDDIT_URL "https:/projecttick.org/projtlauncher/reddit" CACHE STRING "URL for the subreddit.")

# Builds
# Builds
set(Launcher_QT_VERSION_MAJOR "6" CACHE STRING "Major Qt version to build against")

# Java downloader
set(Launcher_ENABLE_JAVA_DOWNLOADER_DEFAULT ON)

# Although we recommend enabling this, we cannot guarantee binary compatibility on
# differing Linux/BSD/etc distributions. Downstream packagers should be explicitly opt-ing into this
# feature if they know it will work with their distribution.
if(UNIX AND NOT APPLE)
    set(Launcher_ENABLE_JAVA_DOWNLOADER_DEFAULT OFF)
endif()

# Java downloader
option(Launcher_ENABLE_JAVA_DOWNLOADER "Build the java downloader feature" ${Launcher_ENABLE_JAVA_DOWNLOADER_DEFAULT})

# Native libraries
if(UNIX AND APPLE)
    set(Launcher_GLFW_LIBRARY_NAME "libglfw.dylib" CACHE STRING "Name of native glfw library")
    set(Launcher_OPENAL_LIBRARY_NAME "libopenal.dylib" CACHE STRING "Name of native openal library")
elseif(UNIX)
    set(Launcher_GLFW_LIBRARY_NAME "libglfw.so" CACHE STRING "Name of native glfw library")
    set(Launcher_OPENAL_LIBRARY_NAME "libopenal.so" CACHE STRING "Name of native openal library")
elseif(WIN32)
    set(Launcher_GLFW_LIBRARY_NAME "glfw.dll" CACHE STRING "Name of native glfw library")
    set(Launcher_OPENAL_LIBRARY_NAME "OpenAL.dll" CACHE STRING "Name of native openal library")
endif()

# API Keys
# NOTE: These API keys are here for convenience. If you rebrand this software or intend to break the terms of service
# of these platforms, please change these API keys beforehand.
# Be aware that if you were to use these API keys for malicious purposes they might get revoked, which might cause
# breakage to thousands of users.
# If you don't plan to use these features of this software, you can just remove these values.

# By using this key in your builds you accept the terms of use laid down in
# https://docs.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use
set(Launcher_MSA_CLIENT_ID "3035382c-8f73-493a-b579-d182905c2864" CACHE STRING "Client ID you can get from Microsoft Identity Platform when you register an application")

# By using this key in your builds you accept the terms and conditions laid down in
# https://support.curseforge.com/en/support/solutions/articles/9000207405-curse-forge-3rd-party-api-terms-and-conditions
# NOTE: CurseForge requires you to change this if you make any kind of derivative work.
# This key was issued specifically for ProjT Launcher
set(Launcher_CURSEFORGE_API_KEY "$2a$10$S7KcKijbCj8mCHUQcn0tgOmtHg0kA8q9FI0niNJJ7knPq0INomzrG" CACHE STRING "API key for the CurseForge platform")

set(Launcher_COMPILER_NAME ${CMAKE_CXX_COMPILER_ID})
set(Launcher_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})
set(Launcher_COMPILER_TARGET_SYSTEM ${CMAKE_SYSTEM_NAME})
set(Launcher_COMPILER_TARGET_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
set(Launcher_COMPILER_TARGET_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})

#### Check the current Git commit and branch
include(GetGitRevisionDescription)
git_get_exact_tag(Launcher_GIT_TAG)
get_git_head_revision(Launcher_GIT_REFSPEC Launcher_GIT_COMMIT)

message(STATUS "Git commit: ${Launcher_GIT_COMMIT}")
message(STATUS "Git tag: ${Launcher_GIT_TAG}")
message(STATUS "Git refspec: ${Launcher_GIT_REFSPEC}")

string(TIMESTAMP TODAY "%Y-%m-%d")
set(Launcher_BUILD_TIMESTAMP "${TODAY}")

################################ 3rd Party Libs ################################

if(WIN32 AND NOT MINGW)
    # Detect NuGet architecture
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|arm64")
        set(NUGET_ARCH "arm64")
        set(OSSL_ARCH "arm64")
    else()
        set(NUGET_ARCH "x64")
        set(OSSL_ARCH "x64")
    endif()

    # Note: ZLIB is handled via bundled version only, no NuGet fallback

    # Find OpenSSL from NuGet
    file(GLOB NUGET_OSSL_ROOTS "${CMAKE_SOURCE_DIR}/dependencies/openssl-native*")
    if(NUGET_OSSL_ROOTS)
        list(GET NUGET_OSSL_ROOTS 0 OSSL_ROOT_DIR)
        message(STATUS "Found OpenSSL NuGet: ${OSSL_ROOT_DIR}")
        set(OPENSSL_ROOT_DIR "${OSSL_ROOT_DIR}/build/native" CACHE PATH "" FORCE)
        set(OPENSSL_USE_STATIC_LIBS ON)
        
        file(GLOB_RECURSE OSSL_LIBS "${OSSL_ROOT_DIR}/*.lib")
        foreach(LIB_PATH ${OSSL_LIBS})
            # Strictly match architecture folder to avoid win-x86
            if(LIB_PATH MATCHES "win-${OSSL_ARCH}" OR LIB_PATH MATCHES "/${OSSL_ARCH}/")
                if(LIB_PATH MATCHES "libcrypto.lib$|crypto.lib$")
                    if(NOT LIB_PATH MATCHES "[Dd]ebug" OR NOT OPENSSL_CRYPTO_LIBRARY)
                        set(OPENSSL_CRYPTO_LIBRARY "${LIB_PATH}" CACHE FILEPATH "" FORCE)
                    endif()
                elseif(LIB_PATH MATCHES "libssl.lib$|ssl.lib$")
                    if(NOT LIB_PATH MATCHES "[Dd]ebug" OR NOT OPENSSL_SSL_LIBRARY)
                        set(OPENSSL_SSL_LIBRARY "${LIB_PATH}" CACHE FILEPATH "" FORCE)
                    endif()
                endif()
            endif()
        endforeach()
        
        if(OPENSSL_CRYPTO_LIBRARY AND OPENSSL_SSL_LIBRARY)
            set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include" CACHE PATH "" FORCE)
            set(OPENSSL_LIBRARIES "${OPENSSL_SSL_LIBRARY}" "${OPENSSL_CRYPTO_LIBRARY}" CACHE STRING "" FORCE)
            message(STATUS "Found OpenSSL via NuGet: ${OPENSSL_CRYPTO_LIBRARY}")
            set(OpenSSL_FOUND TRUE CACHE BOOL "" FORCE)
            set(OPENSSL_FOUND TRUE CACHE BOOL "" FORCE)
        endif()
    endif()

    list(APPEND CMAKE_PREFIX_PATH "${PTLIBZIPPY_ROOT}" "${OPENSSL_ROOT_DIR}")
    list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/dependencies")
endif()

# Always use bundled ptlibzippy
set(PTLIBZIPPY_FOUND FALSE)

# System Libraries - Using bundled ptlibzippy only
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/zlib/CMakeLists.txt")
    message(STATUS "Using bundled ptlibzippy")
        set(_projt_prev_skip_install_all "${SKIP_INSTALL_ALL}")
        set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
        # For fuzzing builds, disable shared library to avoid sanitizer symbol conflicts
        if(BUILD_FUZZERS)
            set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
            set(PTLIBZIPPY_BUILD_SHARED OFF CACHE BOOL "" FORCE)
            set(PTLIBZIPPY_BUILD_STATIC ON CACHE BOOL "" FORCE)
            message(STATUS "Fuzzing build: Building ptlibzippy as static library only")
        endif()
        # Disable instrumentation for external dependencies to avoid linker issues
        # IMPORTANT: Save and restore flags to prevent interference with fuzzer compilation
        set(_SAVED_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        set(_SAVED_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(_SAVED_CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        set(_SAVED_CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        
        string(REGEX REPLACE "-fprofile-instr-generate|--coverage|-fprofile-generate|-fprofile-arcs|-ftest-coverage" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        string(REGEX REPLACE "-fprofile-instr-generate|--coverage|-fprofile-generate|-fprofile-arcs|-ftest-coverage" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        if(UNIX AND NOT APPLE)
        # Provide a shared ptlibzippy so libpng and the launcher can use the bundled copy at runtime.
        set(PTLIBZIPPY_BUILD_SHARED ON CACHE BOOL "" FORCE)
        set(PTLIBZIPPY_BUILD_STATIC ON CACHE BOOL "" FORCE)
        endif()
        
        # For OSS-Fuzz: Strip sanitizer flags from zlib build to avoid symbol conflicts
        if(DEFINED ENV{LIB_FUZZING_ENGINE})
            string(REGEX REPLACE "-fsanitize=[^ ]*" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
            string(REGEX REPLACE "-fsanitize-coverage=[^ ]*" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
            string(REGEX REPLACE "-fno-sanitize-recover=[^ ]*" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
            message(STATUS "OSS-Fuzz build: Stripping sanitizer flags from zlib")
        endif()
        
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("zlib")
        projt_push_autogen_disabled()
        add_subdirectory(zlib)
        projt_pop_autogen_disabled()
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
        
        # Restore flags after zlib subdirectory
        set(CMAKE_C_FLAGS "${_SAVED_CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${_SAVED_CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS_DEBUG "${_SAVED_CMAKE_C_FLAGS_DEBUG}")
        set(CMAKE_C_FLAGS_RELEASE "${_SAVED_CMAKE_C_FLAGS_RELEASE}")
        
        if(TARGET ptlibzippystatic)
            set_target_properties(ptlibzippystatic PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        if(TARGET ptlibzippy)
            set_target_properties(ptlibzippy PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()

        # Set PTlibzippy variables for projects that need them
        if(TARGET PTlibzippy::PTlibzippy)
            get_target_property(_projt_ptlibzippy_includes PTlibzippy::PTlibzippy INTERFACE_INCLUDE_DIRECTORIES)
        endif()
        if(_projt_ptlibzippy_includes)
            set(PTLIBZIPPY_INCLUDE_DIR "${_projt_ptlibzippy_includes}")
        elseif(DEFINED PTlibzippy_SOURCE_DIR AND DEFINED PTlibzippy_BINARY_DIR)
            set(PTLIBZIPPY_INCLUDE_DIR "${PTlibzippy_SOURCE_DIR};${PTlibzippy_BINARY_DIR}")
        endif()
        if(PTLIBZIPPY_INCLUDE_DIR)
            set(PTLIBZIPPY_INCLUDE_DIRS "${PTLIBZIPPY_INCLUDE_DIR}")
        endif()
        set(PTLIBZIPPY_LIBRARY "PTlibzippy::PTlibzippy")
        set(PTLIBZIPPY_LIBRARIES "PTlibzippy::PTlibzippy")
        set(PTLIBZIPPY_FOUND TRUE)

        set(PROJT_PTLIBZIPPY_AVAILABLE TRUE)
        unset(_projt_ptlibzippy_includes)
        # Restore libpng install behavior (libpng uses SKIP_INSTALL_ALL)
        set(SKIP_INSTALL_ALL "${_projt_prev_skip_install_all}" CACHE BOOL "" FORCE)
else()
    message(FATAL_ERROR "Bundled ptlibzippy not found in ${CMAKE_CURRENT_SOURCE_DIR}/zlib/")
endif()

if(DEFINED PTlibzippy_SOURCE_DIR AND DEFINED PTlibzippy_BINARY_DIR)
    # Make PTlibzippy headers available to libpng config generation.
    set(ZLIB_INCLUDE_DIRS "${PTlibzippy_SOURCE_DIR};${PTlibzippy_BINARY_DIR}")
elseif(PTLIBZIPPY_INCLUDE_DIRS)
    set(ZLIB_INCLUDE_DIRS "${PTLIBZIPPY_INCLUDE_DIRS}")
endif()

if(NOT LAUNCHER_FUZZ_ONLY AND UNIX AND NOT APPLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libpng/CMakeLists.txt")
        message(STATUS "Using bundled libpng")
        # Ensure libpng install rules are enabled even if other deps toggled them.
        set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)
        set(SKIP_INSTALL_LIBRARIES OFF CACHE BOOL "" FORCE)
        set(PNG_SHARED ON CACHE BOOL "" FORCE)
        set(PNG_STATIC OFF CACHE BOOL "" FORCE)
        # Avoid loading both libpng16d and system libpng16 in Debug builds.
        set(PNG_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
        set(PNG_TESTS OFF CACHE BOOL "" FORCE)
        set(PNG_TOOLS OFF CACHE BOOL "" FORCE)
        set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("libpng")
        projt_push_autogen_disabled()
        add_subdirectory(libpng)
        projt_pop_autogen_disabled()
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
    else()
        message(FATAL_ERROR "Bundled libpng not found in ${CMAKE_CURRENT_SOURCE_DIR}/libpng/")
    endif()
endif()

if(NOT LAUNCHER_FUZZ_ONLY)
    if(NOT OpenSSL_FOUND)
        find_package(OpenSSL REQUIRED)
    endif()
    set(LAUNCHER_WITH_WEBENGINE ON)
    if(MINGW)
        set(LAUNCHER_WITH_WEBENGINE OFF)
    endif()
    if(MSVC AND (CMAKE_VS_PLATFORM_NAME STREQUAL "ARM64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64"))
        set(LAUNCHER_WITH_WEBENGINE OFF)
    endif()

    set(QT_COMPONENTS Core CoreTools Widgets Concurrent Network Test Xml NetworkAuth OpenGL)
    if(LAUNCHER_WITH_WEBENGINE)
        list(APPEND QT_COMPONENTS WebEngineWidgets WebChannel)
    endif()
    find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
    if(DEFINED Qt6Core_VERSION)
        set(_projt_qt_version "${Qt6Core_VERSION}")
    elseif(DEFINED Qt6_VERSION)
        set(_projt_qt_version "${Qt6_VERSION}")
    endif()
    if(DEFINED _projt_qt_version AND _projt_qt_version VERSION_LESS "6.8.0")
        message(FATAL_ERROR "Qt 6.8.0 or newer is required. Detected: ${_projt_qt_version}")
    endif()
    unset(_projt_qt_version)
    find_package(Qt6 COMPONENTS DBus)
    list(APPEND Launcher_QT_DBUS Qt6::DBus)
endif()
# Note: Qt6 removed from fuzzer builds - fuzz_qjson_parse disabled due to glib dependency issues

# System / Package Manager Provided Libs - Using bundled versions
if(NOT LAUNCHER_FUZZ_ONLY)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bzip2/CMakeLists.txt")
        message(STATUS "Using bundled bzip2")
        set(ENABLE_LIB_ONLY ON CACHE BOOL "" FORCE)
        set(ENABLE_TESTS OFF CACHE BOOL "" FORCE)
        set(ENABLE_DOCS OFF CACHE BOOL "" FORCE)
        set(ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
        # Disable shared library and use static library for consistent linking
        set(ENABLE_SHARED_LIB OFF CACHE BOOL "" FORCE)
        set(ENABLE_STATIC_LIB ON CACHE BOOL "" FORCE)
        # Disable instrumentation for external dependencies to avoid linker issues
        string(REGEX REPLACE "-fprofile-instr-generate|--coverage|-fprofile-generate|-fprofile-arcs|-ftest-coverage" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        string(REGEX REPLACE "-fprofile-instr-generate|--coverage|-fprofile-generate|-fprofile-arcs|-ftest-coverage" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("bzip2")
        add_subdirectory(bzip2)
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
        set(BZIP2_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bzip2" CACHE PATH "" FORCE)
        # Provide compatibility for CMake's FindBZip2 and consumers that expect
        # BZip2 variables when using the bundled bzip2 implementation.
        if(NOT TARGET BZip2::BZip2)
            add_library(BZip2::BZip2 ALIAS bz2_static)
        endif()
        set(BZIP2_FOUND TRUE CACHE BOOL "" FORCE)
        set(BZIP2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bzip2" CACHE PATH "" FORCE)
        set(BZIP2_LIBRARIES bz2_static CACHE STRING "" FORCE)
    else()
        message(FATAL_ERROR "Bundled bzip2 not found")
    endif()

    # Build bundled minizip for deterministic WebEngine linking on Linux
    if(PROJT_PTLIBZIPPY_AVAILABLE AND UNIX AND NOT APPLE)
        set(MINIZIP_BUILD_SHARED ON CACHE BOOL "" FORCE)
        set(MINIZIP_BUILD_STATIC OFF CACHE BOOL "" FORCE)
        set(MINIZIP_BUILD_TESTING OFF CACHE BOOL "" FORCE)
        set(MINIZIP_INSTALL ON CACHE BOOL "" FORCE)
        set(MINIZIP_ENABLE_BZIP2 ON CACHE BOOL "" FORCE)
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("minizip")
        add_subdirectory(zlib/contrib/minizip)
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/quazip/CMakeLists.txt")
        message(STATUS "Using bundled QuaZip")
        set(QUAZIP_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
        set(QUAZIP_ENABLE_QTEXTCODEC OFF CACHE BOOL "" FORCE)
        set(QUAZIP_INSTALL ON CACHE BOOL "" FORCE)
        set(QUAZIP_INSTALL_CMAKE_CONFIG OFF CACHE BOOL "" FORCE)
        set(QUAZIP_BZIP2 ON CACHE BOOL "" FORCE)
        set(QUAZIP_FETCH_LIBS OFF CACHE BOOL "" FORCE)
        set(QUAZIP_FORCE_FETCH_LIBS OFF CACHE BOOL "" FORCE)
        # Disable instrumentation for external dependencies to avoid linker issues
        string(REGEX REPLACE "-fprofile-instr-generate|--coverage|-fprofile-generate|-fprofile-arcs|-ftest-coverage" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        string(REGEX REPLACE "-fprofile-instr-generate|--coverage|-fprofile-generate|-fprofile-arcs|-ftest-coverage" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("quazip")
        add_subdirectory(quazip)
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
    else()
        message(FATAL_ERROR "Bundled QuaZip not found")
    endif()

    # tomlplusplus - Using bundled fork only
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tomlplusplus/CMakeLists.txt")
        message(STATUS "Using bundled tomlplusplus")
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("tomlplusplus")
        add_subdirectory(tomlplusplus)
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
    else()
        message(FATAL_ERROR "tomlplusplus not found. tomlplusplus/ directory is missing.")
    endif()
endif()

# cmark - Using bundled only
if(NOT LAUNCHER_FUZZ_ONLY)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmark/CMakeLists.txt")
        message(STATUS "Using bundled cmark")
        set(_launcher_prev_cmark_build_shared_libs "")
        set(_launcher_cmark_build_shared_libs_set FALSE)
        if(DEFINED BUILD_SHARED_LIBS)
            set(_launcher_prev_cmark_build_shared_libs "${BUILD_SHARED_LIBS}")
            set(_launcher_cmark_build_shared_libs_set TRUE)
        endif()
        set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
        unset(CMARK_SHARED CACHE)
        unset(CMARK_STATIC CACHE)
        set(_launcher_prev_cmark_osx_arch "${CMAKE_OSX_ARCHITECTURES}")
        if(APPLE
            AND CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64|aarch64"
            AND CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
            set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
        endif()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("cmark")
        add_subdirectory(cmark)
        projt_pop_output_dirs()
        if(_launcher_cmark_build_shared_libs_set)
            set(BUILD_SHARED_LIBS "${_launcher_prev_cmark_build_shared_libs}" CACHE BOOL "" FORCE)
        else()
            unset(BUILD_SHARED_LIBS CACHE)
        endif()
        unset(_launcher_prev_cmark_build_shared_libs)
        unset(_launcher_cmark_build_shared_libs_set)
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
        if(DEFINED _launcher_prev_cmark_osx_arch)
            set(CMAKE_OSX_ARCHITECTURES "${_launcher_prev_cmark_osx_arch}")
            unset(_launcher_prev_cmark_osx_arch)
        endif()
        if(TARGET cmark AND NOT TARGET cmark::cmark)
            add_library(cmark::cmark ALIAS cmark)
        endif()
    else()
        message(FATAL_ERROR "cmark not found. cmark/ directory is missing.")
    endif()
endif()

if(NOT LAUNCHER_FUZZ_ONLY)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libqrencode/CMakeLists.txt")
        message(STATUS "Using bundled libqrencode")
        # Set CMake policy version to avoid version compatibility error
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        # Avoid optional tools/tests that require getopt/PNG on Windows.
        if(WIN32)
            set(_qrencode_prev_build_shared_libs "${BUILD_SHARED_LIBS}")
            set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
            set(ENABLE_SHARED OFF CACHE BOOL "" FORCE)
            set(ENABLE_STATIC ON CACHE BOOL "" FORCE)
        endif()
        set(WITH_TOOLS OFF CACHE BOOL "" FORCE)
        set(WITH_PNG OFF CACHE BOOL "" FORCE)
        set(_launcher_prev_build_testing "${BUILD_TESTING}")
        set(_launcher_prev_with_tests "")
        set(_launcher_with_tests_set FALSE)
        if(DEFINED WITH_TESTS)
            set(_launcher_prev_with_tests "${WITH_TESTS}")
            set(_launcher_with_tests_set TRUE)
        endif()
        set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
        set(WITH_TESTS OFF CACHE BOOL "" FORCE)
        set(_qrencode_prev_install_libdir "")
        set(_qrencode_install_libdir_set FALSE)
        if(APPLE)
            set(_qrencode_bundle_dir "ProjTLauncher.app/Contents/Frameworks")
            if(DEFINED Launcher_Name AND NOT Launcher_Name STREQUAL "")
                set(_qrencode_bundle_dir "${Launcher_Name}.app/Contents/Frameworks")
            endif()
            if(DEFINED CMAKE_INSTALL_LIBDIR)
                set(_qrencode_prev_install_libdir "${CMAKE_INSTALL_LIBDIR}")
                set(_qrencode_install_libdir_set TRUE)
            endif()
            set(CMAKE_INSTALL_LIBDIR "${_qrencode_bundle_dir}" CACHE PATH "" FORCE)
        endif()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
        endif()
        projt_push_output_dirs("libqrencode")
        projt_push_autogen_disabled()
        add_subdirectory(libqrencode)
        projt_pop_autogen_disabled()
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
        if(APPLE)
            if(_qrencode_install_libdir_set)
                set(CMAKE_INSTALL_LIBDIR "${_qrencode_prev_install_libdir}" CACHE PATH "" FORCE)
            else()
                unset(CMAKE_INSTALL_LIBDIR CACHE)
            endif()
        endif()
        if(WIN32)
            set(BUILD_SHARED_LIBS "${_qrencode_prev_build_shared_libs}" CACHE BOOL "" FORCE)
        endif()
        set(BUILD_TESTING "${_launcher_prev_build_testing}" CACHE BOOL "" FORCE)
        if(_launcher_with_tests_set)
            set(WITH_TESTS "${_launcher_prev_with_tests}" CACHE BOOL "" FORCE)
        else()
            unset(WITH_TESTS CACHE)
        endif()
        unset(_launcher_prev_build_testing)
        unset(_launcher_prev_with_tests)
        unset(_launcher_with_tests_set)
        unset(_qrencode_prev_install_libdir)
        unset(_qrencode_install_libdir_set)
        add_library(qrcodegenerator ALIAS qrencode)
        # Ensure the qrencode target exposes include directories so consumers can find qrencode.h
        if(TARGET qrencode)
            get_target_property(_qr_inc qrencode INTERFACE_INCLUDE_DIRECTORIES)
            if(NOT _qr_inc)
                target_include_directories(qrencode PUBLIC
                    "${CMAKE_CURRENT_SOURCE_DIR}/libqrencode"
                    "${CMAKE_CURRENT_SOURCE_DIR}/libqrencode/src"
                    "${CMAKE_CURRENT_SOURCE_DIR}/libqrencode/lib"
                    "${CMAKE_CURRENT_SOURCE_DIR}/libqrencode/include")
            endif()
        endif()
        if(NOT DEFINED QRENCODE_INC OR QRENCODE_INC STREQUAL "")
            set(QRENCODE_INC "${CMAKE_CURRENT_SOURCE_DIR}/libqrencode;${CMAKE_CURRENT_SOURCE_DIR}/libqrencode/include;${CMAKE_CURRENT_SOURCE_DIR}/libqrencode/lib" CACHE PATH "QRENCODE include dir" FORCE)
        endif()
    else()
        message(FATAL_ERROR "libqrencode not found. libqrencode/ directory is missing.")
    endif()
endif()

if(NOT LAUNCHER_FUZZ_ONLY)
    include(ECMQtDeclareLoggingCategory)
endif()

####################################### Program Info #######################################

if(NOT LAUNCHER_FUZZ_ONLY)
    set(Launcher_APP_BINARY_NAME "projtlauncher" CACHE STRING "Name of the Launcher binary")
    projt_push_output_dirs("program_info")
    add_subdirectory(program_info)
    projt_pop_output_dirs()
endif()

####################################### Install layout #######################################

set(Launcher_ENABLE_UPDATER NO)
set(Launcher_BUILD_UPDATER NO)

if (NOT APPLE AND (NOT Launcher_UPDATER_RELEASES_URL STREQUAL "" AND NOT Launcher_BUILD_ARTIFACT STREQUAL ""))
    set(Launcher_BUILD_UPDATER YES)
endif()

if(NOT LAUNCHER_FUZZ_ONLY)
    if(NOT (UNIX AND APPLE))
        # Install "portable.txt" if selected component is "portable"
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${Launcher_Portable_File}" DESTINATION "." COMPONENT portable EXCLUDE_FROM_ALL)
    endif()
endif()

if(NOT LAUNCHER_FUZZ_ONLY AND UNIX AND APPLE)
    set(BINARY_DEST_DIR "${Launcher_Name}.app/Contents/MacOS")
    set(LIBRARY_DEST_DIR "${Launcher_Name}.app/Contents/MacOS")
    set(PLUGIN_DEST_DIR "${Launcher_Name}.app/Contents/MacOS")
    set(FRAMEWORK_DEST_DIR "${Launcher_Name}.app/Contents/Frameworks")
    set(RESOURCES_DEST_DIR "${Launcher_Name}.app/Contents/Resources")
    set(JARS_DEST_DIR "${Launcher_Name}.app/Contents/MacOS/jars")

    # Mac bundle settings
    set(MACOSX_BUNDLE_BUNDLE_NAME "${Launcher_DisplayName}")
    set(MACOSX_BUNDLE_INFO_STRING "${Launcher_DisplayName}: A custom launcher for Minecraft that allows you to easily manage multiple installations of Minecraft at once.")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.projecttick.${Launcher_Name}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${Launcher_VERSION_NAME}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${Launcher_VERSION_NAME}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${Launcher_VERSION_NAME}")
    set(MACOSX_BUNDLE_ICON_FILE ${Launcher_Name}.icns)
    set(MACOSX_BUNDLE_COPYRIGHT "${Launcher_Copyright_Mac}")
    set(MACOSX_SPARKLE_UPDATE_PUBLIC_KEY "Lju8CtOHHDmev7KkEl3ks/6wWkG1mlZ0G89XIG8hNLQ=" CACHE STRING "Public key for Sparkle update feed")
    set(MACOSX_SPARKLE_UPDATE_FEED_URL "https://projecttick.org/product/projt-launcher/feed/appcast.xml" CACHE STRING "URL for Sparkle update feed")
    set(MACOSX_BOOTSTRAP_FEED_URL "https://projecttick.org/product/projt-launcher/appcast.xml" CACHE STRING "URL for bootstrap update feed")
    set(MACOSX_BOOTSTRAP_URL_TEMPLATE "https://github.com/Project-Tick/ProjT-Launcher/releases/download/%s/ProjTLauncher-macOS-%s.zip" CACHE STRING "URL template for bootstrap download")

    set(MACOSX_SPARKLE_DOWNLOAD_URL "https://github.com/sparkle-project/Sparkle/releases/download/2.8.0/Sparkle-2.8.0.tar.xz" CACHE STRING "URL to Sparkle release archive")
    set(MACOSX_SPARKLE_SHA256 "fd5681ee92bf238aaac2d08214ceaf0cc8976e452d7f882d80bac1e61581f3b1" CACHE STRING "SHA256 checksum for Sparkle release archive")
    set(MACOSX_SPARKLE_DIR "${CMAKE_BINARY_DIR}/frameworks/Sparkle")

    if(NOT MACOSX_SPARKLE_UPDATE_PUBLIC_KEY STREQUAL "" AND NOT MACOSX_SPARKLE_UPDATE_FEED_URL STREQUAL "")
        set(Launcher_ENABLE_UPDATER YES)
    endif()

    # Add the icon
    install(FILES ${Launcher_Branding_ICNS} DESTINATION ${RESOURCES_DEST_DIR} RENAME ${Launcher_Name}.icns)

    if(NOT LAUNCHER_FUZZ_ONLY)
        add_subdirectory(bootstrap/macos)
    endif()

elseif(NOT LAUNCHER_FUZZ_ONLY AND UNIX)
    foreach(_launcher_install_dir IN ITEMS
        BINDIR
        SBINDIR
        LIBDIR
        LIBEXECDIR
        INCLUDEDIR
        LOCALSTATEDIR
        SHAREDSTATEDIR
        DATAROOTDIR
        DATADIR
        LOCALEDIR
        MANDIR
        INFODIR
        SYSCONFDIR
        RUNSTATEDIR
        DOCDIR)
        if(NOT DEFINED KDE_INSTALL_${_launcher_install_dir}
            AND DEFINED CMAKE_INSTALL_${_launcher_install_dir}
            AND NOT CMAKE_INSTALL_${_launcher_install_dir} STREQUAL "")
            set(KDE_INSTALL_${_launcher_install_dir} "${CMAKE_INSTALL_${_launcher_install_dir}}" CACHE PATH "" FORCE)
        endif()
    endforeach()
    unset(_launcher_install_dir)
    include(KDEInstallDirs)

    set(BINARY_DEST_DIR "bin")
    set(LIBRARY_DEST_DIR "lib${LIB_SUFFIX}")
    set(JARS_DEST_DIR "share/${Launcher_Name}")

    # Set RPATH (include bundled lib dir if configured)
    set(_launcher_rpath_entries "")
    if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
        list(APPEND _launcher_rpath_entries "$ORIGIN/../${Launcher_BUNDLED_LIBDIR}")
    endif()
    list(APPEND _launcher_rpath_entries "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}" "$ORIGIN")
    string(JOIN ":" Launcher_BINARY_RPATH ${_launcher_rpath_entries})
    unset(_launcher_rpath_entries)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${Launcher_Desktop} DESTINATION ${KDE_INSTALL_APPDIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${Launcher_MetaInfo} DESTINATION ${KDE_INSTALL_METAINFODIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${Launcher_SVG} DESTINATION "${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${Launcher_mrpack_MIMEInfo} DESTINATION ${KDE_INSTALL_MIMEDIR})

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/launcher/qtlogging.ini" DESTINATION "share/${Launcher_Name}")

    set(PLUGIN_DEST_DIR "plugins")
    set(BUNDLE_DEST_DIR ".")
    set(RESOURCES_DEST_DIR ".")

    if(Launcher_ManPage)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${Launcher_ManPage} DESTINATION "${KDE_INSTALL_MANDIR}/man6")
    endif()

    # Install basic runner script if component "portable" is selected
    configure_file(launcher/Launcher.in "${CMAKE_CURRENT_BINARY_DIR}/LauncherScript" @ONLY)
    install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/LauncherScript" DESTINATION "." RENAME ${Launcher_Name} COMPONENT portable EXCLUDE_FROM_ALL)

elseif(WIN32)
    set(BINARY_DEST_DIR ".")
    set(LIBRARY_DEST_DIR ".")
    set(PLUGIN_DEST_DIR ".")
    set(RESOURCES_DEST_DIR ".")
    set(JARS_DEST_DIR "jars")
elseif(LAUNCHER_FUZZ_ONLY)
    # For fuzz-only builds on other platforms, skip installer/layout specifics.
    set(BINARY_DEST_DIR ".")
    set(LIBRARY_DEST_DIR ".")
    set(PLUGIN_DEST_DIR ".")
    set(RESOURCES_DEST_DIR ".")
    set(JARS_DEST_DIR "jars")
else()
    message(FATAL_ERROR "Platform not supported")
endif()

######################################## Packaging ########################################

if(NOT LAUNCHER_FUZZ_ONLY AND UNIX AND NOT APPLE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LauncherPackaging.cmake)
endif()



################################ Included Libs ################################

include(ExternalProject)
set_directory_properties(PROPERTIES EP_BASE External)

# libnbtplusplus - Using submodule/bundled as it's not in package managers
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libnbtplusplus/CMakeLists.txt")
    message(STATUS "Using bundled libnbtplusplus")
    option(NBT_BUILD_SHARED "Build NBT shared library" OFF)
    option(NBT_USE_ZLIB "Build NBT library with zlib support" ON)
    option(NBT_BUILD_TESTS "Build NBT library tests" OFF)
    projt_push_output_dirs("libnbtplusplus")
    add_subdirectory(./libnbtplusplus)
    projt_pop_output_dirs()
else()
    message(FATAL_ERROR "libnbtplusplus not found. libnbtplusplus/ directory is missing.")
endif()

if(NOT LAUNCHER_FUZZ_ONLY)
    projt_push_output_dirs("systeminfo")
    add_subdirectory(./systeminfo) # system information library
    projt_pop_output_dirs()
endif()
if(NOT LAUNCHER_FUZZ_ONLY)
    projt_push_output_dirs("launcherjava")
    add_subdirectory(./launcherjava) # java based launcher part for Minecraft
    projt_pop_output_dirs()
    projt_push_output_dirs("javacheck")
    add_subdirectory(./javacheck) # java compatibility checker
    projt_pop_output_dirs()
    projt_push_output_dirs("rainbow")
    add_subdirectory(./rainbow) # Qt extension for colors
    projt_pop_output_dirs()
    projt_push_output_dirs("LocalPeer")
    add_subdirectory(./LocalPeer) # fork of a library from Qt solutions
    projt_pop_output_dirs()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gamemode/CMakeLists.txt")
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_push_install_libdir("${Launcher_BUNDLED_LIBDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_push_install_includedir("${Launcher_BUNDLED_INCLUDEDIR}")
            endif()
            if(DEFINED Launcher_BUNDLED_LIBEXECDIR AND NOT Launcher_BUNDLED_LIBEXECDIR STREQUAL "")
                projt_push_install_libexecdir("${Launcher_BUNDLED_LIBEXECDIR}")
            endif()
        endif()
        set(GAMEMODE_WITH_PRIVILEGED_GROUP "ptgamemode" CACHE STRING "" FORCE)
        projt_push_output_dirs("gamemode")
        add_subdirectory(./gamemode)
        projt_pop_output_dirs()
        if(UNIX AND NOT APPLE)
            if(DEFINED Launcher_BUNDLED_LIBEXECDIR AND NOT Launcher_BUNDLED_LIBEXECDIR STREQUAL "")
                projt_pop_install_libexecdir()
            endif()
            if(DEFINED Launcher_BUNDLED_INCLUDEDIR AND NOT Launcher_BUNDLED_INCLUDEDIR STREQUAL "")
                projt_pop_install_includedir()
            endif()
            if(DEFINED Launcher_BUNDLED_LIBDIR AND NOT Launcher_BUNDLED_LIBDIR STREQUAL "")
                projt_pop_install_libdir()
            endif()
        endif()
    else()
        message(STATUS "gamemode/ not found; skipping bundled gamemode")
    endif()
    projt_push_output_dirs("murmur2")
    add_subdirectory(./murmur2) # Hash for usage with the CurseForge API
    projt_pop_output_dirs()
    projt_push_output_dirs("qdcss")
    add_subdirectory(./qdcss) # css parser
    projt_pop_output_dirs()
endif()

############################### Built Artifacts ###############################

if(NOT LAUNCHER_FUZZ_ONLY)
    projt_push_output_dirs("buildconfig")
    add_subdirectory(buildconfig)
    projt_pop_output_dirs()
endif()

if(BUILD_TESTING)
    projt_push_output_dirs("tests")
    add_subdirectory(tests)
    projt_pop_output_dirs()
endif()
if(BUILD_FUZZERS)
    projt_push_output_dirs("fuzz")
    add_subdirectory(fuzz EXCLUDE_FROM_ALL)
    projt_pop_output_dirs()
endif()
# NOTE: this must always be last to appease the CMake deity of quirky install command evaluation order.
if(NOT LAUNCHER_FUZZ_ONLY)
    projt_push_output_dirs("launcher")
    add_subdirectory(launcher)
    projt_pop_output_dirs()
endif()
