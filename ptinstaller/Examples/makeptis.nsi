;PTIS Setup Script
;--------------------------------

!pragma warning error all
!pragma warning warning 7010 ; File /NonFatal

!ifdef VER_MAJOR & VER_MINOR
  !define /ifndef VER_REVISION 0
  !define /ifndef VER_BUILD 0
!endif

!define /ifndef VERSION 'anonymous-build'

;--------------------------------
;Configuration

!if ${PTIS_PTR_SIZE} > 4
  !define BITS 64
  !define NAMESUFFIX " (64 bit)"
!else
  !define BITS 32
  !define NAMESUFFIX ""
!endif

!ifndef OUTFILE
  !define OUTFILE "..\ptis${BITS}-${VERSION}-setup.exe"
  !searchreplace OUTFILE "${OUTFILE}" ptis32 ptis
!endif

OutFile "${OUTFILE}"
Unicode true
SetCompressor /SOLID lzma

InstType "Full"
InstType "Lite"
InstType "Minimal"

InstallDir $PROGRAMFILES${BITS}\PTIS
InstallDirRegKey HKLM Software\PTIS ""

RequestExecutionLevel admin

;--------------------------------
;Header Files

!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
!include "Memento.nsh"
!include "WordFunc.nsh"
!include "Util.nsh"
!include "Integration.nsh"

;--------------------------------
;Configuration

;Names
Name "PTIS"
Caption "PTIS ${VERSION}${NAMESUFFIX} Setup"

!define REG_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\PTIS"

;Memento Settings
!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY "${REG_UNINST_KEY}"

;Interface Settings
!define MUI_ABORTWARNING

!define MUI_ICON "${PTISDIR}\Contrib\Graphics\Icons\ptis3-install.ico"
!define MUI_UNICON "${PTISDIR}\Contrib\Graphics\Icons\ptis3-uninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${PTISDIR}\Contrib\Graphics\Header\ptis3-branding-r.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${PTISDIR}\Contrib\Graphics\Wizard\ptis3-branding.bmp"

!define MUI_COMPONENTSPAGE_SMALLDESC

;Pages
!define MUI_WELCOMEPAGE_TITLE "Welcome to the PTIS ${VERSION} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of PTIS (Nullsoft Scriptable Install System) ${VERSION}, the next generation of the Windows installer and uninstaller system that doesn't suck and isn't huge.$\r$\n$\r$\nPTIS includes a Modern User Interface, LZMA compression, support for multiple languages and an easy plug-in system.$\r$\n$\r$\n$_CLICK"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\COPYING"
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
Page custom PageReinstall PageLeaveReinstall
!endif
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_LINK "Visit the PTIS site for the latest news, FAQs and support"
!define MUI_FINISHPAGE_LINK_LOCATION "http://ptis.sf.net/"

!define MUI_FINISHPAGE_RUN "$INSTDIR\PTIS.exe"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT

!define MUI_FINISHPAGE_SHOWREADME
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Show release notes"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION ShowReleaseNotes

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Version information

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
VIProductVersion ${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD}
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "FileDescription" "PTIS Setup"
VIAddVersionKey "LegalCopyright" "http://ptis.sf.net/License"
!endif

;--------------------------------
;Installer Sections

!macro InstallPlugin pi
  !if ${BITS} >= 64
    File "/oname=$InstDir\Plugins\amd64-unicode\${pi}.dll" ..\Plugins\amd64-unicode\${pi}.dll
  !else
    File "/oname=$InstDir\Plugins\x86-ansi\${pi}.dll" ..\Plugins\x86-ansi\${pi}.dll
    File "/oname=$InstDir\Plugins\x86-unicode\${pi}.dll" ..\Plugins\x86-unicode\${pi}.dll
  !endif
!macroend

!macro InstallStub stub
  !if ${BITS} >= 64
    File ..\Stubs\${stub}-amd64-unicode
  !else
    File ..\Stubs\${stub}-x86-ansi
    File ..\Stubs\${stub}-x86-unicode
  !endif
!macroend

${MementoSection} "PTIS Core Files (required)" SecCore

  SetDetailsPrint textonly
  DetailPrint "Installing PTIS Core Files..."
  SetDetailsPrint listonly

  SectionIn 1 2 3 RO
  SetOutPath $INSTDIR

  IfFileExists $INSTDIR\ptisconf.nsi "" +2
  Rename $INSTDIR\ptisconf.nsi $INSTDIR\ptisconf.nsh
  SetOverwrite off
  File ..\ptisconf.nsh

  SetOverwrite on
  File ..\makeptis.exe
  File ..\makeptisw.exe
  File ..\COPYING
  File ..\PTIS.chm
  !pragma verifychm "..\PTIS.chm"
  !if /FileExists "..\PTIS.exe"
    !if /FileExists "..\PTIS.exe.manifest"
      File "..\PTIS.exe.manifest"
    !endif
  !else
    !define NO_PTISMENU_HTML 1
    !makeptis '-v2 "PTISMenu.nsi" "-XOutFile ..\PTIS.exe"' = 0
  !endif
  File ..\PTIS.exe

  SetOutPath $INSTDIR\Bin
  File ..\Bin\makeptis.exe
