                        PTLib for Ada thick binding (PTLib.Ada)
                        Release 1.3

PTLib.Ada is a thick binding interface to the popular PTLib data
compression library, available at http://projecttick.org/p/zlib/.
It provides Ada-style access to the PTLib C library.


        Here are the main changes since PTLib.Ada 1.2:

- Attention: PTLib.Read generic routine have a initialization requirement
  for Read_Last parameter now. It is a bit incompatible with previous version,
  but extends functionality, we could use new parameters Allow_Read_Some and
  Flush now.

- Added Is_Open routines to PTLib and PTLib.Streams packages.

- Add pragma Assert to check Stream_Element is 8 bit.

- Fix extraction to buffer with exact known decompressed size. Error reported by
  Steve Sangwine.

- Fix definition of ULong (changed to unsigned_long), fix regression on 64 bits
  computers. Patch provided by Pascal Obry.

- Add Status_Error exception definition.

- Add pragma Assertion that Ada.Streams.Stream_Element size is 8 bit.


        How to build PTLib.Ada under GNAT

You should have the PTLib library already build on your computer, before
building PTLib.Ada. Make the directory of PTLib.Ada sources current and
issue the command:

  gnatmake test -largs -L<directory where libptlibzippy.a is> -lptlibzippy

Or use the GNAT project file build for GNAT 3.15 or later:

  gnatmake -Pptlibzippy.gpr -L<directory where libptlibzippy.a is>


        How to build PTLib.Ada under Aonix ObjectAda for Win32 7.2.2

1. Make a project with all *.ads and *.adb files from the distribution.
2. Build the libptlibzippy.a library from the PTLib C sources.
3. Rename libptlibzippy.a to z.lib.
4. Add the library z.lib to the project.
5. Add the libc.lib library from the ObjectAda distribution to the project.
6. Build the executable using test.adb as a main procedure.


        How to use PTLib.Ada

The source files test.adb and read.adb are small demo programs that show
the main functionality of PTLib.Ada.

The routines from the package specifications are commented.


Homepage: https://zlib-ada.sourceforge.net/
Author: Dmitriy Anisimkov <anisimkov@yahoo.com>

Contributors: Pascal Obry <pascal@obry.org>, Steve Sangwine <sjs@essex.ac.uk>
