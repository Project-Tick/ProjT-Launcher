Name "Restore PTIS Registry"
OutFile RegRestore.exe

SilentInstall silent

XPStyle on

Section
WriteRegStr HKLM SOFTWARE\PTIS "" $PROGRAMFILES\PTIS
WriteRegStr HKCR PTIS.Header\DefaultIcon "" $PROGRAMFILES\PTIS\makeptisw.exe,1
WriteRegStr HKCR PTIS.Script\DefaultIcon "" $PROGRAMFILES\PTIS\makeptisw.exe,1
WriteRegStr HKCR PTIS.Script\shell\compile\command "" '"$PROGRAMFILES\PTIS\makeptisw.exe" "%1"'
WriteRegStr HKCR PTIS.Script\shell\compile-compressor\command "" '"$PROGRAMFILES\PTIS\makeptisw.exe" /ChooseCompressor "%1"'
MessageBox MB_OK Restored!
SectionEnd
