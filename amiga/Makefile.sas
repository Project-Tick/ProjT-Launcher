# SMakefile for PTlibzippy
# Modified from the standard UNIX Makefile Copyright Jean-loup Gailly
# Osma Ahvenlampi <Osma.Ahvenlampi@hut.fi>
# Amiga, SAS/C 6.56 & Smake

CC=sc
CFLAGS=OPT
#CFLAGS=OPT CPU=68030
#CFLAGS=DEBUG=LINE
LDFLAGS=LIB z.lib

SCOPTIONS=OPTSCHED OPTINLINE OPTALIAS OPTTIME OPTINLOCAL STRMERGE \
       NOICONS PARMS=BOTH NOSTACKCHECK UTILLIB NOVERSION ERRORREXX \
       DEF=POSTINC

OBJS = adler32.o compress.o crc32.o gzclose.o gzlib.o gzread.o gzwrite.o \
       uncompr.o deflate.o trees.o ptzippyutil.o inflate.o infback.o inftrees.o inffast.o

TEST_OBJS = example.o minigzip.o

all: SCOPTIONS example minigzip

check: test
test: all
	example
	echo hello world | minigzip | minigzip -d

install: z.lib
	copy clone ptlibzippy.h ptzippyconf.h INCLUDE:
	copy clone z.lib LIB:

z.lib: $(OBJS)
	oml z.lib r $(OBJS)

example: example.o z.lib
	$(CC) $(CFLAGS) LINK TO $@ example.o $(LDFLAGS)

minigzip: minigzip.o z.lib
	$(CC) $(CFLAGS) LINK TO $@ minigzip.o $(LDFLAGS)

mostlyclean: clean
clean:
	-delete force quiet example minigzip *.o z.lib foo.gz *.lnk SCOPTIONS

SCOPTIONS: Makefile.sas
	copy to $@ <from <
$(SCOPTIONS)
<

# DO NOT DELETE THIS LINE -- make depend depends on it.

adler32.o: ptlibzippy.h ptzippyconf.h
compress.o: ptlibzippy.h ptzippyconf.h
crc32.o: crc32.h ptlibzippy.h ptzippyconf.h
deflate.o: deflate.h ptzippyutil.h ptlibzippy.h ptzippyconf.h
example.o: ptlibzippy.h ptzippyconf.h
gzclose.o: ptlibzippy.h ptzippyconf.h ptzippyguts.h
gzlib.o: ptlibzippy.h ptzippyconf.h ptzippyguts.h
gzread.o: ptlibzippy.h ptzippyconf.h ptzippyguts.h
gzwrite.o: ptlibzippy.h ptzippyconf.h ptzippyguts.h
inffast.o: ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h inflate.h inffast.h
inflate.o: ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h inflate.h inffast.h
infback.o: ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h inflate.h inffast.h
inftrees.o: ptzippyutil.h ptlibzippy.h ptzippyconf.h inftrees.h
minigzip.o: ptlibzippy.h ptzippyconf.h
trees.o: deflate.h ptzippyutil.h ptlibzippy.h ptzippyconf.h trees.h
uncompr.o: ptlibzippy.h ptzippyconf.h
ptzippyutil.o: ptzippyutil.h ptlibzippy.h ptzippyconf.h
