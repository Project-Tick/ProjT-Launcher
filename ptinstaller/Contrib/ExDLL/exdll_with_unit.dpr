{
  PTIS ExDLL2 example
  Original is ExDLL
  (C) 2001 - Peter Windridge

  Changed with delphi unit ptis.pas
  by bernhard mayer

  Tested in Delphi 7.0
}

// Example PTIS code
{
Section
  exdll_with_unit::registerplugincallback

  StrCpy $3 "Hello"
  Push "World"
  exdll_with_unit::pop_dlg_push
  Pop $1
  DetailPrint $$1=$1
  DetailPrint $$3=$3

  GetFunctionAddress $0 ptistest
  Push $0
  exdll_with_unit::callptisfunc
SectionEnd

Function ptistest
  DetailPrint "Hello from PTIS function"
FunctionEnd
}


library exdll;

uses
  ptis, windows;

{$IFDEF UNICODE}
type TString = System.WideString; // UnicodeString?
type PTChar = PWideChar;
{$ELSE}
type TString = AnsiString;
type PTChar = PAnsiChar;
{$ENDIF}

procedure pop_dlg_push(const hwndParent: HWND; const string_size: integer; const variables: PTChar; const stacktop: pointer); cdecl;
begin
  // set up global variables
  Init(hwndParent, string_size, variables, stacktop);

  PTISDialog(GetUserVariable(INST_3), 'The value of $3', MB_OK);
  PTISDialog(PopString, 'pop', MB_OK);
  PushString('Hello, this is a push');
  SetUserVariable(INST_3, 'This is user var $3');
end;


procedure callptisfunc(const hwndParent: HWND; const string_size: integer; const variables: PTChar; const stacktop: pointer; const extraparameters: pointer); cdecl;
var
  FuncAddr : TString;
begin
  Init(hwndParent, string_size, variables, stacktop, extraparameters);

  FuncAddr := PopString();
  Call(FuncAddr);
end;


function myptiscallback(const NSPIM: TNSPIM): Pointer; cdecl;
begin
  Result := nil;
  if NSPIM = NSPIM_UNLOAD then
    begin
      PTISDialog(PTChar('NSPIM_UNLOAD is the final callback, goodbye...'), PTChar('myptiscallback'), MB_OK);
    end;
end;


procedure registerplugincallback(const hwndParent: HWND; const string_size: integer; const variables: PTChar; const stacktop: pointer; const extraparameters: pointer); cdecl;
var
  ThisDllInstance : HMODULE;
begin
  Init(hwndParent, string_size, variables, stacktop, extraparameters);

  if g_extraparameters <> nil then
  begin
    ThisDllInstance := hInstance;
    TRegisterPluginCallback(g_extraparameters.RegisterPluginCallback)(ThisDllInstance, @myptiscallback);
  end;
end;


exports pop_dlg_push;
exports callptisfunc;
exports registerplugincallback;

begin
end.