!ifdef USE_NEW_ZLIB
  File ..\Bin\zlib.dll
!else
  File ..\Bin\zlib1.dll
!endif

  SetOutPath $INSTDIR\Stubs
  File ..\Stubs\uninst
  !insertmacro InstallStub bzip2
  !insertmacro InstallStub bzip2_solid
  !insertmacro InstallStub lzma
  !insertmacro InstallStub lzma_solid
  !insertmacro InstallStub zlib
  !insertmacro InstallStub zlib_solid
  

  SetOutPath $INSTDIR\Include
  File ..\Include\WinMessages.nsh
  File ..\Include\Sections.nsh
  File ..\Include\Library.nsh
  File ..\Include\UpgradeDLL.nsh
  File ..\Include\LogicLib.nsh
  File ..\Include\StrFunc.nsh
  File ..\Include\Colors.nsh
  File ..\Include\FileFunc.nsh
  File ..\Include\TextFunc.nsh
  File ..\Include\WordFunc.nsh
  File ..\Include\WinVer.nsh
  File ..\Include\x64.nsh
  File ..\Include\Memento.nsh
  File ..\Include\LangFile.nsh
  File ..\Include\InstallOptions.nsh
  File ..\Include\MultiUser.nsh
  File ..\Include\VB6RunTime.nsh
  File ..\Include\Util.nsh
  File ..\Include\Integration.nsh
  File ..\Include\WinCore.nsh

  SetOutPath $INSTDIR\Include\Win
  File ..\Include\Win\WinDef.nsh
  File ..\Include\Win\WinError.nsh
  File ..\Include\Win\WinNT.nsh
  File ..\Include\Win\WinUser.nsh
  File ..\Include\Win\COM.nsh
  File ..\Include\Win\Propkey.nsh
  File ..\Include\Win\RestartManager.nsh

  SetOutPath $INSTDIR\Docs\StrFunc
  File ..\Docs\StrFunc\StrFunc.txt

  SetOutPath $INSTDIR\Docs\MultiUser
  File ..\Docs\MultiUser\Readme.html

  SetOutPath $INSTDIR\Docs\makeptisw
  File ..\Docs\makeptisw\*.txt

  !ifndef NO_PTISMENU_HTML
    SetOutPath $INSTDIR\Menu
    File ..\Menu\*.html
    SetOutPath $INSTDIR\Menu\images
    File ..\Menu\images\header.gif
    File ..\Menu\images\line.gif
    File ..\Menu\images\site.gif
  !endif

  Delete $INSTDIR\makeptis.htm
  Delete $INSTDIR\Docs\*.html
  Delete $INSTDIR\Docs\style.css
  RMDir $INSTDIR\Docs

  SetOutPath $INSTDIR\Bin
  !if ${BITS} >= 64
    File /NonFatal  ..\Bin\RegTool-x86.bin
    File            ..\Bin\RegTool-amd64.bin
  !else
    File            ..\Bin\RegTool-x86.bin
    !if /FileExists ..\Bin\RegTool-amd64.bin ; It is unlikely that this exists, avoid the /NonFatal warning.
      File          ..\Bin\RegTool-amd64.bin
    !endif
  !endif

  CreateDirectory $INSTDIR\Plugins\x86-ansi
  CreateDirectory $INSTDIR\Plugins\x86-unicode
  !if ${BITS} >= 64
    CreateDirectory $INSTDIR\Plugins\amd64-unicode
  !endif
  !insertmacro InstallPlugin TypeLib

  ReadRegStr $R0 HKCR ".nsi" ""
  StrCmp $R0 "PTISFile" 0 +2
    DeleteRegKey HKCR "PTISFile"

  WriteRegStr HKCR ".nsi" "" "PTIS.Script"
  WriteRegStr HKCR ".nsi" "PerceivedType" "text"
  WriteRegStr HKCR "PTIS.Script" "" "PTIS Script File"
  WriteRegStr HKCR "PTIS.Script\DefaultIcon" "" "$INSTDIR\makeptisw.exe,1"
  ReadRegStr $R0 HKCR "PTIS.Script\shell\open\command" ""
  ${If} $R0 == ""
    WriteRegStr HKCR "PTIS.Script\shell" "" "open"
    WriteRegStr HKCR "PTIS.Script\shell\open\command" "" 'notepad.exe "%1"'
  ${EndIf}
  WriteRegStr HKCR "PTIS.Script\shell\compile" "" "Compile PTIS Script"
  WriteRegStr HKCR "PTIS.Script\shell\compile\command" "" '"$INSTDIR\makeptisw.exe" "%1"'
  WriteRegStr HKCR "PTIS.Script\shell\compile-compressor" "" "Compile PTIS Script (Choose Compressor)"
  WriteRegStr HKCR "PTIS.Script\shell\compile-compressor\command" "" '"$INSTDIR\makeptisw.exe" /ChooseCompressor "%1"'

  ReadRegStr $R0 HKCR ".nsh" ""
  StrCmp $R0 "NSHFile" 0 +2
    DeleteRegKey HKCR "NSHFile"

  WriteRegStr HKCR ".nsh" "" "PTIS.Header"
  WriteRegStr HKCR ".nsh" "PerceivedType" "text"
  WriteRegStr HKCR "PTIS.Header" "" "PTIS Header File"
  WriteRegStr HKCR "PTIS.Header\DefaultIcon" "" "$INSTDIR\makeptisw.exe,2"
  ReadRegStr $R0 HKCR "PTIS.Header\shell\open\command" ""
  ${If} $R0 == ""
    WriteRegStr HKCR "PTIS.Header\shell" "" "open"
    WriteRegStr HKCR "PTIS.Header\shell\open\command" "" 'notepad.exe "%1"'
  ${EndIf}

  ${NotifyShell_AssocChanged}

