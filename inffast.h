/* inffast.h -- header to use inffast.c
 * Copyright (C) 1995-2003, 2010 Mark Adler
 * Copyright (C) 2026 Project Tick
 * For conditions of distribution and use, see copyright notice in ptlibzippy.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use ptlibzippy.h.
 */

void PTLIBZIPPY_INTERNAL inflate_fast(z_streamp strm, unsigned start);
