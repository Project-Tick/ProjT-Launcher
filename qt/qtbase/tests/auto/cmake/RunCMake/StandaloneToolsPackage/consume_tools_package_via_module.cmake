# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

find_package(Qt6 REQUIRED COMPONENTS Workshop)

if(NOT QT_GARAGE_TOOLS_CONFIG_EXTRAS_LOADED)
    message(FATAL_ERROR "Qt6GarageToolsConfigExtras.cmake was not loaded.")
endif()

if(NOT QT_GARAGE_TOOLS_EXTRA_INCLUDE_LOADED)
    message(FATAL_ERROR "Qt6GarageToolsExtraInclude.cmake was not loaded.")
endif()

if(NOT QT_SCREW_DRIVER_LOADED)
    message(FATAL_ERROR "FindWrapScrewdriver.cmake was not loaded.")
endif()
