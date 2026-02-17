cmake_minimum_required(VERSION 3.15)
project(gamemode VERSION 0.0.5.1 LANGUAGES C)

include(GNUInstallDirs)
include(CheckSymbolExists)

# Platform gating
set(GAMEMODE_IS_LINUX OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GAMEMODE_IS_LINUX ON)
endif()

# Non-Linux: only install headers
if(NOT GAMEMODE_IS_LINUX)
    install(FILES lib/gamemode_client.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development)
    return()
endif()

# Linux build options
set(GAMEMODE_SD_BUS_PROVIDER "systemd" CACHE STRING "sd-bus provider: systemd, elogind, no-daemon")
set_property(CACHE GAMEMODE_SD_BUS_PROVIDER PROPERTY STRINGS systemd elogind no-daemon)
option(GAMEMODE_WITH_SYSTEMD_USER_UNIT "Install systemd user unit" ON)
set(GAMEMODE_WITH_SYSTEMD_USER_UNIT_DIR "" CACHE STRING "systemd user unit directory")
option(GAMEMODE_WITH_SYSTEMD_GROUP "Install systemd sysusers group" ON)
set(GAMEMODE_WITH_SYSTEMD_GROUP_DIR "" CACHE STRING "systemd sysusers.d directory")
set(GAMEMODE_WITH_DBUS_SERVICE_DIR "" CACHE STRING "DBUS session service directory")
set(GAMEMODE_WITH_PRIVILEGED_GROUP "gamemode" CACHE STRING "Privileged group name")
option(GAMEMODE_WITH_PAM_RENICING "Install limits.d config" ON)
set(GAMEMODE_WITH_PAM_LIMITS_DIR "/etc/security/limits.d" CACHE STRING "PAM limits.d directory")
option(GAMEMODE_WITH_EXAMPLES "Build examples" ON)
option(GAMEMODE_WITH_UTIL "Build utilities" ON)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# Dependencies
pkg_check_modules(DBUS REQUIRED dbus-1)

set(SD_BUS_DEPS "")
set(SD_BUS_DEFS "")
if(GAMEMODE_SD_BUS_PROVIDER STREQUAL "systemd")
    pkg_check_modules(SYSTEMD REQUIRED libsystemd)
    list(APPEND SD_BUS_DEPS ${SYSTEMD_LIBRARIES})
    list(APPEND SD_BUS_INCLUDES ${SYSTEMD_INCLUDE_DIRS})
elseif(GAMEMODE_SD_BUS_PROVIDER STREQUAL "elogind")
    pkg_check_modules(ELOGIND REQUIRED libelogind)
    list(APPEND SD_BUS_DEPS ${ELOGIND_LIBRARIES})
    list(APPEND SD_BUS_INCLUDES ${ELOGIND_INCLUDE_DIRS})
    list(APPEND SD_BUS_DEFS USE_ELOGIND)
elseif(GAMEMODE_SD_BUS_PROVIDER STREQUAL "no-daemon")
    # no daemon build
else()
    message(FATAL_ERROR "Unknown GAMEMODE_SD_BUS_PROVIDER: ${GAMEMODE_SD_BUS_PROVIDER}")
endif()

# pidfd_open detection
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
check_symbol_exists(pidfd_open "sys/syscall.h;unistd.h" HAVE_FN_PIDFD_OPEN)

# Config header
set(LIBEXECDIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}")
set(_GAMEMODE_SYSCONFDIR "${CMAKE_INSTALL_FULL_SYSCONFDIR}")
if(NOT _GAMEMODE_SYSCONFDIR MATCHES "/$")
    set(_GAMEMODE_SYSCONFDIR "${_GAMEMODE_SYSCONFDIR}/ptgamemode")
else()
    set(_GAMEMODE_SYSCONFDIR "${_GAMEMODE_SYSCONFDIR}ptgamemode")
endif()
set(SYSCONFDIR "${_GAMEMODE_SYSCONFDIR}")
unset(_GAMEMODE_SYSCONFDIR)
set(GAMEMODE_VERSION "${PROJECT_VERSION}")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/build-config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/build-config.h
    @ONLY
)

# Common compiler warnings (best-effort; keep minimal)
set(GAMEMODE_C_WARNINGS
    -fstack-protector
    -Wformat
    -Wstrict-prototypes
    -Wundef
    -fno-common
    -Werror-implicit-function-declaration
    -Wformat-security
    -Werror=format-security
    -Wconversion
    -Wunreachable-code
)

# Common sources
set(GAMEMODE_COMMON_SOURCES
    common/common-logging.c
    common/common-governors.c
    common/common-profile.c
    common/common-splitlock.c
    common/common-external.c
    common/common-helpers.c
    common/common-gpu.c
    common/common-cpu.c
    common/common-pidfds.c
    common/common-power.c
)

