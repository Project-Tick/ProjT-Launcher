//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 0.0.5-1
// |_____|_____|_____|_|___|  https://github.com/Project-Tick/json
//
// SPDX-FileCopyrightText: 2013-2026 Niels Lohmann <https://nlohmann.me>
// SPDX-FileCopyrightText: 2026 Project Tick <https://projecttick.org>
// SPDX-License-Identifier: MIT

#pragma once

#include <nlohmann/detail/abi_macros.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{

// dispatching helper struct
template <class T> struct identity_tag {};

}  // namespace detail
NLOHMANN_JSON_NAMESPACE_END
