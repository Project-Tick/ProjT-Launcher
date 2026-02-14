----------------------------------------------------------------
--  PTLib for Ada thick binding.                               --
--                                                            --
--  Copyright (C) 2002-2003 Dmitriy Anisimkov                 --
--                                                            --
--  Open source license information is in the ptlibzippy.ads file.  --
----------------------------------------------------------------

--  $Id: test.adb,v 1.17 2003/08/12 12:13:30 vagul Exp $

--  The program has a few aims.
--  1. Test PTLib.Ada95 thick binding functionality.
--  2. Show the example of use main functionality of the PTLib.Ada95 binding.
--  3. Build this program automatically compile all PTLib.Ada95 packages under
--     GNAT Ada95 compiler.

with PTLib.Streams;
with Ada.Streams.Stream_IO;
with Ada.Numerics.Discrete_Random;

with Ada.Text_IO;

with Ada.Calendar;

procedure Test is

   use Ada.Streams;
   use Stream_IO;

   ------------------------------------
   --  Test configuration parameters --
   ------------------------------------

   File_Size   : Count   := 100_000;
   Continuous  : constant Boolean := False;

   Header      : constant PTLib.Header_Type := PTLib.Default;
                                              --  PTLib.None;
                                              --  PTLib.Auto;
                                              --  PTLib.GZip;
   --  Do not use Header other then Default in PTLib versions 1.1.4
   --  and older.

   Strategy    : constant PTLib.Strategy_Type := PTLib.Default_Strategy;
   Init_Random : constant := 10;

   -- End --

   In_File_Name  : constant String := "testptlibzippy.in";
   --  Name of the input file

   Z_File_Name   : constant String := "testptlibzippy.zlb";
   --  Name of the compressed file.

   Out_File_Name : constant String := "testptlibzippy.out";
   --  Name of the decompressed file.

   File_In   : File_Type;
   File_Out  : File_Type;
   File_Back : File_Type;
   File_Z    : PTLib.Streams.Stream_Type;

   Filter : PTLib.Filter_Type;

   Time_Stamp : Ada.Calendar.Time;

   procedure Generate_File;
   --  Generate file of specified size with some random data.
   --  The random data is repeatable, for the good compression.

   procedure Compare_Streams
     (Left, Right : in out Root_Stream_Type'Class);
   --  The procedure comparing data in 2 streams.
   --  It is for compare data before and after compression/decompression.

   procedure Compare_Files (Left, Right : String);
   --  Compare files. Based on the Compare_Streams.

   procedure Copy_Streams
     (Source, Target : in out Root_Stream_Type'Class;
      Buffer_Size    : in     Stream_Element_Offset := 1024);
   --  Copying data from one stream to another. It is for test stream
   --  interface of the library.

   procedure Data_In
     (Item : out Stream_Element_Array;
      Last : out Stream_Element_Offset);
   --  this procedure is for generic instantiation of
   --  PTLib.Generic_Translate.
   --  reading data from the File_In.

   procedure Data_Out (Item : in Stream_Element_Array);
   --  this procedure is for generic instantiation of
   --  PTLib.Generic_Translate.
   --  writing data to the File_Out.

   procedure Stamp;
   --  Store the timestamp to the local variable.

   procedure Print_Statistic (Msg : String; Data_Size : PTLib.Count);
   --  Print the time statistic with the message.

   procedure Translate is new PTLib.Generic_Translate
                                (Data_In  => Data_In,
                                 Data_Out => Data_Out);
   --  This procedure is moving data from File_In to File_Out
   --  with compression or decompression, depend on initialization of
   --  Filter parameter.

   -------------------
   -- Compare_Files --
   -------------------

   procedure Compare_Files (Left, Right : String) is
      Left_File, Right_File : File_Type;
   begin
      Open (Left_File, In_File, Left);
      Open (Right_File, In_File, Right);
      Compare_Streams (Stream (Left_File).all, Stream (Right_File).all);
      Close (Left_File);
      Close (Right_File);
   end Compare_Files;

   ---------------------
   -- Compare_Streams --
   ---------------------

   procedure Compare_Streams
     (Left, Right : in out Ada.Streams.Root_Stream_Type'Class)
   is
      Left_Buffer, Right_Buffer : Stream_Element_Array (0 .. 16#FFF#);
      Left_Last, Right_Last : Stream_Element_Offset;
   begin
      loop
         Read (Left, Left_Buffer, Left_Last);
         Read (Right, Right_Buffer, Right_Last);

         if Left_Last /= Right_Last then
            Ada.Text_IO.Put_Line ("Compare error :"
              & Stream_Element_Offset'Image (Left_Last)
              & " /= "
              & Stream_Element_Offset'Image (Right_Last));

            raise Constraint_Error;

         elsif Left_Buffer (0 .. Left_Last)
               /= Right_Buffer (0 .. Right_Last)
         then
            Ada.Text_IO.Put_Line ("ERROR: IN and OUT files is not equal.");
            raise Constraint_Error;

         end if;

         exit when Left_Last < Left_Buffer'Last;
      end loop;
   end Compare_Streams;

   ------------------
   -- Copy_Streams --
   ------------------

   procedure Copy_Streams
     (Source, Target : in out Ada.Streams.Root_Stream_Type'Class;
      Buffer_Size    : in     Stream_Element_Offset := 1024)
   is
      Buffer : Stream_Element_Array (1 .. Buffer_Size);
      Last   : Stream_Element_Offset;
   begin
      loop
         Read  (Source, Buffer, Last);
         Write (Target, Buffer (1 .. Last));

         exit when Last < Buffer'Last;
      end loop;
   end Copy_Streams;

   -------------
   -- Data_In --
   -------------

   procedure Data_In
     (Item : out Stream_Element_Array;
      Last : out Stream_Element_Offset) is
   begin
      Read (File_In, Item, Last);
   end Data_In;

   --------------
   -- Data_Out --
   --------------

   procedure Data_Out (Item : in Stream_Element_Array) is
   begin
      Write (File_Out, Item);
   end Data_Out;

   -------------------
   -- Generate_File --
   -------------------

   procedure Generate_File is
      subtype Visible_Symbols is Stream_Element range 16#20# .. 16#7E#;

      package Random_Elements is
         new Ada.Numerics.Discrete_Random (Visible_Symbols);

      Gen    : Random_Elements.Generator;
      Buffer : Stream_Element_Array := (1 .. 77 => 16#20#) & 10;

      Buffer_Count : constant Count := File_Size / Buffer'Length;
      --  Number of same buffers in the packet.

      Density : constant Count := 30; --  from 0 to Buffer'Length - 2;

      procedure Fill_Buffer (J, D : in Count);
      --  Change the part of the buffer.

      -----------------
      -- Fill_Buffer --
      -----------------

      procedure Fill_Buffer (J, D : in Count) is
      begin
         for K in 0 .. D loop
            Buffer
              (Stream_Element_Offset ((J + K) mod (Buffer'Length - 1) + 1))
             := Random_Elements.Random (Gen);

         end loop;
      end Fill_Buffer;

   begin
      Random_Elements.Reset (Gen, Init_Random);

      Create (File_In, Out_File, In_File_Name);

      Fill_Buffer (1, Buffer'Length - 2);

      for J in 1 .. Buffer_Count loop
         Write (File_In, Buffer);

         Fill_Buffer (J, Density);
      end loop;

      --  fill remain size.

      Write
        (File_In,
         Buffer
           (1 .. Stream_Element_Offset
                   (File_Size - Buffer'Length * Buffer_Count)));

      Flush (File_In);
      Close (File_In);
   end Generate_File;

   ---------------------
   -- Print_Statistic --
   ---------------------

   procedure Print_Statistic (Msg : String; Data_Size : PTLib.Count) is
      use Ada.Calendar;
      use Ada.Text_IO;

      package Count_IO is new Integer_IO (PTLib.Count);

      Curr_Dur : Duration := Clock - Time_Stamp;
   begin
      Put (Msg);

      Set_Col (20);
      Ada.Text_IO.Put ("size =");

      Count_IO.Put
        (Data_Size,
         Width => Stream_IO.Count'Image (File_Size)'Length);

      Put_Line (" duration =" & Duration'Image (Curr_Dur));
   end Print_Statistic;

   -----------
   -- Stamp --
   -----------

   procedure Stamp is
   begin
      Time_Stamp := Ada.Calendar.Clock;
   end Stamp;

begin
   Ada.Text_IO.Put_Line ("PTLib " & PTLib.Version);

   loop
      Generate_File;

      for Level in PTLib.Compression_Level'Range loop

         Ada.Text_IO.Put_Line ("Level ="
            & PTLib.Compression_Level'Image (Level));

         --  Test generic interface.
         Open   (File_In, In_File, In_File_Name);
         Create (File_Out, Out_File, Z_File_Name);

         Stamp;

         --  Deflate using generic instantiation.

         PTLib.Deflate_Init
               (Filter   => Filter,
                Level    => Level,
                Strategy => Strategy,
                Header   => Header);

         Translate (Filter);
         Print_Statistic ("Generic compress", PTLib.Total_Out (Filter));
         PTLib.Close (Filter);

         Close (File_In);
         Close (File_Out);

         Open   (File_In, In_File, Z_File_Name);
         Create (File_Out, Out_File, Out_File_Name);

         Stamp;

         --  Inflate using generic instantiation.

         PTLib.Inflate_Init (Filter, Header => Header);

         Translate (Filter);
         Print_Statistic ("Generic decompress", PTLib.Total_Out (Filter));

         PTLib.Close (Filter);

         Close (File_In);
         Close (File_Out);

         Compare_Files (In_File_Name, Out_File_Name);

         --  Test stream interface.

         --  Compress to the back stream.

         Open   (File_In, In_File, In_File_Name);
         Create (File_Back, Out_File, Z_File_Name);

         Stamp;

         PTLib.Streams.Create
           (Stream          => File_Z,
            Mode            => PTLib.Streams.Out_Stream,
            Back            => PTLib.Streams.Stream_Access
                                 (Stream (File_Back)),
            Back_Compressed => True,
            Level           => Level,
            Strategy        => Strategy,
            Header          => Header);

         Copy_Streams
           (Source => Stream (File_In).all,
            Target => File_Z);

         --  Flushing internal buffers to the back stream.

         PTLib.Streams.Flush (File_Z, PTLib.Finish);

         Print_Statistic ("Write compress",
                          PTLib.Streams.Write_Total_Out (File_Z));

         PTLib.Streams.Close (File_Z);

         Close (File_In);
         Close (File_Back);

         --  Compare reading from original file and from
         --  decompression stream.

         Open (File_In,   In_File, In_File_Name);
         Open (File_Back, In_File, Z_File_Name);

         PTLib.Streams.Create
           (Stream          => File_Z,
            Mode            => PTLib.Streams.In_Stream,
            Back            => PTLib.Streams.Stream_Access
                                 (Stream (File_Back)),
            Back_Compressed => True,
            Header          => Header);

         Stamp;
         Compare_Streams (Stream (File_In).all, File_Z);

         Print_Statistic ("Read decompress",
                          PTLib.Streams.Read_Total_Out (File_Z));

         PTLib.Streams.Close (File_Z);
         Close (File_In);
         Close (File_Back);

         --  Compress by reading from compression stream.

         Open (File_Back, In_File, In_File_Name);
         Create (File_Out, Out_File, Z_File_Name);

         PTLib.Streams.Create
           (Stream          => File_Z,
            Mode            => PTLib.Streams.In_Stream,
            Back            => PTLib.Streams.Stream_Access
                                 (Stream (File_Back)),
            Back_Compressed => False,
            Level           => Level,
            Strategy        => Strategy,
            Header          => Header);

         Stamp;
         Copy_Streams
           (Source => File_Z,
            Target => Stream (File_Out).all);

         Print_Statistic ("Read compress",
                          PTLib.Streams.Read_Total_Out (File_Z));

         PTLib.Streams.Close (File_Z);

         Close (File_Out);
         Close (File_Back);

         --  Decompress to decompression stream.

         Open   (File_In,   In_File, Z_File_Name);
         Create (File_Back, Out_File, Out_File_Name);

         PTLib.Streams.Create
           (Stream          => File_Z,
            Mode            => PTLib.Streams.Out_Stream,
            Back            => PTLib.Streams.Stream_Access
                                 (Stream (File_Back)),
            Back_Compressed => False,
            Header          => Header);

         Stamp;

         Copy_Streams
           (Source => Stream (File_In).all,
            Target => File_Z);

         Print_Statistic ("Write decompress",
                          PTLib.Streams.Write_Total_Out (File_Z));

         PTLib.Streams.Close (File_Z);
         Close (File_In);
         Close (File_Back);

         Compare_Files (In_File_Name, Out_File_Name);
      end loop;

      Ada.Text_IO.Put_Line (Count'Image (File_Size) & " Ok.");

      exit when not Continuous;

      File_Size := File_Size + 1;
   end loop;
end Test;
