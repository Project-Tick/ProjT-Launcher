----------------------------------------------------------------
--  PTLib for Ada thick binding.                               --
--                                                            --
--  Copyright (C) 2002-2003 Dmitriy Anisimkov                 --
--                                                            --
--  Open source license information is in the ptlibzippy.ads file.  --
----------------------------------------------------------------

--  $Id: ptlibzippy-thin.ads,v 1.11 2004/07/23 06:33:11 vagul Exp $

with Interfaces.C.Strings;

with System;

private package PTLib.Thin is

   --  From ptzippyconf.h

   MAX_MEM_LEVEL : constant := 9;         --  ptzippyconf.h:105
                                          --  ptzippyconf.h:105
   MAX_WBITS : constant := 15;      --  ptzippyconf.h:115
                                    --  32K LZ77 window
                                    --  ptzippyconf.h:115
   SEEK_SET : constant := 8#0000#;  --  ptzippyconf.h:244
                                    --  Seek from beginning of file.
                                    --  ptzippyconf.h:244
   SEEK_CUR : constant := 1;        --  ptzippyconf.h:245
                                    --  Seek from current position.
                                    --  ptzippyconf.h:245
   SEEK_END : constant := 2;        --  ptzippyconf.h:246
                                    --  Set file pointer to EOF plus "offset"
                                    --  ptzippyconf.h:246

   type Byte is new Interfaces.C.unsigned_char; --  8 bits
                                                --  ptzippyconf.h:214
   type UInt is new Interfaces.C.unsigned;      --  16 bits or more
                                                --  ptzippyconf.h:216
   type Int is new Interfaces.C.int;

   type ULong is new Interfaces.C.unsigned_long;     --  32 bits or more
                                                     --  ptzippyconf.h:217
   subtype Chars_Ptr is Interfaces.C.Strings.chars_ptr;

   type ULong_Access is access ULong;
   type Int_Access is access Int;

   subtype Voidp is System.Address;            --  ptzippyconf.h:232

   subtype Byte_Access is Voidp;

   Nul : constant Voidp := System.Null_Address;
   --  end from zconf

   Z_NO_FLUSH : constant := 8#0000#;   --  ptlibzippy.h:125
                                       --  ptlibzippy.h:125
   Z_PARTIAL_FLUSH : constant := 1;       --  ptlibzippy.h:126
                                          --  will be removed, use
                                          --  Z_SYNC_FLUSH instead
                                          --  ptlibzippy.h:126
   Z_SYNC_FLUSH : constant := 2;       --  ptlibzippy.h:127
                                       --  ptlibzippy.h:127
   Z_FULL_FLUSH : constant := 3;       --  ptlibzippy.h:128
                                       --  ptlibzippy.h:128
   Z_FINISH : constant := 4;        --  ptlibzippy.h:129
                                    --  ptlibzippy.h:129
   Z_OK : constant := 8#0000#;   --  ptlibzippy.h:132
                                 --  ptlibzippy.h:132
   Z_STREAM_END : constant := 1;       --  ptlibzippy.h:133
                                       --  ptlibzippy.h:133
   Z_NEED_DICT : constant := 2;        --  ptlibzippy.h:134
                                       --  ptlibzippy.h:134
   Z_ERRNO : constant := -1;        --  ptlibzippy.h:135
                                    --  ptlibzippy.h:135
   Z_STREAM_ERROR : constant := -2;       --  ptlibzippy.h:136
                                          --  ptlibzippy.h:136
   Z_DATA_ERROR : constant := -3;      --  ptlibzippy.h:137
                                       --  ptlibzippy.h:137
   Z_MEM_ERROR : constant := -4;       --  ptlibzippy.h:138
                                       --  ptlibzippy.h:138
   Z_BUF_ERROR : constant := -5;       --  ptlibzippy.h:139
                                       --  ptlibzippy.h:139
   Z_VERSION_ERROR : constant := -6;      --  ptlibzippy.h:140
                                          --  ptlibzippy.h:140
   Z_NO_COMPRESSION : constant := 8#0000#;   --  ptlibzippy.h:145
                                             --  ptlibzippy.h:145
   Z_BEST_SPEED : constant := 1;       --  ptlibzippy.h:146
                                       --  ptlibzippy.h:146
   Z_BEST_COMPRESSION : constant := 9;       --  ptlibzippy.h:147
                                             --  ptlibzippy.h:147
   Z_DEFAULT_COMPRESSION : constant := -1;      --  ptlibzippy.h:148
                                                --  ptlibzippy.h:148
   Z_FILTERED : constant := 1;      --  ptlibzippy.h:151
                                    --  ptlibzippy.h:151
   Z_HUFFMAN_ONLY : constant := 2;        --  ptlibzippy.h:152
                                          --  ptlibzippy.h:152
   Z_DEFAULT_STRATEGY : constant := 8#0000#; --  ptlibzippy.h:153
                                             --  ptlibzippy.h:153
   Z_BINARY : constant := 8#0000#;  --  ptlibzippy.h:156
                                    --  ptlibzippy.h:156
   Z_ASCII : constant := 1;      --  ptlibzippy.h:157
                                 --  ptlibzippy.h:157
   Z_UNKNOWN : constant := 2;       --  ptlibzippy.h:158
                                    --  ptlibzippy.h:158
   Z_DEFLATED : constant := 8;      --  ptlibzippy.h:161
                                    --  ptlibzippy.h:161
   Z_NULL : constant := 8#0000#; --  ptlibzippy.h:164
                                 --  for initializing zalloc, zfree, opaque
                                 --  ptlibzippy.h:164
   type gzFile is new Voidp;                  --  ptlibzippy.h:646

   type Z_Stream is private;

   type Z_Streamp is access all Z_Stream;     --  ptlibzippy.h:89

   type alloc_func is access function
     (Opaque : Voidp;
      Items  : UInt;
      Size   : UInt)
      return Voidp; --  ptlibzippy.h:63

   type free_func is access procedure (opaque : Voidp; address : Voidp);

   function ptlibzippyVersion return Chars_Ptr;

   function Deflate (strm : Z_Streamp; flush : Int) return Int;

   function DeflateEnd (strm : Z_Streamp) return Int;

   function Inflate (strm : Z_Streamp; flush : Int) return Int;

   function InflateEnd (strm : Z_Streamp) return Int;

   function deflateSetDictionary
     (strm       : Z_Streamp;
      dictionary : Byte_Access;
      dictLength : UInt)
      return       Int;

   function deflateCopy (dest : Z_Streamp; source : Z_Streamp) return Int;
   --  ptlibzippy.h:478

   function deflateReset (strm : Z_Streamp) return Int; -- ptlibzippy.h:495

   function deflateParams
     (strm     : Z_Streamp;
      level    : Int;
      strategy : Int)
      return     Int;       -- ptlibzippy.h:506

   function inflateSetDictionary
     (strm       : Z_Streamp;
      dictionary : Byte_Access;
      dictLength : UInt)
      return       Int; --  ptlibzippy.h:548

   function inflateSync (strm : Z_Streamp) return Int;  --  ptlibzippy.h:565

   function inflateReset (strm : Z_Streamp) return Int; --  ptlibzippy.h:580

   function compress
     (dest      : Byte_Access;
      destLen   : ULong_Access;
      source    : Byte_Access;
      sourceLen : ULong)
      return      Int;           -- ptlibzippy.h:601

   function compress2
     (dest      : Byte_Access;
      destLen   : ULong_Access;
      source    : Byte_Access;
      sourceLen : ULong;
      level     : Int)
      return      Int;          -- ptlibzippy.h:615

   function uncompress
     (dest      : Byte_Access;
      destLen   : ULong_Access;
      source    : Byte_Access;
      sourceLen : ULong)
      return      Int;

   function gzopen (path : Chars_Ptr; mode : Chars_Ptr) return gzFile;

   function gzdopen (fd : Int; mode : Chars_Ptr) return gzFile;

   function gzsetparams
     (file     : gzFile;
      level    : Int;
      strategy : Int)
      return     Int;

   function gzread
     (file : gzFile;
      buf  : Voidp;
      len  : UInt)
      return Int;

   function gzwrite
     (file : in gzFile;
      buf  : in Voidp;
      len  : in UInt)
      return Int;

   function gzprintf (file : in gzFile; format : in Chars_Ptr) return Int;

   function gzputs (file : in gzFile; s : in Chars_Ptr) return Int;

   function gzgets
     (file : gzFile;
      buf  : Chars_Ptr;
      len  : Int)
      return Chars_Ptr;

   function gzputc (file : gzFile; char : Int) return Int;

   function gzgetc (file : gzFile) return Int;

   function gzflush (file : gzFile; flush : Int) return Int;

   function gzseek
     (file   : gzFile;
      offset : Int;
      whence : Int)
      return   Int;

   function gzrewind (file : gzFile) return Int;

   function gztell (file : gzFile) return Int;

   function gzeof (file : gzFile) return Int;

   function gzclose (file : gzFile) return Int;

   function gzerror (file : gzFile; errnum : Int_Access) return Chars_Ptr;

   function adler32
     (adler : ULong;
      buf   : Byte_Access;
      len   : UInt)
      return  ULong;

   function crc32
     (crc  : ULong;
      buf  : Byte_Access;
      len  : UInt)
      return ULong;

   function deflateInit
     (strm        : Z_Streamp;
      level       : Int;
      version     : Chars_Ptr;
      stream_size : Int)
      return        Int;

   function deflateInit2
     (strm        : Z_Streamp;
      level       : Int;
      method      : Int;
      windowBits  : Int;
      memLevel    : Int;
      strategy    : Int;
      version     : Chars_Ptr;
      stream_size : Int)
      return        Int;

   function Deflate_Init
     (strm       : Z_Streamp;
      level      : Int;
      method     : Int;
      windowBits : Int;
      memLevel   : Int;
      strategy   : Int)
      return       Int;
   pragma Inline (Deflate_Init);

   function inflateInit
     (strm        : Z_Streamp;
      version     : Chars_Ptr;
      stream_size : Int)
      return        Int;

   function inflateInit2
     (strm        : in Z_Streamp;
      windowBits  : in Int;
      version     : in Chars_Ptr;
      stream_size : in Int)
      return      Int;

   function inflateBackInit
     (strm        : in Z_Streamp;
      windowBits  : in Int;
      window      : in Byte_Access;
      version     : in Chars_Ptr;
      stream_size : in Int)
      return      Int;
   --  Size of window have to be 2**windowBits.

   function Inflate_Init (strm : Z_Streamp; windowBits : Int) return Int;
   pragma Inline (Inflate_Init);

   function ptError (err : Int) return Chars_Ptr;

   function inflateSyncPoint (z : Z_Streamp) return Int;

   function get_crc_table return ULong_Access;

   --  Interface to the available fields of the z_stream structure.
   --  The application must update next_in and avail_in when avail_in has
   --  dropped to zero. It must update next_out and avail_out when avail_out
   --  has dropped to zero. The application must initialize zalloc, zfree and
   --  opaque before calling the init function.

   procedure Set_In
     (Strm   : in out Z_Stream;
      Buffer : in Voidp;
      Size   : in UInt);
   pragma Inline (Set_In);

   procedure Set_Out
     (Strm   : in out Z_Stream;
      Buffer : in Voidp;
      Size   : in UInt);
   pragma Inline (Set_Out);

   procedure Set_Mem_Func
     (Strm   : in out Z_Stream;
      Opaque : in Voidp;
      Alloc  : in alloc_func;
      Free   : in free_func);
   pragma Inline (Set_Mem_Func);

   function Last_Error_Message (Strm : in Z_Stream) return String;
   pragma Inline (Last_Error_Message);

   function Avail_Out (Strm : in Z_Stream) return UInt;
   pragma Inline (Avail_Out);

   function Avail_In (Strm : in Z_Stream) return UInt;
   pragma Inline (Avail_In);

   function Total_In (Strm : in Z_Stream) return ULong;
   pragma Inline (Total_In);

   function Total_Out (Strm : in Z_Stream) return ULong;
   pragma Inline (Total_Out);

   function inflateCopy
     (dest   : in Z_Streamp;
      Source : in Z_Streamp)
      return Int;

   function compressBound (Source_Len : in ULong) return ULong;

   function deflateBound
     (Strm       : in Z_Streamp;
      Source_Len : in ULong)
      return     ULong;

   function gzungetc (C : in Int; File : in  gzFile) return Int;

   function ptlibzippyCompileFlags return ULong;

