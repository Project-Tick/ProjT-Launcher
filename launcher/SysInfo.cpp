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
#include "SysInfo.h"

#include <QString>
#include <QSysInfo>

#include "HardwareInfo.h"

#ifdef Q_OS_MACOS
#include <sys/sysctl.h>

bool rosettaDetect()
{
	int ret		= 0;
	size_t size = sizeof(ret);
	if (sysctlbyname("sysctl.proc_translated", &ret, &size, nullptr, 0) == -1)
	{
		return false;
	}
	return ret == 1;
}
#endif

namespace SysInfo
{
	QString currentSystem()
	{
#if defined(Q_OS_LINUX)
		return "linux";
#elif defined(Q_OS_MACOS)
		return "osx";
#elif defined(Q_OS_WINDOWS)
		return "windows";
#elif defined(Q_OS_FREEBSD)
		return "freebsd";
#elif defined(Q_OS_OPENBSD)
		return "openbsd";
#else
		return "unknown";
#endif
	}

	QString useQTForArch()
	{
#if defined(Q_OS_MACOS) && !defined(Q_PROCESSOR_ARM)
		if (rosettaDetect())
		{
			return "arm64";
		}
		else
		{
			return "x86_64";
		}
#endif
		return QSysInfo::currentCpuArchitecture();
	}

	int defaultMaxJvmMem()
	{
		// If totalRAM < 6GB, use (totalRAM / 1.5), else 4GB
		if (const uint64_t totalRAM = HardwareInfo::totalRamMiB(); totalRAM < (4096 * 1.5))
			return totalRAM / 1.5;
		else
			return 4096;
	}

	QString getSupportedJavaArchitecture()
	{
		auto sys  = currentSystem();
		auto arch = useQTForArch();
		if (sys == "windows")
		{
			if (arch == "x86_64")
				return "windows-x64";
			if (arch == "i386")
				return "windows-x86";
			// Unknown, maybe arm, appending arch
			return "windows-" + arch;
		}
		if (sys == "osx")
		{
			if (arch == "arm64")
				return "mac-os-arm64";
			if (arch.contains("64"))
				return "mac-os-x64";
			if (arch.contains("86"))
				return "mac-os-x86";
			// Unknown, maybe something new, appending arch
			return "mac-os-" + arch;
		}
		else if (sys == "linux")
		{
			if (arch == "x86_64")
				return "linux-x64";
			if (arch == "i386")
				return "linux-x86";
			// will work for arm32 arm(64)
			return "linux-" + arch;
		}
		return {};
	}
} // namespace SysInfo
