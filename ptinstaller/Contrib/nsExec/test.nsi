Name "nsExec Test"

OutFile "nsExec Test.exe"

ShowInstDetails show

Section "Silent MakePTIS"
	nsExec::Exec '"${PTISDIR}\makeptis.exe"'
	Pop $0 # return value/error/timeout
	DetailPrint ""
	DetailPrint "       Return value: $0"
	DetailPrint ""
SectionEnd

Section "MakePTIS commands help"
	nsExec::ExecToLog '"${PTISDIR}\makeptis.exe" /CMDHELP'
	Pop $0 # return value/error/timeout
	DetailPrint ""
	DetailPrint "       Return value: $0"
	DetailPrint ""
SectionEnd

Section "Output to variable"
	nsExec::ExecToStack '"${PTISDIR}\makeptis.exe" /VERSION'
	Pop $0 # return value/error/timeout
	Pop $1 # printed text, up to ${PTIS_MAX_STRLEN}
	DetailPrint '"${PTISDIR}\makeptis.exe" /VERSION printed: $1'
	DetailPrint ""
	DetailPrint "       Return value: $0"
	DetailPrint ""
SectionEnd