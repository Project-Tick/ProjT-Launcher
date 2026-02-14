# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Limit set of tests to run for static Qt builds.
if(QT_BUILD_MINIMAL_STATIC_TESTS)
    return()
endif()

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

if(QT_FEATURE_private_tests)
    add_subdirectory(qquickellipseshape)
    add_subdirectory(qquickrectangleshape)
    add_subdirectory(qquickregularpolygonshape)
    add_subdirectory(qquickstarshape)
endif()
