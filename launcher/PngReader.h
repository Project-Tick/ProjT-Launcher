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

#pragma once

#include <QByteArray>
#include <QImage>
#include <QString>

/**
 * @brief PNG reader using the bundled libpng and zlib
 *
 * This class provides PNG reading functionality using the project's own
 * libpng and zlib implementations, avoiding conflicts with system libraries.
 *
 * Qt's QImage uses the system libpng which may be compiled with a different
 * zlib version, causing "unsupported zlib version" errors. This class
 * bypasses that by using our bundled libraries directly.
 */
class PngReader
{
  public:
	/**
	 * @brief Read a PNG image from a file
	 * @param filePath Path to the PNG file
	 * @return QImage containing the decoded image, or null QImage on failure
	 */
	static QImage readFromFile(const QString& filePath);

	/**
	 * @brief Read a PNG image from memory
	 * @param data PNG data in memory
	 * @return QImage containing the decoded image, or null QImage on failure
	 */
	static QImage readFromData(const QByteArray& data);

	/**
	 * @brief Check if a file is a valid PNG
	 * @param filePath Path to the file
	 * @return true if the file has a valid PNG signature
	 */
	static bool isPngFile(const QString& filePath);

	/**
	 * @brief Check if data is valid PNG
	 * @param data Data to check
	 * @return true if the data has a valid PNG signature
	 */
	static bool isPngData(const QByteArray& data);

	/**
	 * @brief Get the last error message
	 * @return Error message from the last failed operation
	 */
	static QString lastError();

	/**
	 * @brief Set the last error message (for internal use by callbacks)
	 * @param error Error message
	 */
	static void setLastError(const QString& error);

  private:
	static QString s_lastError;
};
