/*
 * util.h
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
 * Unicode support by Jim Park -- 07/23/2007
 */

#ifndef ___PTIS_UTIL_H___
#define ___PTIS_UTIL_H___

#include "../Platform.h"
#include "config.h"
#include <shlobj.h>

extern TCHAR ps_tmpbuf[PTIS_MAX_STRLEN*2];
TCHAR * PTISCALL GetPTISString(TCHAR *outbuf, int strtab);
#define GetPTISStringTT(strtab) GetPTISString(0, (strtab))
#define GetPTISStringNP(strtab) ((const TCHAR *)g_blocks[NB_STRINGS].offset+(strtab))

// A negative string table index means it's a language string table, so we
// use the LANG_STR_TAB() macro to decode it.
#define GetPTISTab(strtab) (strtab < 0 ? LANG_STR_TAB(strtab) : strtab)

#define myatoi(s) ( (int)strtoiptr(s) )
INT_PTR PTISCALL strtoiptr(const TCHAR *s);
#define myitoa iptrtostr
void PTISCALL iptrtostr(TCHAR *s, INT_PTR d);
TCHAR * PTISCALL mystrcpy(TCHAR *out, const TCHAR *in);
int PTISCALL mystrlen(const TCHAR *in);
TCHAR * PTISCALL mystrcat(TCHAR *out, const TCHAR *concat);
TCHAR * PTISCALL mystrstr(TCHAR *a, TCHAR *b);
int StrWideToACP(LPCWSTR Src, char* Dst, int DstCap);
#ifdef UNICODE
#define strcpyWideToT mystrcpy
#else
void strcpyWideToT(TCHAR *out, LPCWSTR in);
#endif

#ifdef _WIN64
#define ComIIDFromString(s,out) SHCLSIDFromString((s),(CLSID*)(out))
#elif defined(UNICODE)
#define ComIIDFromString(s,out) IIDFromString((s), (IID*)(out))
#else
HRESULT ComIIDFromString(LPCTSTR str, IID*out);
#endif

#ifndef KEY_CREATE_LINK
#define KEY_CREATE_LINK 0x0020
#endif
#define KEY_FORCEVIEW KEY_CREATE_LINK // Our private flag used by RegKey* to indicate that we want it to handle HKLM[32|64] style root keys. Cannot be set if the HKEY is a real handle!
#define KEY_ALTERVIEW SYNCHRONIZE // Our private flag used by RegKey* to indicate that we want it to apply g_exec_flags.alter_reg_view. (MSDN:"Registry keys do not support the SYNCHRONIZE standard access right")
#define KEY_FROMSCRIPT (KEY_FORCEVIEW|KEY_ALTERVIEW) // Use this flag for registry operations from a .nsi script
#define PTIS_REGSAM_PRIVATEMASK (KEY_FROMSCRIPT|KEY_FORCEVIEW|KEY_ALTERVIEW)
HKEY PTISCALL GetRegKeyAndSAM(HKEY hKey, REGSAM*pRS);
LONG PTISCALL RegKeyOpen(HKEY hBase, LPCTSTR SubKey, REGSAM RS, HKEY*phKey);
LONG PTISCALL RegKeyCreate(HKEY hBase, LPCTSTR SubKey, REGSAM RS, HKEY*phKey);
void PTISCALL myRegGetStr(HKEY root, const TCHAR *sub, const TCHAR *name, TCHAR *out, UINT altview);


#define GetWinVerNTDDIMajMin() ( *(USHORT*)(&g_osinfo.WVMin) )
#define IsWin95NT4() ( sizeof(void*) == 4 && GetWinVerNTDDIMajMin() == 0x0400 )
#define IsWin9598ME() ( sizeof(TCHAR) == 1 && g_osinfo.WVProd == 0 )
#define IsWin9598() ( IsWin9598ME() && GetWinVerNTDDIMajMin() < MAKEWORD(90, 4) )
#define IsWinVista() ( GetWinVerNTDDIMajMin() == 0x0600 )
#define PTIS_OSINFO_PROD_WOW64FLAG ( sizeof(void*) > 4 ? ( 0 ) : ( 0x80 ) )
#define IsWow64() ( sizeof(void*) > 4 ? ( FALSE ) : ( (signed char) g_osinfo.WVProd < 0 ) ) // PTIS_OSINFO_PROD_WOW64FLAG
#define SystemSupportsAltRegView() ( sizeof(void*) > 4 ? ( TRUE ) : ( IsWow64() ) )


