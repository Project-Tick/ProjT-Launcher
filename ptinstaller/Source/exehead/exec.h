/*
 * exec.h
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

#ifndef _EXEC_H_
#define _EXEC_H_

#include "api.h"

int PTISCALL ExecuteCodeSegment(int pos, HWND hwndProgress); // returns 0 on success
int PTISCALL ExecuteCallbackFunction(int num); // returns 0 on success

#endif//_EXEC_H_
