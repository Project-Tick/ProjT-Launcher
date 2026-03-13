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

#include "PngReader.h"

#include <QFile>
#include <QDebug>

#include <png.h>
#include <csetjmp>
#include <cstring>

QString PngReader::s_lastError;

namespace
{

	struct PngReadContext
	{
		const unsigned char* data;
		size_t size;
		size_t offset;
	};

	void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		PngReadContext* ctx = static_cast<PngReadContext*>(png_get_io_ptr(png_ptr));
		if (ctx->offset + length > ctx->size)
		{
			png_error(png_ptr, "Read past end of data");
			return;
		}
		std::memcpy(data, ctx->data + ctx->offset, length);
		ctx->offset += length;
	}

	void pngErrorCallback(png_structp png_ptr, png_const_charp error_msg)
	{
		PngReader::setLastError(QString::fromUtf8(error_msg));
		longjmp(png_jmpbuf(png_ptr), 1);
	}

	void pngWarningCallback(png_structp, png_const_charp warning_msg)
	{
		qWarning() << "PNG warning:" << warning_msg;
	}

} // namespace

QImage PngReader::readFromData(const QByteArray& data)
{
	if (data.size() < 8)
	{
		s_lastError = "Data too small to be a PNG";
		return QImage();
	}

	// Check PNG signature
	if (!isPngData(data))
	{
		s_lastError = "Invalid PNG signature";
		return QImage();
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, pngErrorCallback, pngWarningCallback);
	if (!png_ptr)
	{
		s_lastError = "Failed to create PNG read struct";
		return QImage();
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		s_lastError = "Failed to create PNG info struct";
		return QImage();
	}

	QImage result;

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		return QImage();
	}

	PngReadContext ctx;
	ctx.data   = reinterpret_cast<const unsigned char*>(data.constData());
	ctx.size   = data.size();
	ctx.offset = 0;

	png_set_read_fn(png_ptr, &ctx, pngReadCallback);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width	= png_get_image_width(png_ptr, info_ptr);
	png_uint_32 height	= png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth	= png_get_bit_depth(png_ptr, info_ptr);

	// Convert to 8-bit RGBA
	if (bit_depth == 16)
	{
		png_set_strip_16(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);

	// Allocate row pointers
	std::vector<png_bytep> row_pointers(height);
	result = QImage(width, height, QImage::Format_RGBA8888);

	if (result.isNull())
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		s_lastError = "Failed to allocate QImage";
		return QImage();
	}

	for (png_uint_32 y = 0; y < height; y++)
	{
		row_pointers[y] = result.scanLine(y);
	}

	png_read_image(png_ptr, row_pointers.data());
	png_read_end(png_ptr, nullptr);

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	s_lastError.clear();
	return result;
}

QImage PngReader::readFromFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		s_lastError = QString("Failed to open file: %1").arg(file.errorString());
		return QImage();
	}

	QByteArray data = file.readAll();
	return readFromData(data);
}

bool PngReader::isPngFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray header = file.read(8);
	return isPngData(header);
}

bool PngReader::isPngData(const QByteArray& data)
{
	if (data.size() < 8)
	{
		return false;
	}

	static const unsigned char png_signature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

	return std::memcmp(data.constData(), png_signature, 8) == 0;
}

QString PngReader::lastError()
{
	return s_lastError;
}

void PngReader::setLastError(const QString& error)
{
	s_lastError = error;
}