WIN32_FIND_DATA * PTISCALL file_exists(TCHAR *buf);
TCHAR * PTISCALL my_GetTempFileName(TCHAR *buf, const TCHAR *dir);
BOOL PTISCALL myReadFile(HANDLE h, LPVOID buf, DWORD cb);
BOOL PTISCALL myWriteFile(HANDLE h, const void*buf, DWORD cb);
HRESULT PTISCALL UTF16LEBOM(HANDLE h, INT_PTR ForWrite);

//BOOL PTISCALL my_SetWindowText(HWND hWnd, const TCHAR *val);
#define my_SetWindowText SetWindowText
BOOL PTISCALL my_SetDialogItemText(HWND dlg, UINT idx, const TCHAR *val);
//#define my_SetDialogItemText SetDlgItemText
//int PTISCALL my_GetWindowText(HWND hWnd, TCHAR *val, int size);
#define my_GetWindowText GetWindowText
int PTISCALL my_GetDialogItemText(UINT idx, TCHAR *val);
//#define my_GetDialogItemText GetDlgItemText

#ifdef PTIS_CONFIG_LOG
extern TCHAR log_text[2048]; // BUGBUG: Should this be 2*PTIS_MAX_STRLEN? ...and a little larger when PTIS_CONFIG_LOG_TIMESTAMP is defined!
void PTISCALL log_write(int close);
const TCHAR * _RegKeyHandleToName(HKEY hKey);
void _LogData2Hex(TCHAR *buf, size_t cchbuf, BYTE *data, size_t cbdata);
void log_printf(TCHAR *format, ...);
#define log_printf2(x1,x2) log_printf(x1,x2);
#define log_printf3(x1,x2,x3) log_printf(x1,x2,x3);
#define log_printf4(x1,x2,x3,x4) log_printf(x1,x2,x3,x4);
#define log_printf5(x1,x2,x3,x4,x5) log_printf(x1,x2,x3,x4,x5);
#define log_printf6(x1,x2,x3,x4,x5,x6) log_printf(x1,x2,x3,x4,x5,x6);
#define log_printf7(x1,x2,x3,x4,x5,x6,x7) log_printf(x1,x2,x3,x4,x5,x6,x7);
#define log_printf8(x1,x2,x3,x4,x5,x6,x7,x8) log_printf(x1,x2,x3,x4,x5,x6,x7,x8);
#define RegKeyHandleToName(x1) _RegKeyHandleToName(x1);
#define LogData2Hex(x1,x2,x3,x4) _LogData2Hex(x1,x2,x3,x4);
extern int log_dolog;
extern TCHAR g_log_file[1024];
#else
#define log_printf(x1)
#define log_printf2(x1,x2)
#define log_printf3(x1,x2,x3)
#define log_printf4(x1,x2,x3,x4)
#define log_printf5(x1,x2,x3,x4,x5)
#define log_printf6(x1,x2,x3,x4,x5,x6)
#define log_printf7(x1,x2,x3,x4,x5,x6,x7)
#define log_printf8(x1,x2,x3,x4,x5,x6,x7,x8)
#define RegKeyHandleToName(x1) NULL
#define LogData2Hex(x1,x2,x3,x4)
#endif

extern const UINT32 g_restrictedacl[];
#define GetAdminGrpAcl() ( (PACL) g_restrictedacl )
#define GetAdminGrpSid() ( (PSID) &g_restrictedacl[4] )
BOOL PTISCALL UserIsAdminGrpMember(); // Does not check integrity level, returns true if the process has a non-deny administrators group ACE in the token
DWORD PTISCALL CreateRestrictedDirectory(LPCTSTR path);
DWORD PTISCALL CreateNormalDirectory(LPCTSTR path);

HANDLE PTISCALL myCreateProcess(TCHAR *cmd);
BOOL PTISCALL myShellExecuteEx(SHELLEXECUTEINFO*pSEI);
int PTISCALL my_MessageBox(const TCHAR *text, UINT type);

void PTISCALL myDelete(TCHAR *buf, int flags);

HANDLE PTISCALL myOpenFile(const TCHAR *fn, DWORD da, DWORD cd);
int PTISCALL validpathspec(TCHAR *ubuf);
TCHAR * PTISCALL addtrailingslash(TCHAR *str);
//TCHAR PTISCALL lastchar(const TCHAR *str);
#define lastchar(str) *CharPrev(str,str+mystrlen(str))
TCHAR * PTISCALL findchar(TCHAR *str, TCHAR c);
TCHAR * PTISCALL trimslashtoend(TCHAR *buf);
TCHAR * PTISCALL skip_root(TCHAR *path);
int PTISCALL is_valid_instpath(TCHAR *s);
void PTISCALL validate_filename(TCHAR *fn);

/**
 * MoveFileOnReboot tries to move a file by the name of pszExisting to the
 * name pszNew.
 *
 * @param pszExisting The old name of the file.
 * @param pszNew The new name of the file.
 */
