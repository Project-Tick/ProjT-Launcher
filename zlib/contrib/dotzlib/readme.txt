This directory contains a .Net wrapper class library for the ZLib1.dll

The wrapper includes support for inflating/deflating memory buffers,
.Net streaming wrappers for the gz streams part of PTlibzippy, and wrappers
for the checksum parts of PTlibzippy. See DotPTLib/UnitTests.cs for examples.

Directory structure:
--------------------

LICENSE_1_0.txt       - License file.
readme.txt            - This file.
DotPTLib.chm           - Class library documentation
DotPTLib.build         - NAnt build file
DotPTLib.sln           - Microsoft Visual Studio 2003 solution file

DotPTLib\*.cs          - Source files for the class library

Unit tests:
-----------
The file DotPTLib/UnitTests.cs contains unit tests for use with NUnit 2.1 or higher.
To include unit tests in the build, define nunit before building.


Build instructions:
-------------------

1. Using Visual Studio.Net 2003:
   Open DotPTLib.sln in VS.Net and build from there. Output file (DotPTLib.dll)
   will be found ./DotPTLib/bin/release or ./DotPTLib/bin/debug, depending on
   you are building the release or debug version of the library. Check
   DotPTLib/UnitTests.cs for instructions on how to include unit tests in the
   build.

2. Using NAnt:
   Open a command prompt with access to the build environment and run nant
   in the same directory as the DotPTLib.build file.
   You can define 2 properties on the nant command-line to control the build:
   debug={true|false} to toggle between release/debug builds (default=true).
   nunit={true|false} to include or exclude unit tests (default=true).
   Also the target clean will remove binaries.
   Output file (DotPTLib.dll) will be found in either ./DotPTLib/bin/release
   or ./DotPTLib/bin/debug, depending on whether you are building the release
   or debug version of the library.

   Examples:
     nant -D:debug=false -D:nunit=false
       will build a release mode version of the library without unit tests.
     nant
       will build a debug version of the library with unit tests
     nant clean
       will remove all previously built files.


---------------------------------
Copyright (c) Henrik Ravn 2004

Use, modification and distribution are subject to the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


TODO: PLEASE TEST IN WINDOWS ENVIRONMENT
