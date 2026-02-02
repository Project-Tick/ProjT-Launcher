//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 0.0.5-1
// |_____|_____|_____|_|___|  https://github.com/Project-Tick/json
//
// SPDX-FileCopyrightText: 2013-2026 Niels Lohmann <https://nlohmann.me>
// SPDX-FileCopyrightText: 2026 Project Tick <https://projecttick.org>
// SPDX-License-Identifier: MIT

#pragma once

#include <nlohmann/detail/macro_scope.hpp>

#if JSON_HAS_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{
namespace std_fs = std::experimental::filesystem;
}  // namespace detail
NLOHMANN_JSON_NAMESPACE_END
#elif JSON_HAS_FILESYSTEM
#include <filesystem> // NOLINT(build/c++17)
NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{
namespace std_fs = std::filesystem;
}  // namespace detail
NLOHMANN_JSON_NAMESPACE_END
#endif