void PTISCALL MoveFileOnReboot(LPCTSTR pszExisting, LPCTSTR pszNew);
DWORD PTISCALL remove_ro_attr(LPCTSTR file);

#ifdef _PTIS_NODEFLIB_CRTMEMCPY
#define mini_memcpy memcpy
#else
void PTISCALL mini_memcpy(void *out, const void *in, UINT_PTR cb);
#endif

enum myGetProcAddressFunctions {
  MGA_SetDefaultDllDirectories, // Win8+ but also exists on Vista/2008/7/2008R2 if KB2533623 is installed
#ifndef _WIN64
  MGA_GetDiskFreeSpaceEx,
  MGA_GetUserDefaultUILanguage,
#endif
#if !defined(_WIN64) || defined(_M_IA64)
  MGA_RegDeleteKeyEx,
#endif
  MGA_InitiateShutdown,
  MGA_SHGetKnownFolderPath,
  MGA_IsUserAnAdmin,
  MGA_SHCloneSpecialIDList,
#ifndef _WIN64
  MGA_IsOS,
#endif
  MGA_SHAutoComplete, // x64 can link to shlwapi directly but as long as MGA_SHGetFolderPath is used we can stick with myGetProcAddress
  MGA_SHGetFolderPath, // TODO: This can probably call something else directly on x64
#ifdef PTIS_SUPPORT_GETDLLVERSION
  MGA_GetFileVersionInfoSize, // Version.dll exists in all Windows versions, it is delay loaded to avoid DLL hijacking [bug #1125]
  MGA_GetFileVersionInfo,
  MGA_VerQueryValue
#endif
};

HMODULE PTISCALL LoadSystemLibrary(LPCSTR name);
void*PTISCALL myGetProcAddress(const enum myGetProcAddressFunctions func);
void PTISCALL MessageLoop(UINT uCheckedMsg);

/**
 * This function is useful for Unicode support.  Since the Windows
 * GetProcAddress function always takes a char*, this function wraps
 * the windows call and does the appropriate translation when
 * appropriate.
 *
 * @param dllHandle Handle to the DLL loaded by LoadLibrary[Ex].
 * @param funcName The name of the function to get the address of.
 * @return The pointer to the function.  Null if failure.
 */
void* PTISCALL PTISGetProcAddress(HANDLE dllHandle, TCHAR* funcName);

DWORD PTISCALL WaitForProcess(HANDLE hProcess);

// Turn a pair of chars into a word
// Turn four chars into a dword
#ifdef __BIG_ENDIAN__ // Not very likely, but, still...
#ifdef _UNICODE
#ifdef _PTIS_NO_INT64_SHR
#define CMP4CHAR(mem, const4) ((((LPDWORD)(mem))[0] == (const4[3]|const4[2]<<16)) && (((LPDWORD)(mem))[1] == (const4[1]|const4[0]<<16)))
#else
#define CMP4CHAR(mem, const4) (*(PDWORD64)(mem) == (const4[3]|const4[2]<<16|(DWORD64)const4[1]<<32|(DWORD64)const4[0]<<48))
#endif
#define SET2CHAR(mem, const2) (*(LPDWORD)(mem) = (const2[1]|const2[0]<<16))
#else
#define CMP4CHAR(mem, const4) (*(LPDWORD)(mem) == (const4[3]|const4[2]<<8|const4[1]<<16|const4[0]<<24))
#define SET2CHAR(mem, const2) (*(LPWORD)(mem) = (const2[1]|const2[0]<<8))
#endif
#else
#ifdef _UNICODE
#ifdef _PTIS_NO_INT64_SHR
#define CMP4CHAR(mem, const4) ((((LPDWORD)(mem))[0] == (DWORD)(const4[0]|const4[1]<<16)) && (((LPDWORD)(mem))[1] == (DWORD)(const4[2]|const4[3]<<16)))
#else
#define CMP4CHAR(mem, const4) (*(PDWORD64)(mem) == (DWORD64)(const4[0]|const4[1]<<16|(DWORD64)const4[2]<<32|(DWORD64)const4[3]<<48))
#endif
#define SET2CHAR(mem, const2) (*(LPDWORD)(mem) = (const2[0]|const2[1]<<16))
#else
#define CMP4CHAR(mem, const4) (*(LPDWORD)(mem) == (DWORD)(const4[0]|const4[1]<<8|const4[2]<<16|const4[3]<<24))
#define SET2CHAR(mem, const2) (*(LPWORD)(mem) = (const2[0]|const2[1]<<8))
#endif
#endif

#endif//!___PTIS_UTIL_H___
