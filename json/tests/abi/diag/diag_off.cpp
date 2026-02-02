//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++ (supporting code)
// |  |  |__   |  |  | | | |  version 0.0.5-1
// |_____|_____|_____|_|___|  https://github.com/Project-Tick/json
//
// SPDX-FileCopyrightText: 2013-2026 Niels Lohmann <https://nlohmann.me>
// SPDX-FileCopyrightText: 2026 Project Tick <https://projecttick.org>
// SPDX-License-Identifier: MIT

#undef JSON_DIAGNOSTICS
#define JSON_DIAGNOSTICS 0
#include <nlohmann/json.hpp>

#include "diag.hpp"

std::size_t json_sizeof_diag_off()
{
    return sizeof(nlohmann::json);
}

std::size_t json_sizeof_diag_off_explicit()
{
    return sizeof(::NLOHMANN_JSON_NAMESPACE::json);
}

void json_at_diag_off()
{
    using nlohmann::json;
    json j = json{{"foo", json::object()}};
    j.at(json::json_pointer("/foo/bar"));
}
