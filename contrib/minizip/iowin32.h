/* iowin32.h -- IO base function header for compress/uncompress .zip
     part of the MiniZip project - ( https://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2026 Gilles Vollant (minizip) ( https://www.winimage.com/zLibDll/minizip.html )

         Modifications for Zip64 support
         Copyright (C) 2009-2010 Mathias Svensson ( https://result42.com )

         For more info read MiniZip_info.txt

*/

#include <windows.h>


#ifdef __cplusplus
extern "C" {
#endif

void fill_win32_filefunc(ptlibzippy_filefunc_def* pptlibzippy_filefunc_def);
void fill_win32_filefunc64(ptlibzippy_filefunc64_def* pptlibzippy_filefunc_def);
void fill_win32_filefunc64A(ptlibzippy_filefunc64_def* pptlibzippy_filefunc_def);
void fill_win32_filefunc64W(ptlibzippy_filefunc64_def* pptlibzippy_filefunc_def);

#ifdef __cplusplus
}
#endif
