#ifndef ___PTIS_PLUGIN__H___
#define ___PTIS_PLUGIN__H___

#ifdef __cplusplus
extern "C" {
#endif

#include "api.h"
#include "ptis_tchar.h" // BUGBUG: Why cannot our plugins use the compilers tchar.h?

#ifndef PTISCALL
#  define PTISCALL WINAPI
#endif

#define EXDLL_INIT()           {  \
        g_stringsize=string_size; \
        g_stacktop=stacktop;      \
        g_variables=variables; }

typedef struct _stack_t {
  struct _stack_t *next;
#ifdef UNICODE
  WCHAR text[1]; // this should be the length of g_stringsize when allocating
#else
  char text[1];
#endif
} stack_t;

enum
{
INST_0,         // $0
INST_1,         // $1
INST_2,         // $2
INST_3,         // $3
INST_4,         // $4
INST_5,         // $5
INST_6,         // $6
INST_7,         // $7
INST_8,         // $8
INST_9,         // $9
INST_R0,        // $R0
INST_R1,        // $R1
INST_R2,        // $R2
INST_R3,        // $R3
INST_R4,        // $R4
INST_R5,        // $R5
INST_R6,        // $R6
INST_R7,        // $R7
INST_R8,        // $R8
INST_R9,        // $R9
INST_CMDLINE,   // $CMDLINE
INST_INSTDIR,   // $INSTDIR
INST_OUTDIR,    // $OUTDIR
INST_EXEDIR,    // $EXEDIR
INST_LANG,      // $LANGUAGE
__INST_LAST
};

extern unsigned int g_stringsize;
extern stack_t **g_stacktop;
extern LPTSTR g_variables;

void PTISCALL pushstring(LPCTSTR str);
void PTISCALL pushintptr(INT_PTR value);
#define pushint(v) pushintptr((INT_PTR)(v))
int PTISCALL popstring(LPTSTR str); // 0 on success, 1 on empty stack
int PTISCALL popstringn(LPTSTR str, int maxlen); // with length limit, pass 0 for g_stringsize
INT_PTR PTISCALL popintptr();
#define popint() ( (int) popintptr() )
int PTISCALL popint_or(); // with support for or'ing (2|4|8)
INT_PTR PTISCALL ptishelper_str_to_ptr(LPCTSTR s);
#define myatoi(s) ( (int) ptishelper_str_to_ptr(s) ) // converts a string to an integer
unsigned int PTISCALL myatou(LPCTSTR s); // converts a string to an unsigned integer, decimal only
int PTISCALL myatoi_or(LPCTSTR s); // with support for or'ing (2|4|8)
LPTSTR PTISCALL getuservariable(const int varnum);
void PTISCALL setuservariable(const int varnum, LPCTSTR var);

#ifdef UNICODE
#define PopStringW(x) popstring(x)
#define PushStringW(x) pushstring(x)
#define SetUserVariableW(x,y) setuservariable(x,y)

int  PTISCALL PopStringA(LPSTR ansiStr);
void PTISCALL PushStringA(LPCSTR ansiStr);
void PTISCALL GetUserVariableW(const int varnum, LPWSTR wideStr);
void PTISCALL GetUserVariableA(const int varnum, LPSTR ansiStr);
void PTISCALL SetUserVariableA(const int varnum, LPCSTR ansiStr);

#else
// ANSI defs

#define PopStringA(x) popstring(x)
#define PushStringA(x) pushstring(x)
#define SetUserVariableA(x,y) setuservariable(x,y)

int  PTISCALL PopStringW(LPWSTR wideStr);
void PTISCALL PushStringW(LPWSTR wideStr);
void PTISCALL GetUserVariableW(const int varnum, LPWSTR wideStr);
void PTISCALL GetUserVariableA(const int varnum, LPSTR ansiStr);
void PTISCALL SetUserVariableW(const int varnum, LPCWSTR wideStr);

#endif

#ifdef __cplusplus
}
#endif

#endif//!___PTIS_PLUGIN__H___
