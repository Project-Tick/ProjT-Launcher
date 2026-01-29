/*
 * config.h
 * 
 * This file is a part of PTIS.
 * 
 * Copyright (C) 1999-2025 Nullsoft and Contributors
 * 
 * Licensed under the zlib/libpng license (the "License");
 * you may not use this file except in compliance with the License.
 * 
 * Licence details can be found in the file COPYING.
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.
 *
 * Reviewed for Unicode support by Jim Park -- 08/22/2007
 */

#ifndef PTIS_CONFIG_H
#define PTIS_CONFIG_H

#ifndef APSTUDIO_INVOKED // keep msdev's resource editor from mangling the .rc file

#include <ptis-sconf.h>

#ifndef PTIS_CONFIG_VISIBLE_SUPPORT
  #ifdef PTIS_CONFIG_LICENSEPAGE
    #undef PTIS_CONFIG_LICENSEPAGE
  #endif
  #ifdef PTIS_CONFIG_COMPONENTPAGE
    #undef PTIS_CONFIG_COMPONENTPAGE
  #endif
  #ifdef PTIS_SUPPORT_BGBG
    #undef PTIS_SUPPORT_BGBG
  #endif
  #ifdef PTIS_CONFIG_ENHANCEDUI_SUPPORT
    #undef PTIS_CONFIG_ENHANCEDUI_SUPPORT
  #endif
#endif

#ifdef PTIS_CONFIG_ENHANCEDUI_SUPPORT
  #ifndef PTIS_SUPPORT_HWNDS
    #define PTIS_SUPPORT_HWNDS
  #endif
#endif

#ifdef PTIS_CONFIG_LOG_ODS
  #ifndef PTIS_CONFIG_LOG
    #error PTIS_CONFIG_LOG_ODS relies on PTIS_CONFIG_LOG, but PTIS_CONFIG_LOG is not defined
  #endif
#endif

#ifdef PTIS_CONFIG_LOG_STDOUT
  #ifndef PTIS_CONFIG_LOG
    #error PTIS_CONFIG_LOG_STDOUT relies on PTIS_CONFIG_LOG, but PTIS_CONFIG_LOG is not defined
  #endif
#endif

#ifdef PTIS_CONFIG_LOG_TIMESTAMP
  #ifndef PTIS_CONFIG_LOG
    #error PTIS_CONFIG_LOG_TIMESTAMP relies on PTIS_CONFIG_LOG, but PTIS_CONFIG_LOG is not defined
  #endif
#endif

#if defined(PTIS_CONFIG_CRC_SUPPORT) && defined(PTIS_CONFIG_VISIBLE_SUPPORT)
  #define _PTIS_CONFIG_VERIFYDIALOG
#endif

#if defined(PTIS_CONFIG_UNINSTALL_SUPPORT) && defined(PTIS_CONFIG_VISIBLE_SUPPORT)
  #define _PTIS_CONFIG_UNINSTDLG
#endif

#if defined(PTIS_CONFIG_UNINSTALL_SUPPORT) && defined(PTIS_CONFIG_VISIBLE_SUPPORT)
  #define _PTIS_CONFIG_UNINSTDLG
#endif

#ifdef EXEHEAD
  #ifdef PTIS_CONFIG_COMPRESSION_SUPPORT
    #ifndef PTIS_COMPRESS_USE_ZLIB
      #ifndef PTIS_COMPRESS_USE_BZIP2
        #ifndef PTIS_COMPRESS_USE_LZMA
          #error compression is enabled but zlib, bzip2 and lzma are disabled.
        #endif
      #endif
    #endif
  #endif

  #ifdef PTIS_COMPRESS_USE_ZLIB
    #ifdef PTIS_COMPRESS_USE_BZIP2
      #error both zlib and bzip2 are enabled.
    #endif
    #ifdef PTIS_COMPRESS_USE_LZMA
      #error both zlib and lzma are enabled.
    #endif
  #endif
  #ifdef PTIS_COMPRESS_USE_BZIP2
    #ifdef PTIS_COMPRESS_USE_LZMA
      #error both bzip2 and lzma are enabled.
    #endif
  #endif

  #ifdef PTIS_CONFIG_COMPRESSION_SUPPORT
    #ifdef PTIS_COMPRESS_WHOLE
      #ifdef PTIS_CONFIG_VISIBLE_SUPPORT
        #ifndef _PTIS_CONFIG_VERIFYDIALOG
          #define _PTIS_CONFIG_VERIFYDIALOG
        #endif
      #endif
    #endif
  #endif
#endif // EXEHEAD

#ifdef PTIS_COMPRESS_WHOLE
  #ifndef PTIS_CONFIG_COMPRESSION_SUPPORT
    #error PTIS_COMPRESS_WHOLE defined, PTIS_CONFIG_COMPRESSION_SUPPORT not
  #endif
#endif

#ifdef PTIS_CONFIG_CRC_ANAL
  #ifndef PTIS_CONFIG_CRC_SUPPORT
    #error PTIS_CONFIG_CRC_ANAL defined but PTIS_CONFIG_CRC_SUPPORT not
  #endif
#endif

#ifndef PTIS_COMPRESS_BZIP2_LEVEL
  #define PTIS_COMPRESS_BZIP2_LEVEL 9
#endif

#ifdef PTIS_CONFIG_PLUGIN_SUPPORT
  #ifndef PTIS_SUPPORT_RMDIR
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_RMDIR, but PTIS_SUPPORT_RMDIR is not defined
  #endif
  #ifndef PTIS_SUPPORT_FILE
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_FILE, but PTIS_SUPPORT_FILE is not defined
  #endif
  #ifndef PTIS_SUPPORT_ACTIVEXREG
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_ACTIVEXREG, but PTIS_SUPPORT_ACTIVEXREG is not defined
  #endif
  #ifndef PTIS_SUPPORT_STACK
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_STACK, but PTIS_SUPPORT_STACK is not defined
  #endif
  #ifndef PTIS_SUPPORT_FNUTIL
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_FNUTIL, but PTIS_SUPPORT_FNUTIL is not defined
  #endif
  #ifndef PTIS_SUPPORT_DELETE
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_DELETE, but PTIS_SUPPORT_DELETE is not defined
  #endif
  #ifndef PTIS_SUPPORT_MESSAGEBOX
    #error PTIS_CONFIG_PLUGIN_SUPPORT relies on PTIS_SUPPORT_MESSAGEBOX, but PTIS_SUPPORT_MESSAGEBOX is not defined
  #endif
#endif

#if PTIS_MAX_INST_TYPES > 32
  #error PTIS_MAX_INST_TYPES > 32
#endif

#ifndef PTIS_DEFAULT_LANG
  #define PTIS_DEFAULT_LANG 1033
#endif

#endif//!APSTUDIO_INVOKED

#endif // PTIS_CONFIG_H
