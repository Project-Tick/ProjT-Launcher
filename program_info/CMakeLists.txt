if(UNIX)
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_search_module(SCDOC scdoc)
        if(SCDOC_FOUND)
            pkg_get_variable(SCDOC_SCDOC scdoc scdoc)
        endif()
    endif()
endif()

set(Launcher_CommonName "ProjTLauncher")
set(Launcher_DisplayName "ProjT Launcher")

set(Launcher_Name "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_DisplayName "${Launcher_DisplayName}" PARENT_SCOPE)

set(Launcher_AppID "tr.org.projecttick.ProjTLauncher")
set(Launcher_SVGFileName "${Launcher_AppID}.svg")
set(Launcher_Copyright "© 2025-2026 Project Tick\\n© 2022-2025 Prism Launcher Contributors\\n© 2021-2022 PolyMC Contributors\\n© 2012-2021 MultiMC Contributors")
set(Launcher_Copyright_Mac "© 2025-2026 Project Tick, © 2022-2025 Prism Launcher Contributors, © 2021-2022 PolyMC Contributors and © 2012-2021 MultiMC Contributors" PARENT_SCOPE)
set(Launcher_Copyright "${Launcher_Copyright}" PARENT_SCOPE)
set(Launcher_Domain "projtlauncher.projecttick.org.tr" PARENT_SCOPE)
set(Launcher_UserAgent "${Launcher_CommonName}/${Launcher_VERSION_NAME}" PARENT_SCOPE)
set(Launcher_ConfigFile "projtlauncher.cfg" PARENT_SCOPE)
set(Launcher_Git "https://github.com/Project-Tick/ProjT-Launcher" PARENT_SCOPE)
set(Launcher_AppID "${Launcher_AppID}" PARENT_SCOPE)
set(Launcher_SVGFileName "${Launcher_SVGFileName}" PARENT_SCOPE)

set(Launcher_Desktop "program_info/${Launcher_AppID}.desktop" PARENT_SCOPE)
set(Launcher_mrpack_MIMEInfo "program_info/modrinth-mrpack-mime.xml" PARENT_SCOPE)
set(Launcher_MetaInfo "program_info/${Launcher_AppID}.metainfo.xml" PARENT_SCOPE)
set(Launcher_SVG "program_info/${Launcher_SVGFileName}" PARENT_SCOPE)
set(Launcher_Branding_ICNS "program_info/projtlauncher.icns" PARENT_SCOPE)
set(Launcher_Branding_ICO "program_info/projtlauncher.ico")
set(Launcher_Branding_ICO "${Launcher_Branding_ICO}" PARENT_SCOPE)
set(Launcher_Branding_WindowsRC "program_info/projtlauncher.rc" PARENT_SCOPE)
set(Launcher_Branding_LogoQRC "program_info/projtlauncher.qrc" PARENT_SCOPE)

set(Launcher_Portable_File "program_info/portable.txt" PARENT_SCOPE)

configure_file(${Launcher_AppID}.desktop.in ${Launcher_AppID}.desktop)
configure_file(${Launcher_AppID}.metainfo.xml.in ${Launcher_AppID}.metainfo.xml)
configure_file(projtlauncher.rc.in projtlauncher.rc @ONLY)
configure_file(projtlauncher.qrc.in projtlauncher.qrc @ONLY)
configure_file(projtlauncher.manifest.in projtlauncher.manifest @ONLY)
configure_file(projtlauncher.ico projtlauncher.ico COPYONLY)
configure_file(${Launcher_SVGFileName} ${Launcher_SVGFileName} COPYONLY)

set(Launcher_Project_Source_Dir "${PROJECT_SOURCE_DIR}")

file(READ "${PROJECT_SOURCE_DIR}/LICENSE" MAIN_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/zlib/LICENSE" ZLIB_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/quazip/COPYING" QUAZIP_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/qdcss/LICENSE" QDCSS_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/libnbtplusplus/COPYING" LIBNBT_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/libnbtplusplus/COPYING.LESSER" LIBNBT_LESSER_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/launcherjava/LICENSE" LAUNCHERJAVA_LICENSE)
file(READ "${PROJECT_SOURCE_DIR}/javacheck/LICENSE" JAVACHECK_LICENSE)

set(COMBINED_LICENSES "--- zlib ---\n\n${ZLIB_LICENSE}\n\n--- QuaZip ---\n\n${QUAZIP_LICENSE}\n\n--- qdcss ---\n\n${QDCSS_LICENSE}\n\n--- libnbtplusplus ---\n\n${LIBNBT_LICENSE}\n\n${LIBNBT_LESSER_LICENSE}\n\n--- launcherjava ---\n\n${LAUNCHERJAVA_LICENSE}\n\n--- javacheck ---\n\n${JAVACHECK_LICENSE}")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CombinedLicenses.txt" "${COMBINED_LICENSES}")

if(MSVC)
    set(Launcher_MSVC_Redist_NSIS_Section [=[
!ifdef haveNScurl
Section "Visual Studio Runtime"
    Var /GLOBAL vc_redist_exe
    ${If} ${IsNativeARM64}
        StrCpy $vc_redist_exe "vc_redist.arm64.exe"
    ${Else}
        StrCpy $vc_redist_exe "vc_redist.x64.exe"
    ${EndIf}
    DetailPrint 'Downloading Microsoft Visual C++ Redistributable...'
    NScurl::http GET "https://aka.ms/vs/17/release/$vc_redist_exe" "$INSTDIR\vc_redist\$vc_redist_exe" /INSIST /CANCEL /Zone.Identifier /END
    Pop $0
    ${If} $0 == "OK"
        DetailPrint "Download successful"
        ExecWait "$INSTDIR\vc_redist\$vc_redist_exe /install /passive /norestart"
    ${Else}
        DetailPrint "Download failed with error $0"
    ${EndIf}
SectionEnd
!endif
]=])
endif()

configure_file(win_install.nsi.in win_install.nsi @ONLY)

if(SCDOC_FOUND)
    set(in_scd "${CMAKE_CURRENT_SOURCE_DIR}/projtlauncher.6.scd")
    set(out_man "${CMAKE_CURRENT_BINARY_DIR}/projtlauncher.6")
    add_custom_command(
        DEPENDS "${in_scd}"
        OUTPUT "${out_man}"
        COMMAND ${SCDOC_SCDOC} < "${in_scd}" > "${out_man}"
    )
    add_custom_target(man ALL DEPENDS ${out_man})
    set(Launcher_ManPage "program_info/projtlauncher.6" PARENT_SCOPE)
endif()
