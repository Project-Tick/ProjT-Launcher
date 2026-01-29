/*
 * state.h
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
 * Unicode support by Jim Park -- 08/22/2007
 */

#ifndef PTIS_EXEHEAD_STATE_H
#define PTIS_EXEHEAD_STATE_H
#include "fileform.h"
#include "api.h"

#ifdef __GNUC__
// GCC warns about array bounds when accessing g_usrvarssection[2] because it is only [1] at compile time, 
// the other part of this hack is in util.c where g_usrvarsstart is initialized.
extern const PTIS_STRING*const g_usrvarsstart;
#define g_usrvars ( (PTIS_STRING*) (g_usrvarsstart) )
#else
extern PTIS_STRING g_usrvarssection[1];
#define g_usrvars g_usrvarssection
#endif

#define state_command_line        (((PTIS_STRING *) g_usrvars)[20])
#define state_install_directory   (((PTIS_STRING *) g_usrvars)[21])
#define state_output_directory    (((PTIS_STRING *) g_usrvars)[22])
#define state_exe_directory       (((PTIS_STRING *) g_usrvars)[23])
#define state_language            (((PTIS_STRING *) g_usrvars)[24])
#define state_temp_dir            (((PTIS_STRING *) g_usrvars)[25])
#ifdef PTIS_CONFIG_PLUGIN_SUPPORT
#  define state_plugins_dir       (((PTIS_STRING *) g_usrvars)[26])
#endif
#define state_exe_path            (((PTIS_STRING *) g_usrvars)[27])
#define state_exe_file            (((PTIS_STRING *) g_usrvars)[28])
#define state_click_next          (((PTIS_STRING *) g_usrvars)[30])

extern TCHAR g_caption[PTIS_MAX_STRLEN*2];
#ifdef PTIS_CONFIG_VISIBLE_SUPPORT
extern HWND g_hwnd;
extern HANDLE g_hInstance;
extern HWND insthwnd,insthwndbutton;
#else
#define g_hwnd 0
#define g_hInstance 0
#endif//PTIS_CONFIG_VISIBLE_SUPPORT


typedef struct {
  exec_flags_t exec_flags;
  osinfo osi;
} execflags_and_osinfo;

extern execflags_and_osinfo g_execflags_and_osinfo;
#define g_exec_flags (g_execflags_and_osinfo.exec_flags)
#define g_osinfo (g_execflags_and_osinfo.osi)

#endif //~ Include guard