${MementoSectionEnd}

${MementoSection} "Script Examples" SecExample

  SetDetailsPrint textonly
  DetailPrint "Installing Script Examples..."
  SetDetailsPrint listonly

  SectionIn 1 2
  SetOutPath $INSTDIR\Examples
  File ..\Examples\makeptis.nsi
  File ..\Examples\example1.nsi
  File ..\Examples\example2.nsi
  File ..\Examples\AppGen.nsi
  File ..\Examples\install-per-user.nsi
  File ..\Examples\install-shared.nsi
  File ..\Examples\waplugin.nsi
  File ..\Examples\bigtest.nsi
  File ..\Examples\primes.nsi
  File ..\Examples\rtest.nsi
  File ..\Examples\gfx.nsi
  File ..\Examples\one-section.nsi
  File ..\Examples\languages.nsi
  File ..\Examples\Library.nsi
  File ..\Examples\VersionInfo.nsi
  File ..\Examples\LogicLib.nsi
  File ..\Examples\silent.nsi
  File ..\Examples\StrFunc.nsi
  File ..\Examples\FileFunc.nsi
  File ..\Examples\FileFunc.ini
  File ..\Examples\FileFuncTest.nsi
  File ..\Examples\TextFunc.nsi
  File ..\Examples\TextFunc.ini
  File ..\Examples\TextFuncTest.nsi
  File ..\Examples\WordFunc.nsi
  File ..\Examples\WordFunc.ini
  File ..\Examples\WordFuncTest.nsi
  File ..\Examples\Memento.nsi
  File ..\Examples\MultiUser.nsi
  File ..\Examples\unicode.nsi
  File ..\Examples\PTISMenu.nsi

  SetOutPath $INSTDIR\Examples\Plugin
  File ..\Examples\Plugin\exdll.c
  File ..\Examples\Plugin\exdll.dpr
  File ..\Examples\Plugin\exdll.dsp
  File ..\Examples\Plugin\exdll.dsw
  File ..\Examples\Plugin\exdll_with_unit.dpr
  File ..\Examples\Plugin\exdll-vs2008.sln
  File ..\Examples\Plugin\exdll-vs2008.vcproj
  File ..\Examples\Plugin\extdll.inc
  File ..\Examples\Plugin\ptis.pas

  SetOutPath $INSTDIR\Examples\Plugin\ptis
  File ..\Examples\Plugin\ptis\pluginapi.h
  File /nonfatal ..\Examples\Plugin\ptis\pluginapi*.lib
  File ..\Examples\Plugin\ptis\api.h
  File ..\Examples\Plugin\ptis\ptis_tchar.h

${MementoSectionEnd}

${MementoSection} "Start Menu Shortcut" SecShortcuts

  SetDetailsPrint textonly
  DetailPrint "Installing Start Menu shortcut..."
  SetDetailsPrint listonly

  SectionIn 1 2
  SetOutPath $INSTDIR
  CreateShortcut "$SMPROGRAMS\PTIS${NAMESUFFIX}.lnk" "$INSTDIR\PTIS.exe"

${MementoSectionEnd}

SectionGroup "User Interfaces" SecInterfaces

