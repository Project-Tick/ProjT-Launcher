# Makefile for PTlibzippy
# For use with Delphi and C++ Builder under Win32
# Updated for PTlibzippy 1.2.x by Cosmin Truta

# ------------ Borland C++ ------------

# This project uses the Delphi (fastcall/register) calling convention:
LOC = -DZEXPORT=__fastcall -DZEXPORTVA=__cdecl

CC = bcc32
LD = bcc32
AR = tlib
# do not use "-pr" in CFLAGS
CFLAGS = -a -d -k- -O2 -I$(SRCDIR) $(LOC)
LDFLAGS =


# Path to PTlibzippy source directory (override if running from this dir)
SRCDIR = ../..
VPATH = $(SRCDIR)

# variables
PTLIBZIPPY_LIB = ptlibzippy.lib

OBJ1 = adler32.obj compress.obj crc32.obj deflate.obj gzclose.obj gzlib.obj gzread.obj
OBJ2 = gzwrite.obj infback.obj inffast.obj inflate.obj inftrees.obj trees.obj uncompr.obj ptzippyutil.obj
OBJP1 = +adler32.obj+compress.obj+crc32.obj+deflate.obj+gzclose.obj+gzlib.obj+gzread.obj
OBJP2 = +gzwrite.obj+infback.obj+inffast.obj+inflate.obj+inftrees.obj+trees.obj+uncompr.obj+ptzippyutil.obj


# targets
all: $(PTLIBZIPPY_LIB) example.exe minigzip.exe

.c.obj:
	$(CC) -c $(CFLAGS) $*.c

adler32.obj: adler32.c ptlibzippy.h ptzippyconf.h

compress.obj: compress.c ptlibzippy.h ptzippyconf.h

crc32.obj: crc32.c ptlibzippy.h ptzippyconf.h crc32.h

deflate.obj: deflate.c deflate.h ptzippyutil.h ptlibzippy.h ptzippyconf.h

gzclose.obj: gzclose.c ptlibzippy.h ptzippyconf.h ptzippyguts.h

gzlib.obj: gzlib.c ptlibzippy.h ptzippyconf.h ptzippyguts.h

gzread.obj: gzread.c ptlibzippy.h ptzippyconf.h ptzippyguts.h

gzwrite.obj: gzwrite.c ptlibzippy.h ptzippyconf.h ptzippyguts.h

infback.obj: infback.c ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h inflate.h \
 inffast.h inffixed.h

inffast.obj: inffast.c ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h inflate.h \
 inffast.h

inflate.obj: inflate.c ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h inflate.h \
 inffast.h inffixed.h

inftrees.obj: inftrees.c ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h

trees.obj: trees.c ptzippyutil.h ptlibzippy.h ptzippyconf.h deflate.h trees.h

uncompr.obj: uncompr.c ptlibzippy.h ptzippyconf.h

ptzippyutil.obj: ptzippyutil.c ptzippyutil.h ptlibzippy.h ptzippyconf.h

example.obj: test/example.c ptlibzippy.h ptzippyconf.h

minigzip.obj: test/minigzip.c ptlibzippy.h ptzippyconf.h


# For the sake of the old Borland make,
# the command line is cut to fit in the MS-DOS 128 byte limit:
$(PTLIBZIPPY_LIB): $(OBJ1) $(OBJ2)
	-del $(PTLIBZIPPY_LIB)
	$(AR) $(PTLIBZIPPY_LIB) $(OBJP1)
	$(AR) $(PTLIBZIPPY_LIB) $(OBJP2)


# testing
test: example.exe minigzip.exe
	example
	echo hello world | minigzip | minigzip -d

example.exe: example.obj $(PTLIBZIPPY_LIB)
	$(LD) $(LDFLAGS) example.obj $(PTLIBZIPPY_LIB)

minigzip.exe: minigzip.obj $(PTLIBZIPPY_LIB)
	$(LD) $(LDFLAGS) minigzip.obj $(PTLIBZIPPY_LIB)


# cleanup
clean:
	-del *.obj
	-del *.exe
	-del *.lib
	-del *.tds
	-del ptlibzippy.bak
	-del foo.gz

