----------------------------------------------------------------
--  PTLib for Ada thick binding.                               --
--                                                            --
--  Copyright (C) 2002-2003 Dmitriy Anisimkov                 --
--                                                            --
--  Open source license information is in the ptlibzippy.ads file.  --
----------------------------------------------------------------

--  $Id: read.adb,v 1.8 2004/05/31 10:53:40 vagul Exp $

--  Test/demo program for the generic read interface.

with Ada.Numerics.Discrete_Random;
with Ada.Streams;
with Ada.Text_IO;

with PTLib;

procedure Read is

   use Ada.Streams;

   ------------------------------------
   --  Test configuration parameters --
   ------------------------------------

   File_Size   : Stream_Element_Offset := 100_000;

   Continuous  : constant Boolean          := False;
   --  If this constant is True, the test would be repeated again and again,
   --  with increment File_Size for every iteration.

   Header      : constant PTLib.Header_Type := PTLib.Default;
   --  Do not use Header other than Default in PTLib versions 1.1.4 and older.

   Init_Random : constant := 8;
   --  We are using the same random sequence, in case of we catch bug,
   --  so we would be able to reproduce it.

   -- End --

   Pack_Size : Stream_Element_Offset;
   Offset    : Stream_Element_Offset;

   Filter     : PTLib.Filter_Type;

   subtype Visible_Symbols
      is Stream_Element range 16#20# .. 16#7E#;

   package Random_Elements is new
      Ada.Numerics.Discrete_Random (Visible_Symbols);

   Gen : Random_Elements.Generator;
   Period  : constant Stream_Element_Offset := 200;
   --  Period constant variable for random generator not to be very random.
   --  Bigger period, harder random.

   Read_Buffer : Stream_Element_Array (1 .. 2048);
   Read_First  : Stream_Element_Offset;
   Read_Last   : Stream_Element_Offset;

   procedure Reset;

   procedure Read
     (Item : out Stream_Element_Array;
      Last : out Stream_Element_Offset);
   --  this procedure is for generic instantiation of
   --  PTLib.Read
   --  reading data from the File_In.

   procedure Read is new PTLib.Read
                           (Read,
                            Read_Buffer,
                            Rest_First => Read_First,
                            Rest_Last  => Read_Last);

   ----------
   -- Read --
   ----------

   procedure Read
     (Item : out Stream_Element_Array;
      Last : out Stream_Element_Offset) is
   begin
      Last := Stream_Element_Offset'Min
               (Item'Last,
                Item'First + File_Size - Offset);

      for J in Item'First .. Last loop
         if J < Item'First + Period then
            Item (J) := Random_Elements.Random (Gen);
         else
            Item (J) := Item (J - Period);
         end if;

         Offset   := Offset + 1;
      end loop;
   end Read;

   -----------
   -- Reset --
   -----------

   procedure Reset is
   begin
      Random_Elements.Reset (Gen, Init_Random);
      Pack_Size := 0;
      Offset := 1;
      Read_First := Read_Buffer'Last + 1;
      Read_Last  := Read_Buffer'Last;
   end Reset;

begin
   Ada.Text_IO.Put_Line ("PTLib " & PTLib.Version);

   loop
      for Level in PTLib.Compression_Level'Range loop

         Ada.Text_IO.Put ("Level ="
            & PTLib.Compression_Level'Image (Level));

         --  Deflate using generic instantiation.

         PTLib.Deflate_Init
               (Filter,
                Level,
                Header => Header);

         Reset;

         Ada.Text_IO.Put
           (Stream_Element_Offset'Image (File_Size) & " ->");

         loop
            declare
               Buffer : Stream_Element_Array (1 .. 1024);
               Last   : Stream_Element_Offset;
            begin
               Read (Filter, Buffer, Last);

               Pack_Size := Pack_Size + Last - Buffer'First + 1;

               exit when Last < Buffer'Last;
            end;
         end loop;

         Ada.Text_IO.Put_Line (Stream_Element_Offset'Image (Pack_Size));

         PTLib.Close (Filter);
      end loop;

      exit when not Continuous;

      File_Size := File_Size + 1;
   end loop;
end Read;
