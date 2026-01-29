OutFile icon1.exe
Name icon1

Icon "${PTISDIR}\Contrib\Graphics\Icons\arrow-install.ico"
UninstallIcon "${PTISDIR}\Contrib\Graphics\Icons\ptis1-install.ico"

Section
Return
WriteUninstaller $TEMP\uninst.exe
SectionEnd

Section uninstall
SectionEnd