${MementoSection} "Modern User Interface" SecInterfacesModernUI

  SetDetailsPrint textonly
  DetailPrint "Installing User Interfaces | Modern User Interface..."
  SetDetailsPrint listonly

  SectionIn 1 2

  SetOutPath "$INSTDIR\Examples\Modern UI"
  File "..\Examples\Modern UI\Basic.nsi"
  File "..\Examples\Modern UI\HeaderBitmap.nsi"
  File "..\Examples\Modern UI\MultiLanguage.nsi"
  File "..\Examples\Modern UI\StartMenu.nsi"
  File "..\Examples\Modern UI\WelcomeFinish.nsi"

  SetOutPath "$INSTDIR\Contrib\Modern UI"
  File "..\Contrib\Modern UI\System.nsh"
  File "..\Contrib\Modern UI\ioSpecial.ini"

  SetOutPath "$INSTDIR\Docs\Modern UI"
  File "..\Docs\Modern UI\Readme.html"
  File "..\Docs\Modern UI\Changelog.txt"
  File "..\Docs\Modern UI\License.txt"

  SetOutPath "$INSTDIR\Docs\Modern UI\images"
  File "..\Docs\Modern UI\images\header.gif"
  File "..\Docs\Modern UI\images\screen1.png"
  File "..\Docs\Modern UI\images\screen2.png"
  File "..\Docs\Modern UI\images\open.gif"
  File "..\Docs\Modern UI\images\closed.gif"

  SetOutPath $INSTDIR\Contrib\UIs
  File "..\Contrib\UIs\modern.exe"
  File "..\Contrib\UIs\modern_headerbmp.exe"
  File "..\Contrib\UIs\modern_headerbmpr.exe"
  File "..\Contrib\UIs\modern_nodesc.exe"
  File "..\Contrib\UIs\modern_smalldesc.exe"

  SetOutPath $INSTDIR\Include
  File "..\Include\MUI.nsh"

  SetOutPath "$INSTDIR\Contrib\Modern UI 2"
  File "..\Contrib\Modern UI 2\Deprecated.nsh"
  File "..\Contrib\Modern UI 2\Interface.nsh"
  File "..\Contrib\Modern UI 2\Localization.nsh"
  File "..\Contrib\Modern UI 2\MUI2.nsh"
  File "..\Contrib\Modern UI 2\Pages.nsh"

  SetOutPath "$INSTDIR\Contrib\Modern UI 2\Pages"
  File "..\Contrib\Modern UI 2\Pages\Components.nsh"
  File "..\Contrib\Modern UI 2\Pages\Directory.nsh"
  File "..\Contrib\Modern UI 2\Pages\Finish.nsh"
  File "..\Contrib\Modern UI 2\Pages\InstallFiles.nsh"
  File "..\Contrib\Modern UI 2\Pages\License.nsh"
  File "..\Contrib\Modern UI 2\Pages\StartMenu.nsh"
  File "..\Contrib\Modern UI 2\Pages\UninstallConfirm.nsh"
  File "..\Contrib\Modern UI 2\Pages\Welcome.nsh"

  SetOutPath "$INSTDIR\Docs\Modern UI 2"
  File "..\Docs\Modern UI 2\Readme.html"
  File "..\Docs\Modern UI 2\License.txt"

  SetOutPath "$INSTDIR\Docs\Modern UI 2\images"
  File "..\Docs\Modern UI 2\images\header.gif"
  File "..\Docs\Modern UI 2\images\screen1.png"
  File "..\Docs\Modern UI 2\images\screen2.png"
  File "..\Docs\Modern UI 2\images\open.gif"
  File "..\Docs\Modern UI 2\images\closed.gif"

  SetOutPath $INSTDIR\Include
  File "..\Include\MUI2.nsh"

${MementoSectionEnd}

${MementoSection} "Default User Interface" SecInterfacesDefaultUI

  SetDetailsPrint textonly
  DetailPrint "Installing User Interfaces | Default User Interface..."
  SetDetailsPrint listonly

  SectionIn 1

  SetOutPath "$INSTDIR\Contrib\UIs"
  File "..\Contrib\UIs\default.exe"

${MementoSectionEnd}

${MementoSection} "Tiny User Interface" SecInterfacesTinyUI

  SetDetailsPrint textonly
  DetailPrint "Installing User Interfaces | Tiny User Interface..."
  SetDetailsPrint listonly

  SectionIn 1

  SetOutPath "$INSTDIR\Contrib\UIs"
  File "..\Contrib\UIs\sdbarker_tiny.exe"

${MementoSectionEnd}

SectionGroupEnd

${MementoSection} "Graphics" SecGraphics

  SetDetailsPrint textonly
  DetailPrint "Installing Graphics..."
  SetDetailsPrint listonly

  SectionIn 1

  Delete $INSTDIR\Contrib\Icons\*.ico
  Delete $INSTDIR\Contrib\Icons\*.bmp
  RMDir $INSTDIR\Contrib\Icons
  SetOutPath $INSTDIR\Contrib\Graphics
  File /r "..\Contrib\Graphics\*.ico"
  File /r "..\Contrib\Graphics\*.bmp"
${MementoSectionEnd}

${MementoSection} "Language Files" SecLangFiles

  SetDetailsPrint textonly
  DetailPrint "Installing Language Files..."
  SetDetailsPrint listonly

  SectionIn 1

  SetOutPath "$INSTDIR\Contrib\Language files"
  File "..\Contrib\Language files\*.nlf"

  SetOutPath $INSTDIR\Bin
  File ..\Bin\MakeLangID.exe

  ${If} ${SectionIsSelected} ${SecInterfacesModernUI}
    SetOutPath "$INSTDIR\Contrib\Language files"
    File "..\Contrib\Language files\*.nsh"
  ${EndIf}

${MementoSectionEnd}

SectionGroup "Tools" SecTools

${MementoSection} "Zip2Exe" SecToolsZ2E

  SetDetailsPrint textonly
  DetailPrint "Installing Tools | Zip2Exe..."
  SetDetailsPrint listonly

  SectionIn 1

  SetOutPath $INSTDIR\Bin
  File ..\Bin\zip2exe.exe
  SetOutPath $INSTDIR\Contrib\zip2exe
  File ..\Contrib\zip2exe\Base.nsh
  File ..\Contrib\zip2exe\Modern.nsh
  File ..\Contrib\zip2exe\Classic.nsh