add_library(gamemode-daemon-common STATIC ${GAMEMODE_COMMON_SOURCES})
set_target_properties(gamemode-daemon-common PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(gamemode-daemon-common PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_options(gamemode-daemon-common PRIVATE ${GAMEMODE_C_WARNINGS})

add_library(gamemode-lib-common STATIC
    common/common-helpers.c
    common/common-pidfds.c
)
set_target_properties(gamemode-lib-common PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(gamemode-lib-common PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_options(gamemode-lib-common PRIVATE ${GAMEMODE_C_WARNINGS})

# Libraries
add_library(gamemode SHARED lib/client_impl.c)
target_include_directories(gamemode PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
target_include_directories(gamemode PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${DBUS_INCLUDE_DIRS}
)
target_link_libraries(gamemode PRIVATE gamemode-lib-common ${DBUS_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_options(gamemode PRIVATE ${GAMEMODE_C_WARNINGS})
set_target_properties(gamemode PROPERTIES VERSION 0.0.0 SOVERSION 0)

add_library(gamemodeauto SHARED lib/client_loader.c)
target_include_directories(gamemodeauto PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
target_link_libraries(gamemodeauto PRIVATE ${CMAKE_DL_LIBS})
target_compile_options(gamemodeauto PRIVATE ${GAMEMODE_C_WARNINGS})
set_target_properties(gamemodeauto PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS gamemode gamemodeauto
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES lib/gamemode_client.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development)

# Daemon
set(GAMEMODE_BUILD_DAEMON ON)
if(GAMEMODE_SD_BUS_PROVIDER STREQUAL "no-daemon")
    set(GAMEMODE_BUILD_DAEMON OFF)
endif()

if(GAMEMODE_BUILD_DAEMON)
    pkg_check_modules(INIH REQUIRED inih)

    add_executable(gamemoded
        daemon/gamemoded.c
        daemon/gamemode-context.c
        daemon/gamemode-ioprio.c
        daemon/gamemode-sched.c
        daemon/gamemode-wine.c
        daemon/gamemode-tests.c
        daemon/gamemode-gpu.c
        daemon/gamemode-cpu.c
        daemon/gamemode-dbus.c
        daemon/gamemode-config.c
    )
    target_include_directories(gamemoded PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/daemon
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_BINARY_DIR}
        ${DBUS_INCLUDE_DIRS}
        ${SD_BUS_INCLUDES}
        ${INIH_INCLUDE_DIRS}
    )
    target_link_libraries(gamemoded PRIVATE
        gamemode-daemon-common
        Threads::Threads
        ${SD_BUS_DEPS}
        ${INIH_LIBRARIES}
        ${CMAKE_DL_LIBS}
    )
    target_compile_definitions(gamemoded PRIVATE ${SD_BUS_DEFS})
    target_compile_options(gamemoded PRIVATE ${GAMEMODE_C_WARNINGS})
    install(TARGETS gamemoded RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Utilities
if(GAMEMODE_WITH_UTIL)
    set(GAMEMODE_UTILS
        cpugovctl
        gpuclockctl
        cpucorectl
        procsysctl
        platprofctl
        x3dmodectl
    )

    foreach(util ${GAMEMODE_UTILS})
        add_executable(${util} util/${util}.c)
        target_include_directories(${util} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/common
            ${CMAKE_CURRENT_BINARY_DIR}
        )
        target_link_libraries(${util} PRIVATE gamemode-daemon-common)
        target_compile_options(${util} PRIVATE ${GAMEMODE_C_WARNINGS})
        install(TARGETS ${util} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
    endforeach()
endif()

# Example
if(GAMEMODE_WITH_EXAMPLES)
    add_executable(gamemode-simulate-game example/main.c)
    target_include_directories(gamemode-simulate-game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    target_link_libraries(gamemode-simulate-game PRIVATE gamemode)
    install(TARGETS gamemode-simulate-game RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES example/gamemode.ini DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/ptgamemode)
endif()

# Data and configuration installs (daemon-only, matching meson)
if(GAMEMODE_BUILD_DAEMON)
    set(GAMEMODE_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Prefix used for gamemode config files")

    if(GAMEMODE_WITH_DBUS_SERVICE_DIR STREQUAL "")
        set(GAMEMODE_DBUS_SERVICE_DIR "${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/services")
    else()
        set(GAMEMODE_DBUS_SERVICE_DIR "${GAMEMODE_WITH_DBUS_SERVICE_DIR}")
    endif()

    set(GAMEMODE_POLKIT_ACTION_DIR "${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions")
    set(GAMEMODE_POLKIT_RULE_DIR "${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/rules.d")
    set(GAMEMODE_METAINFO_DIR "${CMAKE_INSTALL_DATAROOTDIR}/metainfo")

    # Configure files
    set(BINDIR "${CMAKE_INSTALL_BINDIR}")
    if(NOT IS_ABSOLUTE "${BINDIR}")
        set(BINDIR "${GAMEMODE_PREFIX}/${BINDIR}")
    endif()

    set(LIBEXECDIR "${CMAKE_INSTALL_LIBEXECDIR}")
    if(NOT IS_ABSOLUTE "${LIBEXECDIR}")
        set(LIBEXECDIR "${GAMEMODE_PREFIX}/${LIBEXECDIR}")
    endif()

    set(SYSCONFDIR "${CMAKE_INSTALL_SYSCONFDIR}/ptgamemode")
    if(NOT IS_ABSOLUTE "${SYSCONFDIR}")
        set(SYSCONFDIR "${GAMEMODE_PREFIX}/${SYSCONFDIR}")
    endif()
    set(GAMEMODE_VERSION "${PROJECT_VERSION}")
    set(GAMEMODE_PRIVILEGED_GROUP "${GAMEMODE_WITH_PRIVILEGED_GROUP}")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/example/gamemode.ini" GAMEMODE_EXAMPLE_CONFIG)
    string(REPLACE ";" "\\;" GAMEMODE_EXAMPLE_CONFIG "${GAMEMODE_EXAMPLE_CONFIG}")

    configure_file(data/dbus/org.projecttick.GameMode.service.in
        ${CMAKE_CURRENT_BINARY_DIR}/org.projecttick.GameMode.service
        @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.projecttick.GameMode.service
        DESTINATION ${GAMEMODE_DBUS_SERVICE_DIR})

    if(GAMEMODE_WITH_PRIVILEGED_GROUP)
        configure_file(data/polkit/actions/org.projecttick.GameMode.policy.in
            ${CMAKE_CURRENT_BINARY_DIR}/org.projecttick.GameMode.policy
            @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.projecttick.GameMode.policy
            DESTINATION ${GAMEMODE_POLKIT_ACTION_DIR})

        configure_file(data/polkit/rules.d/gamemode.rules.in
            ${CMAKE_CURRENT_BINARY_DIR}/ptgamemode.rules
            @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ptgamemode.rules
            DESTINATION ${GAMEMODE_POLKIT_RULE_DIR})
    endif()

    configure_file(data/gamemoded.8.in
        ${CMAKE_CURRENT_BINARY_DIR}/gamemoded.8
        @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gamemoded.8
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man8)

    configure_file(data/gamemodelist.1.in
        ${CMAKE_CURRENT_BINARY_DIR}/gamemodelist.1
        @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gamemodelist.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

    if(GAMEMODE_WITH_EXAMPLES)
        configure_file(data/gamemode-simulate-game.1.in
            ${CMAKE_CURRENT_BINARY_DIR}/gamemode-simulate-game.1
            @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gamemode-simulate-game.1
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
    endif()

    configure_file(data/gamemoderun.1.in
        ${CMAKE_CURRENT_BINARY_DIR}/gamemoderun.1
        @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gamemoderun.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

    # Install scripts
    install(PROGRAMS data/gamemoderun DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(PROGRAMS data/gamemodelist DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Metainfo
    install(FILES data/org.projecttick.gamemode.metainfo.xml
        DESTINATION ${GAMEMODE_METAINFO_DIR})

    # systemd (optional)
    if(GAMEMODE_SD_BUS_PROVIDER STREQUAL "systemd" AND GAMEMODE_WITH_SYSTEMD_USER_UNIT)
        if(GAMEMODE_WITH_SYSTEMD_USER_UNIT_DIR STREQUAL "")
            pkg_get_variable(SYSTEMD_USER_UNIT_DIR systemd systemduserunitdir)
            set(GAMEMODE_WITH_SYSTEMD_USER_UNIT_DIR "${SYSTEMD_USER_UNIT_DIR}")
        endif()
        if(GAMEMODE_WITH_SYSTEMD_USER_UNIT_DIR)
            configure_file(data/systemd/user/gamemoded.service.in
                ${CMAKE_CURRENT_BINARY_DIR}/ptgamemoded.service
                @ONLY)
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ptgamemoded.service
                DESTINATION ${GAMEMODE_WITH_SYSTEMD_USER_UNIT_DIR})
        endif()
    endif()

    if(GAMEMODE_SD_BUS_PROVIDER STREQUAL "systemd" AND GAMEMODE_WITH_SYSTEMD_GROUP AND GAMEMODE_WITH_PRIVILEGED_GROUP)
        if(GAMEMODE_WITH_SYSTEMD_GROUP_DIR STREQUAL "")
            pkg_get_variable(SYSTEMD_SYSUSERS_DIR systemd sysusersdir)
            set(GAMEMODE_WITH_SYSTEMD_GROUP_DIR "${SYSTEMD_SYSUSERS_DIR}")
        endif()
        if(GAMEMODE_WITH_SYSTEMD_GROUP_DIR)
            configure_file(data/systemd/sysusers.d/gamemode.conf.in
                ${CMAKE_CURRENT_BINARY_DIR}/ptgamemode.conf
                @ONLY)
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ptgamemode.conf
                DESTINATION ${GAMEMODE_WITH_SYSTEMD_GROUP_DIR})
        endif()
    endif()

    if(GAMEMODE_WITH_PRIVILEGED_GROUP AND GAMEMODE_WITH_PAM_RENICING)
        configure_file(data/pam_limits/10-gamemode.conf.in
            ${CMAKE_CURRENT_BINARY_DIR}/10-ptgamemode.conf
            @ONLY)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/10-ptgamemode.conf
            DESTINATION ${GAMEMODE_WITH_PAM_LIMITS_DIR})
    endif()
endif()
