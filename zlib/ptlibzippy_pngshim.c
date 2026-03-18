/* ptlibzippy_pngshim.c -- libpng-specific zlib symbol shims
 * Copyright (C) 2026 Project Tick
 * For conditions of distribution and use, see copyright notice in ptlibzippy.h
 */

#include "ptzippyguts.h"

uLong ZEXPORT ptpng_adler32(uLong adler, const Bytef *buf, uInt len) {
    return adler32(adler, buf, len);
}

uLong ZEXPORT ptpng_crc32(uLong crc, const Bytef *buf, uInt len) {
    return crc32(crc, buf, len);
}

int ZEXPORT ptpng_deflate(z_streamp strm, int flush) {
    return deflate(strm, flush);
}

int ZEXPORT ptpng_deflateInit2_(z_streamp strm, int level, int method,
                                int windowBits, int memLevel, int strategy,
                                const char *version, int stream_size) {
    return deflateInit2_(strm, level, method, windowBits, memLevel, strategy,
                         version, stream_size);
}

int ZEXPORT ptpng_deflateReset(z_streamp strm) {
    return deflateReset(strm);
}

int ZEXPORT ptpng_inflate(z_streamp strm, int flush) {
    return inflate(strm, flush);
}

int ZEXPORT ptpng_inflateInit2_(z_streamp strm, int windowBits,
                                const char *version, int stream_size) {
    return inflateInit2_(strm, windowBits, version, stream_size);
}

int ZEXPORT ptpng_inflateReset(z_streamp strm) {
    return inflateReset(strm);
}

int ZEXPORT ptpng_inflateReset2(z_streamp strm, int windowBits) {
    return inflateReset2(strm, windowBits);
}
