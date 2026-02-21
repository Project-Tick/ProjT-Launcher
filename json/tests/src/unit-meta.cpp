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

TEST_CASE("version information")
{
    SECTION("meta()")
    {
        json j = json::meta();

        CHECK(j["name"] == "JSON for Modern C++");
        CHECK(j["copyright"] == "(C) 2013-2026 Niels Lohmann");
        CHECK(j["url"] == "https://github.com/Project-Tick/json");
        CHECK(j["version"] == json(
        {
            {"string", "0.0.5-1"},
            {"major", 0},
            {"minor", 0},
            {"patch", 5},
            {"tweak", 1}
        }));

        CHECK(j.find("platform") != j.end());
        CHECK(j.at("compiler").find("family") != j.at("compiler").end());
        CHECK(j.at("compiler").find("version") != j.at("compiler").end());
        CHECK(j.at("compiler").find("c++") != j.at("compiler").end());
    }
}
