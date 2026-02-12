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

#include "WindowsConsole.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <iostream>

namespace
{
	bool ShouldBindStdHandle(DWORD handleId)
	{
		HANDLE handle = GetStdHandle(handleId);
		if (handle == INVALID_HANDLE_VALUE || handle == nullptr)
		{
			return true;
		}
		DWORD type = GetFileType(handle);
		return type == FILE_TYPE_CHAR || type == FILE_TYPE_UNKNOWN;
	}

	void RedirectHandle(DWORD handleId, FILE* stream, const char* mode)
	{
		HANDLE stdHandle = GetStdHandle(handleId);
		if (stdHandle == INVALID_HANDLE_VALUE || stdHandle == nullptr)
		{
			return;
		}

		int fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(stdHandle), _O_TEXT);
		if (fileDescriptor == -1)
		{
			return;
		}

		FILE* file = _fdopen(fileDescriptor, mode);
		if (file == nullptr)
		{
			return;
		}

		if (_dup2(_fileno(file), _fileno(stream)) == 0)
		{
			setvbuf(stream, nullptr, _IONBF, 0);
		}
	}

	void ReopenToNull(FILE* stream, const char* mode)
	{
		FILE* dummyFile = nullptr;
		freopen_s(&dummyFile, "nul", mode, stream);
	}
} // namespace

namespace projt::console
{
	void BindCrtHandlesToStdHandles(bool bindStdIn, bool bindStdOut, bool bindStdErr)
	{
		if (bindStdIn)
		{
			ReopenToNull(stdin, "r");
		}
		if (bindStdOut)
		{
			ReopenToNull(stdout, "w");
		}
		if (bindStdErr)
		{
			ReopenToNull(stderr, "w");
		}

		if (bindStdIn)
		{
			RedirectHandle(STD_INPUT_HANDLE, stdin, "r");
		}
		if (bindStdOut)
		{
			RedirectHandle(STD_OUTPUT_HANDLE, stdout, "w");
		}
		if (bindStdErr)
		{
			RedirectHandle(STD_ERROR_HANDLE, stderr, "w");
		}
		if (bindStdIn)
		{
			std::wcin.clear();
			std::cin.clear();
		}
		if (bindStdOut)
		{
			std::wcout.clear();
			std::cout.clear();
		}
		if (bindStdErr)
		{
			std::wcerr.clear();
			std::cerr.clear();
		}
	}

	bool AttachWindowsConsole()
	{
		bool bindStdIn	= ShouldBindStdHandle(STD_INPUT_HANDLE);
		bool bindStdOut = ShouldBindStdHandle(STD_OUTPUT_HANDLE);
		bool bindStdErr = ShouldBindStdHandle(STD_ERROR_HANDLE);

		if (AttachConsole(ATTACH_PARENT_PROCESS))
		{
			BindCrtHandlesToStdHandles(bindStdIn, bindStdOut, bindStdErr);
			return true;
		}

		return false;
	}

	std::error_code EnableAnsiSupport()
	{
		HANDLE console_handle = CreateFileW(L"CONOUT$",
											FILE_GENERIC_READ | FILE_GENERIC_WRITE,
											FILE_SHARE_WRITE | FILE_SHARE_READ,
											NULL,
											OPEN_EXISTING,
											FILE_ATTRIBUTE_NORMAL,
											NULL);
		if (console_handle == INVALID_HANDLE_VALUE)
		{
			return std::error_code(GetLastError(), std::system_category());
		}

		DWORD console_mode;
		if (0 == GetConsoleMode(console_handle, &console_mode))
		{
			auto error = std::error_code(GetLastError(), std::system_category());
			CloseHandle(console_handle);
			return error;
		}

		if ((console_mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0)
		{
			if (0 == SetConsoleMode(console_handle, console_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
			{
				auto error = std::error_code(GetLastError(), std::system_category());
				CloseHandle(console_handle);
				return error;
			}
		}

		CloseHandle(console_handle);
		return {};
	}
} // namespace projt::console
