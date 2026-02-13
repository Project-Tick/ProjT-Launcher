Building instructions for the DLL versions of Zlib 0.0.5.1
========================================================

This directory contains projects that build PTlibzippy and minizip using
Microsoft Visual C++ 9.0 - 17.x.

You don't need to build these projects yourself. You can download the
binaries from:
  http://www.winimage.com/ptlibzippyDll

More information can be found at this site.





Build instructions for Visual Studio 2008 (32 bits or 64 bits)
--------------------------------------------------------------
- Decompress current PTlibzippy, including all contrib/* files
- Open contrib\vstudio\vc9\ptlibzippyvc.sln with Microsoft Visual C++ 2008
- Or run: vcbuild /rebuild contrib\vstudio\vc9\ptlibzippyvc.sln "Release|Win32"

Build instructions for Visual Studio 2010 (32 bits or 64 bits)
--------------------------------------------------------------
- Decompress current PTlibzippy, including all contrib/* files
- Open contrib\vstudio\vc10\ptlibzippyvc.sln with Microsoft Visual C++ 2010

Build instructions for Visual Studio 2012 (32 bits or 64 bits)
--------------------------------------------------------------
- Decompress current PTlibzippy, including all contrib/* files
- Open contrib\vstudio\vc11\ptlibzippyvc.sln with Microsoft Visual C++ 2012

Build instructions for Visual Studio 2013 (32 bits or 64 bits)
--------------------------------------------------------------
- Decompress current PTlibzippy, including all contrib/* files
- Open contrib\vstudio\vc12\ptlibzippyvc.sln with Microsoft Visual C++ 2013

Build instructions for Visual Studio 2015 (32 bits or 64 bits)
--------------------------------------------------------------
- Decompress current PTlibzippy, including all contrib/* files
- Open contrib\vstudio\vc14\ptlibzippyvc.sln with Microsoft Visual C++ 2015

Build instructions for Visual Studio 2022 (64 bits)
--------------------------------------------------------------
- Decompress current PTlibzippy, including all contrib/* files
- Open contrib\vstudio\vc17\ptlibzippyvc.sln with Microsoft Visual C++ 2022



Important
---------
- To use ptlibzippywapi.dll in your application, you must define the
  macro PTLIBZIPPY_WINAPI when compiling your application's source files.


Additional notes
----------------
- This DLL, named ptlibzippywapi.dll, is compatible to the old ptlibzippy.dll built
  by Gilles Vollant from the PTlibzippy 1.1.x sources, and distributed at
    http://www.winimage.com/ptlibzippyDll
  It uses the WINAPI calling convention for the exported functions, and
  includes the minizip functionality. If your application needs that
  particular build of ptlibzippy.dll, you can rename ptlibzippywapi.dll to ptlibzippy.dll.

- The new DLL was renamed because there exist several incompatible
  versions of ptlibzippy.dll on the Internet.

- There is also an official DLL build of PTlibzippy, named ptlibzippy1.dll. This one
  is exporting the functions using the CDECL convention. See the file
  win32\DLL_FAQ.txt found in this PTlibzippy distribution.

- There used to be a PTLIBZIPPY_DLL macro in PTlibzippy 1.1.x, but now this symbol
  has a slightly different effect. To avoid compatibility problems, do
  not define it here.


Gilles Vollant
info@winimage.com

Visual Studio 2013, 2015, and 2022 Projects from Sean Hunt
seandhunt_7@yahoo.com