${MementoSectionEnd}

SectionGroupEnd

SectionGroup "Plug-ins" SecPluginsPlugins

${MementoSection} "Banner" SecPluginsBanner

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | Banner..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin Banner
  SetOutPath $INSTDIR\Docs\Banner
  File ..\Docs\Banner\Readme.txt
  SetOutPath $INSTDIR\Examples\Banner
  File ..\Examples\Banner\Example.nsi
${MementoSectionEnd}

${MementoSection} "Language DLL" SecPluginsLangDLL

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | Language DLL..."
  SetDetailsPrint listonly

  SectionIn 1
  !insertmacro InstallPlugin LangDLL
${MementoSectionEnd}

${MementoSection} "nsExec" SecPluginsnsExec

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | nsExec..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin nsExec
  SetOutPath $INSTDIR\Docs\nsExec
  File ..\Docs\nsExec\nsExec.txt
  SetOutPath $INSTDIR\Examples\nsExec
  File ..\Examples\nsExec\test.nsi
${MementoSectionEnd}

${MementoSection} "Splash" SecPluginsSplash

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | Splash..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin Splash
  SetOutPath $INSTDIR\Docs\Splash
  File ..\Docs\Splash\splash.txt
  SetOutPath $INSTDIR\Examples\Splash
  File ..\Examples\Splash\Example.nsi
${MementoSectionEnd}

${MementoSection} "AdvSplash" SecPluginsSplashT

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | AdvSplash..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin AdvSplash
  SetOutPath $INSTDIR\Docs\AdvSplash
  File ..\Docs\AdvSplash\advsplash.txt
  SetOutPath $INSTDIR\Examples\AdvSplash
  File ..\Examples\AdvSplash\Example.nsi
${MementoSectionEnd}

${MementoSection} "BgImage" SecPluginsBgImage

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | BgImage..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin BgImage
  SetOutPath $INSTDIR\Docs\BgImage
  File ..\Docs\BgImage\BgImage.txt
  SetOutPath $INSTDIR\Examples\BgImage
  File ..\Examples\BgImage\Example.nsi
${MementoSectionEnd}

${MementoSection} "InstallOptions" SecPluginsIO

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | InstallOptions..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin InstallOptions
  SetOutPath $INSTDIR\Docs\InstallOptions
  File ..\Docs\InstallOptions\Readme.html
  File ..\Docs\InstallOptions\Changelog.txt
  SetOutPath $INSTDIR\Examples\InstallOptions
  File ..\Examples\InstallOptions\test.ini
  File ..\Examples\InstallOptions\test.nsi
  File ..\Examples\InstallOptions\testimgs.ini
  File ..\Examples\InstallOptions\testimgs.nsi
  File ..\Examples\InstallOptions\testlink.ini
  File ..\Examples\InstallOptions\testlink.nsi
  File ..\Examples\InstallOptions\testnotify.ini
  File ..\Examples\InstallOptions\testnotify.nsi
${MementoSectionEnd}

${MementoSection} "nsDialogs" SecPluginsDialogs

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | nsDialogs..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin nsDialogs
  SetOutPath $INSTDIR\Examples\nsDialogs
  File ..\Examples\nsDialogs\example.nsi
  File ..\Examples\nsDialogs\InstallOptions.nsi
  File ..\Examples\nsDialogs\timer.nsi
  File ..\Examples\nsDialogs\welcome.nsi
  SetOutPath $INSTDIR\Include
  File ..\Include\nsDialogs.nsh
  SetOutPath $INSTDIR\Docs\nsDialogs
  File ..\Docs\nsDialogs\Readme.html
${MementoSectionEnd}

${MementoSection} "Math" SecPluginsMath

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | Math..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin Math
  SetOutPath $INSTDIR\Docs\Math
  File ..\Docs\Math\Math.txt
  SetOutPath $INSTDIR\Examples\Math
  File ..\Examples\Math\math.nsi
  File ..\Examples\Math\mathtest.txt
  File ..\Examples\Math\mathtest.nsi
  File ..\Examples\Math\mathtest.ini

${MementoSectionEnd}

${MementoSection} "PTISdl" SecPluginsPTISDL

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | PTISdl..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin PTISdl
  SetOutPath $INSTDIR\Docs\PTISdl
  File ..\Docs\PTISdl\ReadMe.txt
  File ..\Docs\PTISdl\License.txt
${MementoSectionEnd}

${MementoSection} "System" SecPluginsSystem

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | System..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin System
  SetOutPath $INSTDIR\Docs\System
  File ..\Docs\System\System.html
  File ..\Docs\System\WhatsNew.txt
  SetOutPath $INSTDIR\Examples\System
  File ..\Examples\System\Resource.dll
  File ..\Examples\System\SysFunc.nsh
  File ..\Examples\System\System.nsh
  File ..\Examples\System\System.nsi
${MementoSectionEnd}

