// SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ======================================================================== */

#include <fstream>
#include <string>

#include <launch/LaunchTask.h>
#include "PrintInstanceInfo.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
namespace
{
#if defined(Q_OS_LINUX)
	void probeProcCpuinfo(QStringList& log)
	{
		std::ifstream cpuin("/proc/cpuinfo");
		for (std::string line; std::getline(cpuin, line);)
		{
			if (strncmp(line.c_str(), "model name", 10) == 0)
			{
				log << QString::fromStdString(line.substr(13, std::string::npos));
				break;
			}
		}
	}

	void runLspci(QStringList& log)
	{
		std::string line;
		int gpuline = -1;
		int cline	= 0;
		FILE* lspci = popen("lspci -k", "r");

		if (!lspci)
			return;

		char* lineptr = nullptr;
		size_t n	  = 0;
		while (getline(&lineptr, &n, lspci) != -1)
		{
			line = lineptr;
			if (line.length() < 9)
				continue;
			if (line.substr(8, 3) == "VGA")
			{
				gpuline = cline;
				log << QString::fromStdString(line.substr(35, std::string::npos));
			}
			if (gpuline > -1 && gpuline != cline)
			{
				if (cline - gpuline < 3)
				{
					log << QString::fromStdString(line.substr(1, std::string::npos));
				}
			}
			cline++;
		}
		free(lineptr);
		pclose(lspci);
	}
#elif defined(Q_OS_FREEBSD)
	void runSysctlHwModel(QStringList& log)
	{
		char buff[512];
		FILE* hwmodel = popen("sysctl hw.model", "r");
		while (fgets(buff, 512, hwmodel) != NULL)
		{
			log << QString::fromUtf8(buff);
			break;
		}
		pclose(hwmodel);
	}

	void runPciconf(QStringList& log)
	{
		char buff[512];
		std::string strcard;
		FILE* pciconf = popen("pciconf -lv -a vgapci0", "r");
		while (fgets(buff, 512, pciconf) != NULL)
		{
			if (strncmp(buff, "    vendor", 10) == 0)
			{
				std::string str(buff);
				strcard.append(
					str.substr(str.find_first_of("'") + 1, str.find_last_not_of("'") - (str.find_first_of("'") + 2)));
				strcard.append(" ");
			}
			else if (strncmp(buff, "    device", 10) == 0)
			{
				std::string str2(buff);
				strcard.append(str2.substr(str2.find_first_of("'") + 1,
										   str2.find_last_not_of("'") - (str2.find_first_of("'") + 2)));
			}
			log << QString::fromStdString(strcard);
			break;
		}
		pclose(pciconf);
	}
#endif
	void runGlxinfo(QStringList& log)
	{
		FILE* glxinfo = popen("glxinfo", "r");
		if (!glxinfo)
			return;

		char* lineptr = nullptr;
		size_t n	  = 0;
		while (getline(&lineptr, &n, glxinfo) != -1)
		{
			if (strncmp(lineptr, "OpenGL version string:", 22) == 0)
			{
				log << QString::fromUtf8(lineptr);
				break;
			}
		}
		free(lineptr);
		pclose(glxinfo);
	}

} // namespace
#endif

void PrintInstanceInfo::executeTask()
{
	auto instance = m_parent->instance();
	QStringList log;

#if defined(Q_OS_LINUX)
	::probeProcCpuinfo(log);
	::runLspci(log);
	::runGlxinfo(log);
#elif defined(Q_OS_FREEBSD)
	::runSysctlHwModel(log);
	::runPciconf(log);
	::runGlxinfo(log);
#endif

	logLines(log, MessageLevel::Launcher);
	logLines(instance->verboseDescription(m_session, m_targetToJoin), MessageLevel::Launcher);
	emitSucceeded();
}