private

   type Z_Stream is record            -- ptlibzippy.h:68
      Next_In   : Voidp      := Nul;  -- next input byte
      Avail_In  : UInt       := 0;    -- number of bytes available at next_in
      Total_In  : ULong      := 0;    -- total nb of input bytes read so far
      Next_Out  : Voidp      := Nul;  -- next output byte should be put there
      Avail_Out : UInt       := 0;    -- remaining free space at next_out
      Total_Out : ULong      := 0;    -- total nb of bytes output so far
      msg       : Chars_Ptr;          -- last error message, NULL if no error
      state     : Voidp;              -- not visible by applications
      zalloc    : alloc_func := null; -- used to allocate the internal state
      zfree     : free_func  := null; -- used to free the internal state
      opaque    : Voidp;              -- private data object passed to
                                      --  zalloc and zfree
      data_type : Int;                -- best guess about the data type:
                                      --  ascii or binary
      adler     : ULong;              -- adler32 value of the uncompressed
                                      --  data
      reserved  : ULong;              -- reserved for future use
   end record;

   pragma Convention (C, Z_Stream);

   pragma Import (C, ptlibzippyVersion, "ptlibzippyVersion");
   pragma Import (C, Deflate, "deflate");
   pragma Import (C, DeflateEnd, "deflateEnd");
   pragma Import (C, Inflate, "inflate");
   pragma Import (C, InflateEnd, "inflateEnd");
   pragma Import (C, deflateSetDictionary, "deflateSetDictionary");
   pragma Import (C, deflateCopy, "deflateCopy");
   pragma Import (C, deflateReset, "deflateReset");
   pragma Import (C, deflateParams, "deflateParams");
   pragma Import (C, inflateSetDictionary, "inflateSetDictionary");
   pragma Import (C, inflateSync, "inflateSync");
   pragma Import (C, inflateReset, "inflateReset");
   pragma Import (C, compress, "compress");
   pragma Import (C, compress2, "compress2");
   pragma Import (C, uncompress, "uncompress");
   pragma Import (C, gzopen, "gzopen");
   pragma Import (C, gzdopen, "gzdopen");
   pragma Import (C, gzsetparams, "gzsetparams");
   pragma Import (C, gzread, "gzread");
   pragma Import (C, gzwrite, "gzwrite");
   pragma Import (C, gzprintf, "gzprintf");
   pragma Import (C, gzputs, "gzputs");
   pragma Import (C, gzgets, "gzgets");
   pragma Import (C, gzputc, "gzputc");
   pragma Import (C, gzgetc, "gzgetc");
   pragma Import (C, gzflush, "gzflush");
   pragma Import (C, gzseek, "gzseek");
   pragma Import (C, gzrewind, "gzrewind");
   pragma Import (C, gztell, "gztell");
   pragma Import (C, gzeof, "gzeof");
   pragma Import (C, gzclose, "gzclose");
   pragma Import (C, gzerror, "gzerror");
   pragma Import (C, adler32, "adler32");
   pragma Import (C, crc32, "crc32");
   pragma Import (C, deflateInit, "deflateInit_");
   pragma Import (C, inflateInit, "inflateInit_");
   pragma Import (C, deflateInit2, "deflateInit2_");
   pragma Import (C, inflateInit2, "inflateInit2_");
   pragma Import (C, ptError, "ptError");
   pragma Import (C, inflateSyncPoint, "inflateSyncPoint");
   pragma Import (C, get_crc_table, "get_crc_table");

   --  since PTlibzippy 1.2.0:

   pragma Import (C, inflateCopy, "inflateCopy");
   pragma Import (C, compressBound, "compressBound");
   pragma Import (C, deflateBound, "deflateBound");
   pragma Import (C, gzungetc, "gzungetc");
   pragma Import (C, ptlibzippyCompileFlags, "ptlibzippyCompileFlags");

   pragma Import (C, inflateBackInit, "inflateBackInit_");

   --  I stopped binding the inflateBack routines, because realize that
   --  it does not support PTlibzippy and gzip headers for now, and have no
   --  symmetric deflateBack routines.
   --  PTLib-Ada is symmetric regarding deflate/inflate data transformation
   --  and has a similar generic callback interface for the
   --  deflate/inflate transformation based on the regular Deflate/Inflate
   --  routines.

   --  pragma Import (C, inflateBack, "inflateBack");
   --  pragma Import (C, inflateBackEnd, "inflateBackEnd");

end PTLib.Thin;