${MementoSection} "StartMenu" SecPluginsStartMenu

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | StartMenu..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin StartMenu
  SetOutPath $INSTDIR\Docs\StartMenu
  File ..\Docs\StartMenu\Readme.txt
  SetOutPath $INSTDIR\Examples\StartMenu
  File ..\Examples\StartMenu\Example.nsi
${MementoSectionEnd}

${MementoSection} "UserInfo" SecPluginsUserInfo

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | UserInfo..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin UserInfo
  SetOutPath $INSTDIR\Examples\UserInfo
  File ..\Examples\UserInfo\UserInfo.nsi
${MementoSectionEnd}

${MementoSection} "Dialer" SecPluginsDialer

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | Dialer..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin Dialer
  SetOutPath $INSTDIR\Docs\Dialer
  File ..\Docs\Dialer\Dialer.txt
${MementoSectionEnd}

${MementoSection} "VPatch" SecPluginsVPatch

  SetDetailsPrint textonly
  DetailPrint "Installing Plug-ins | VPatch..."
  SetDetailsPrint listonly

  SectionIn 1

  !insertmacro InstallPlugin VPatch
  SetOutPath $INSTDIR\Examples\VPatch
  File ..\Examples\VPatch\example.nsi
  File ..\Examples\VPatch\oldfile.txt
  File ..\Examples\VPatch\newfile.txt
  File ..\Examples\VPatch\patch.pat
  SetOutPath $INSTDIR\Docs\VPatch
  File ..\Docs\VPatch\Readme.html
  SetOutPath $INSTDIR\Bin
  File ..\Bin\GenPat.exe
  SetOutPath $INSTDIR\Include
  File ..\Include\VPatchLib.nsh
${MementoSectionEnd}

${MementoSectionDone}

SectionGroupEnd

Section -post

  ; When Modern UI is installed:
  ; * Always install the English language file
  ; * Always install default icons / bitmaps

  ${If} ${SectionIsSelected} ${SecInterfacesModernUI}

    SetDetailsPrint textonly
    DetailPrint "Configuring Modern UI..."
    SetDetailsPrint listonly

    ${IfNot} ${SectionIsSelected} ${SecLangFiles}
      SetOutPath "$INSTDIR\Contrib\Language files"
      File "..\Contrib\Language files\English.nlf"
      File "..\Contrib\Language files\English.nsh"
    ${EndIf}

    ${IfNot} ${SectionIsSelected} ${SecGraphics}
      SetOutPath $INSTDIR\Contrib\Graphics\Checks
      File "..\Contrib\Graphics\Checks\modern.bmp"
      SetOutPath $INSTDIR\Contrib\Graphics\Icons
      File "..\Contrib\Graphics\Icons\modern-install.ico"
      File "..\Contrib\Graphics\Icons\modern-uninstall.ico"
      SetOutPath $INSTDIR\Contrib\Graphics\Header
      File "..\Contrib\Graphics\Header\ptis.bmp"
      SetOutPath $INSTDIR\Contrib\Graphics\Wizard
      File "..\Contrib\Graphics\Wizard\win.bmp"
    ${EndIf}

  ${EndIf}

  SetDetailsPrint textonly
  DetailPrint "Creating Registry Keys..."
  SetDetailsPrint listonly

  SetOutPath $INSTDIR

  WriteRegStr HKLM "Software\PTIS" "" $INSTDIR
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
  WriteRegDword HKLM "Software\PTIS" "VersionMajor" "${VER_MAJOR}"
  WriteRegDword HKLM "Software\PTIS" "VersionMinor" "${VER_MINOR}"
  WriteRegDword HKLM "Software\PTIS" "VersionRevision" "${VER_REVISION}"
  WriteRegDword HKLM "Software\PTIS" "VersionBuild" "${VER_BUILD}"
!endif

  WriteRegStr HKLM "${REG_UNINST_KEY}" "UninstallString" '"$INSTDIR\uninst-ptis.exe"'
  WriteRegStr HKLM "${REG_UNINST_KEY}" "QuietUninstallString" '"$INSTDIR\uninst-ptis.exe" /S'
  WriteRegStr HKLM "${REG_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${REG_UNINST_KEY}" "DisplayName" "Nullsoft Install System${NAMESUFFIX}"
  WriteRegStr HKLM "${REG_UNINST_KEY}" "DisplayIcon" "$INSTDIR\PTIS.exe"
  WriteRegStr HKLM "${REG_UNINST_KEY}" "DisplayVersion" "${VERSION}"
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
  WriteRegDWORD HKLM "${REG_UNINST_KEY}" "VersionMajor" "${VER_MAJOR}" ; Required by WACK
  WriteRegDWORD HKLM "${REG_UNINST_KEY}" "VersionMinor" "${VER_MINOR}" ; Required by WACK
