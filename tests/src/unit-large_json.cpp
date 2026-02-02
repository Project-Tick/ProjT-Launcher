//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++ (supporting code)
// |  |  |__   |  |  | | | |  version 0.0.5-1
// |_____|_____|_____|_|___|  https://github.com/Project-Tick/json
//
// SPDX-FileCopyrightText: 2013-2026 Niels Lohmann <https://nlohmann.me>
// SPDX-FileCopyrightText: 2026 Project Tick <https://projecttick.org>
// SPDX-License-Identifier: MIT

#include "doctest_compatibility.h"

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <algorithm>

TEST_CASE("tests on very large JSONs")
{
    SECTION("issue #1419 - Segmentation fault (stack overflow) due to unbounded recursion")
    {
        const auto depth = 5000000;

        std::string s(static_cast<std::size_t>(2 * depth), '[');
        std::fill(s.begin() + depth, s.end(), ']');

        json _;
        CHECK_NOTHROW(_ = nlohmann::json::parse(s));
    }
}

