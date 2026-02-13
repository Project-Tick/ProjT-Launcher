# Makefile for PTlibzippy
# OpenWatcom large model
# Last updated: 28-Dec-2005

# To use, do "wmake -f watcom_l.mak"

C_SOURCE =  adler32.c  compress.c crc32.c   deflate.c    &
	    gzclose.c  gzlib.c    gzread.c  gzwrite.c    &
            infback.c  inffast.c  inflate.c inftrees.c   &
            trees.c    uncompr.c  ptzippyutil.c

OBJS =      adler32.obj  compress.obj crc32.obj   deflate.obj    &
	    gzclose.obj  gzlib.obj    gzread.obj  gzwrite.obj    &
            infback.obj  inffast.obj  inflate.obj inftrees.obj   &
            trees.obj    uncompr.obj  ptzippyutil.obj

CC       = wcc
LINKER   = wcl
CFLAGS   = -zq -ml -s -bt=dos -oilrtfm -fr=nul -wx
PTLIBZIPPY_LIB = ptlibzippy_l.lib

.C.OBJ:
        $(CC) $(CFLAGS) $[@

all: $(PTLIBZIPPY_LIB) example.exe minigzip.exe

$(PTLIBZIPPY_LIB): $(OBJS)
	wlib -b -c $(PTLIBZIPPY_LIB) -+adler32.obj  -+compress.obj -+crc32.obj
	wlib -b -c $(PTLIBZIPPY_LIB) -+gzclose.obj  -+gzlib.obj    -+gzread.obj   -+gzwrite.obj
        wlib -b -c $(PTLIBZIPPY_LIB) -+deflate.obj  -+infback.obj
        wlib -b -c $(PTLIBZIPPY_LIB) -+inffast.obj  -+inflate.obj  -+inftrees.obj
        wlib -b -c $(PTLIBZIPPY_LIB) -+trees.obj    -+uncompr.obj  -+ptzippyutil.obj

example.exe: $(PTLIBZIPPY_LIB) example.obj
	$(LINKER) -fe=example.exe example.obj $(PTLIBZIPPY_LIB)

minigzip.exe: $(PTLIBZIPPY_LIB) minigzip.obj
	$(LINKER) -fe=minigzip.exe minigzip.obj $(PTLIBZIPPY_LIB)

clean: .SYMBOLIC
          del *.obj
          del $(PTLIBZIPPY_LIB)
          @echo Cleaning done