!endif
  WriteRegStr HKLM "${REG_UNINST_KEY}" "Publisher" "Nullsoft and Contributors" ; Required by WACK
  WriteRegStr HKLM "${REG_UNINST_KEY}" "URLInfoAbout" "https://ptis.sourceforge.io/"
  WriteRegStr HKLM "${REG_UNINST_KEY}" "HelpLink" "https://ptis.sourceforge.io/Support"
  WriteRegDWORD HKLM "${REG_UNINST_KEY}" "NoModify" "1"
  WriteRegDWORD HKLM "${REG_UNINST_KEY}" "NoRepair" "1"
  ${MakeARPInstallDate} $1
  WriteRegStr HKLM "${REG_UNINST_KEY}" "InstallDate" $1

  WriteUninstaller $INSTDIR\uninst-ptis.exe

  ${MementoSectionSave}

  SetDetailsPrint both

SectionEnd

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} "The core files required to use PTIS (compiler etc.)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExample} "Example installation scripts that show you how to use PTIS"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} "Add icon to your start menu for easy access"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecInterfaces} "User interface designs that can be used to change the installer look and feel"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecInterfacesModernUI} "A modern user interface like the wizards of recent Windows versions"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecInterfacesDefaultUI} "The default PTIS user interface which you can customize to make your own UI"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecInterfacesTinyUI} "A tiny version of the default user interface"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecTools} "Tools that help you with PTIS development"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecToolsZ2E} "A utility that converts a ZIP file to a PTIS installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGraphics} "Icons, checkbox images and other graphics"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecLangFiles} "Language files used to support multiple languages in an installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsPlugins} "Useful plugins that extend PTIS's functionality"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsBanner} "Plugin that lets you show a banner before installation starts"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsLangDLL} "Plugin that lets you add a language select dialog to your installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsnsExec} "Plugin that executes console programs and prints its output in the PTIS log window or hides it"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsSplash} "Splash screen add-on that lets you add a splash screen to an installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsSplashT} "Splash screen add-on with transparency support that lets you add a splash screen to an installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsSystem} "Plugin that lets you call Win32 API or external DLLs"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsMath} "Plugin that lets you evaluate complicated mathematical expressions"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsDialer} "Plugin that provides internet connection functions"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsIO} "Plugin that lets you add custom pages to an installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsDialogs} "Plugin that lets you add custom pages to an installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsStartMenu} "Plugin that lets the user select the start menu folder"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsBgImage} "Plugin that lets you show a persistent background image plugin and play sounds"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsUserInfo} "Plugin that that gives you the user name and the user account type"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsPTISDL} "Plugin that lets you create a web based installer"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsVPatch} "Plugin that lets you create patches to upgrade older files"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Installer Functions

Function .onInit

  ${MementoSectionRestore}

FunctionEnd

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD

Var ReinstallPageCheck

Function PageReinstall

  ReadRegStr $R0 HKLM "Software\PTIS" ""
  ReadRegStr $R1 HKLM "${REG_UNINST_KEY}" "UninstallString"
  ${IfThen} "$R0$R1" == "" ${|} Abort ${|}

  StrCpy $R4 "older"
  ReadRegDWORD $R0 HKLM "Software\PTIS" "VersionMajor"
  ReadRegDWORD $R1 HKLM "Software\PTIS" "VersionMinor"
  ReadRegDWORD $R2 HKLM "Software\PTIS" "VersionRevision"
  ReadRegDWORD $R3 HKLM "Software\PTIS" "VersionBuild"
  ${IfThen} $R0 = 0 ${|} StrCpy $R4 "unknown" ${|} ; Anonymous builds have no version number
  StrCpy $R0 $R0.$R1.$R2.$R3

  ${VersionCompare} ${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD} $R0 $R0
  ${If} $R0 == 0
    StrCpy $R1 "PTIS ${VERSION} is already installed. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Add/Reinstall components"
    StrCpy $R3 "Uninstall PTIS"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose the maintenance option to perform."
    StrCpy $R0 "2"
  ${ElseIf} $R0 == 1
    StrCpy $R1 "An $R4 version of PTIS is installed on your system. It's recommended that you uninstall the current version before installing. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Uninstall before installing"
    StrCpy $R3 "Do not uninstall"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install PTIS."
    StrCpy $R0 "1"
  ${ElseIf} $R0 == 2
    StrCpy $R1 "A newer version of PTIS is already installed! It is not recommended that you install an older version. If you really want to install this older version, it's better to uninstall the current version first. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Uninstall before installing"
    StrCpy $R3 "Do not uninstall"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install PTIS."
    StrCpy $R0 "1"
  ${Else}
    Abort
  ${EndIf}

  nsDialogs::Create 1018
  Pop $R4

  ${NSD_CreateLabel} 0 0 100% 24u $R1
  Pop $R1

  ${NSD_CreateRadioButton} 30u 50u -30u 8u $R2
  Pop $R2
  ${NSD_OnClick} $R2 PageReinstallUpdateSelection

  ${NSD_CreateRadioButton} 30u 70u -30u 8u $R3
  Pop $R3
  ${NSD_OnClick} $R3 PageReinstallUpdateSelection

  ${If} $ReinstallPageCheck != 2
    SendMessage $R2 ${BM_SETCHECK} ${BST_CHECKED} 0
  ${Else}
    SendMessage $R3 ${BM_SETCHECK} ${BST_CHECKED} 0
  ${EndIf}

  ${NSD_SetFocus} $R2

  nsDialogs::Show

