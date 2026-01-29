/*
 * components.h
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
 *
 * Reviewed for Unicode support by Jim Park -- 08/22/2007
 */

#ifndef ___COMPONENTS_H___
#define ___COMPONENTS_H___

void PTISCALL SectionFlagsChanged(unsigned int index);
#define RefreshSectionGroups() _RefreshSectionGroups(0, 0)
unsigned int PTISCALL _RefreshSectionGroups(unsigned int i, int not_first_call);
#ifdef PTIS_CONFIG_COMPONENTPAGE
void PTISCALL SetInstType(int inst_type);
unsigned int PTISCALL GetInstType(HTREEITEM *items);
#endif//PTIS_CONFIG_COMPONENTPAGE

#endif//!___COMPONENTS_H___
