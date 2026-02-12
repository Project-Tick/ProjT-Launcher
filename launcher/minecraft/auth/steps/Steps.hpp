// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/**
 * @file Steps.hpp
 * @brief Convenience header including all authentication pipeline steps.
 *
 * Include this header to get access to all step types:
 * - MicrosoftOAuthStep: Browser-based MSA login
 * - DeviceCodeAuthStep: Device code flow for console/headless
 * - XboxLiveUserStep: XBL user token acquisition
 * - XboxSecurityTokenStep: XSTS token for services
 * - XboxProfileFetchStep: Xbox profile (optional)
 * - MinecraftServicesLoginStep: Minecraft access token
 * - MinecraftProfileFetchStep: Minecraft profile
 * - GameEntitlementsStep: Game ownership check
 * - SkinDownloadStep: Player skin image
 */

#pragma once

#include "Credentials.hpp"
#include "Step.hpp"

#include "DeviceCodeAuthStep.hpp"
#include "GameEntitlementsStep.hpp"
#include "MicrosoftOAuthStep.hpp"
#include "MinecraftProfileFetchStep.hpp"
#include "MinecraftServicesLoginStep.hpp"
#include "SkinDownloadStep.hpp"
#include "XboxLiveUserStep.hpp"
#include "XboxProfileFetchStep.hpp"
#include "XboxSecurityTokenStep.hpp"