FunctionEnd

Function PageReinstallUpdateSelection

  Pop $R1

  ${NSD_GetState} $R2 $R1

  ${If} $R1 == ${BST_CHECKED}
    StrCpy $ReinstallPageCheck 1
  ${Else}
    StrCpy $ReinstallPageCheck 2
  ${EndIf}

FunctionEnd

Function PageLeaveReinstall

  ${NSD_GetState} $R2 $R1

  StrCmp $R0 "1" 0 +2 ; Existing install is not the same version?
    StrCmp $R1 "1" reinst_uninstall reinst_done

  StrCmp $R1 "1" reinst_done ; Same version, skip to add/reinstall components?

  reinst_uninstall:
  ReadRegStr $R1 HKLM "${REG_UNINST_KEY}" "UninstallString"

  ;Run uninstaller
    HideWindow

    ClearErrors
    ExecWait '$R1 _?=$INSTDIR' $0

    BringToFront

    ${IfThen} ${Errors} ${|} StrCpy $0 2 ${|} ; ExecWait failed, set fake exit code

    ${If} $0 <> 0
    ${OrIf} ${FileExists} "$INSTDIR\Bin\makeptis.exe"
      ${If} $0 = 1 ; User aborted uninstaller?
        StrCmp $R0 "2" 0 +2 ; Is the existing install the same version?
          Quit ; ...yes, already installed, we are done
        Abort
      ${EndIf}
      MessageBox MB_ICONEXCLAMATION "Unable to uninstall!"
      Abort
    ${Else}
      StrCpy $0 $R1 1
      ${IfThen} $0 == '"' ${|} StrCpy $R1 $R1 -1 1 ${|} ; Strip quotes from UninstallString
      Delete $R1
      RMDir $INSTDIR
    ${EndIf}

  reinst_done:

FunctionEnd

!endif # VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD

Function ShowReleaseNotes
  StrCpy $0 $WINDIR\hh.exe
  ${IfNotThen} ${FileExists} $0 ${|} SearchPath $0 hh.exe ${|}
  ${If} ${FileExists} $0
    Exec '"$0" mk:@MSITStore:$INSTDIR\PTIS.chm::/SectionF.1.html'
  ${Else}
    ExecShell "" "https://ptis.sourceforge.io/Docs/AppendixF.html#F.1"
  ${EndIf}
FunctionEnd

;--------------------------------
;Uninstaller Section

Section Uninstall

  SetDetailsPrint textonly
  DetailPrint "Uninstalling NSI Development Shell Extensions..."
  SetDetailsPrint listonly

  IfFileExists $INSTDIR\Bin\makeptis.exe ptis_installed
    MessageBox MB_YESNO "It does not appear that PTIS is installed in the directory '$INSTDIR'.$\r$\nContinue anyway (not recommended)?" IDYES ptis_installed
    Abort "Uninstall aborted by user"
  ptis_installed:

  SetDetailsPrint textonly
  DetailPrint "Deleting Registry Keys..."
  SetDetailsPrint listonly

  !macro AssocDeleteFileExtAndProgId _hkey _dotext _pid
  ReadRegStr $R0 ${_hkey} "Software\Classes\${_dotext}" ""
  StrCmp $R0 "${_pid}" 0 +2
    DeleteRegKey ${_hkey} "Software\Classes\${_dotext}"

  DeleteRegKey ${_hkey} "Software\Classes\${_pid}"
  !macroend

  !insertmacro AssocDeleteFileExtAndProgId HKLM ".nsi" "PTIS.Script"
  !insertmacro AssocDeleteFileExtAndProgId HKLM ".nsh" "PTIS.Header"

  ${NotifyShell_AssocChanged}

  DeleteRegKey HKLM "${REG_UNINST_KEY}"
  DeleteRegKey HKLM "Software\PTIS"

  SetDetailsPrint textonly
  DetailPrint "Deleting Files..."
  SetDetailsPrint listonly

  Delete "$SMPROGRAMS\PTIS${NAMESUFFIX}.lnk"
  Delete "$DESKTOP\PTIS${NAMESUFFIX}.lnk" ; Remove legacy shortcut
  Delete $INSTDIR\makeptis.exe
  Delete $INSTDIR\makeptisw.exe
  Delete $INSTDIR\PTIS.exe
  Delete $INSTDIR\PTIS.exe.manifest
  Delete $INSTDIR\license.txt
  Delete $INSTDIR\COPYING
  Delete $INSTDIR\uninst-ptis.exe
  Delete $INSTDIR\ptisconf.nsi
  Delete $INSTDIR\ptisconf.nsh
  Delete $INSTDIR\PTIS.chm
  RMDir /r $INSTDIR\Bin
  RMDir /r $INSTDIR\Contrib
  RMDir /r $INSTDIR\Docs
  RMDir /r $INSTDIR\Examples
  RMDir /r $INSTDIR\Include
  RMDir /r $INSTDIR\Menu
  RMDir /r $INSTDIR\Plugins
  RMDir /r $INSTDIR\Stubs
  RMDir $INSTDIR

  SetDetailsPrint both

SectionEnd
