/*
 * plugin.h
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
 */

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "../Platform.h"
#include "fileform.h"
#include "api.h"

#ifdef PTIS_CONFIG_PLUGIN_SUPPORT

extern int PTISCALL RegisterPluginCallback(HMODULE pluginHandle, PTISPLUGINCALLBACK proc);

extern void PTISCALL Plugins_SendMsgToAllPlugins(int msg);
extern void PTISCALL Plugins_UnloadAll();
extern BOOL PTISCALL Plugins_CanUnload(HANDLE pluginHandle);

#endif /* #ifdef PTIS_CONFIG_PLUGIN_SUPPORT */

#endif /* _PLUGIN_H_ */
