# Copyright (C) 2026 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qmlls_progress LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qmlls_progress
    SOURCES
        tst_qmlls_progress_p.h tst_qmlls_progress.cpp
    LIBRARIES
        Qt::QmlDomPrivate
        Qt::QmlLSPrivate
        Qt::LanguageServerPrivate
        Qt::Test
)
