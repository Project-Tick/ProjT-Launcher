Import('defenv')

### Configuration options

cfg = Variables()

cfg.Add(
  (
    'PTIS_MAX_STRLEN',
    'defines the maximum string length for internal variables and stack entries. 1024 should be plenty, but if you are doing crazy registry stuff, you might want to bump it up. Generally it adds about 16-32x the memory, so setting this to 4096 from 1024 will add around 64k of memory usage (not really a big deal, but not usually needed).',
    1024
  )
)

cfg.Add(
  (
    'PTIS_MAX_INST_TYPES',
    'defines the maximum install types. Note that this should not exceed 32, ever.',
    32
  )
)

cfg.Add(
  (
    'PTIS_DEFAULT_LANG',
    'defines the default language id PTIS will use if nothing else is defined in the script. Default value is 1033 which is English.',
    1033
  )
)

cfg.Add(
  (
    'PTIS_VARS_SECTION',
    'defines the name of the PE section containing the runtime variables',
    '.ndata'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_UNINSTALL_SUPPORT',
    "enables the uninstaller support. Turn it off if your installers don't need uninstallers. Adds less than 1kb.",
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_LICENSEPAGE',
    'enables support for the installer to present a license page.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_COMPONENTPAGE',
    'enables support for the installer to present a page where you can select what sections are installed. with this disabled, all sections are installed by default',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_COMPONENTPAGE_ALTERNATIVE',
    'enables an alternative components page behavior. Checkboxes will only be toggled when clicking on the checkbox itself and not on its label. .onMouseOverSection will only be called when the user selects the component and not when moving the mouse pointer over it.',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_SILENT_SUPPORT',
    'enables support for making installers that are completely silent.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_VISIBLE_SUPPORT',
    'enables support for making installers that are visible.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_ENHANCEDUI_SUPPORT',
    'enables support for CreateFont, SetCtlColors (used by some UIs), SetBrandingImage, .onGUIInit, etc.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_COMPRESSION_SUPPORT',
    'enables support for making installers that use compression (recommended).',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_COMPRESS_BZIP2_SMALLMODE',
    "if defined, bzip2's decompressor uses bzip2's alternative decompression method that uses less runtime memory, at the expense of speed (and executable size). not recommended.",
    'no'
  )
)

cfg.Add(
  (
    'PTIS_COMPRESS_BZIP2_LEVEL',
    'bzip2 compression window size. 1-9 is valid. 9 uses the most memory, but typically compresses best (recommended). 1 uses the least memory, but typically compresses the worst.',
    9
  )
)


cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_CRC_SUPPORT',
    'enables support for installer verification. HIGHLY recommended.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_CRC_ANAL',
    'makes the CRC verification extremely careful, meaning extra bytes on the end of file, or the first 512 bytes changing, will give error. Enable this if you are paranoid, otherwise leaving it off seems safe (and is less prone to reporting virii). If you will be digitally signing your installers, leave this off.',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_LOG',
    'enables the logging facility. turning this on (by uncommenting it) adds about 4kb, but can be useful in debugging your installers.',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_LOG_ODS',
    'makes the logging facility use OutputDebugString instead of a file.',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_LOG_STDOUT',
    'makes the logging facility use stdout instead of a file.',
    'no'
  )
)

cfg.Add(
	BoolVariable(
		'PTIS_CONFIG_LOG_TIMESTAMP',
		'adds a timestamp to each log line.',
		'no'
	)
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_BGBG',
    'enables support for the blue (well, whatever color you want) gradient background window.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_CODECALLBACKS',
    'enables support for installer code callbacks. recommended, as it uses a minimum of space and allows for neat functionality.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_MOVEONREBOOT',
    'enables support for uninstallers that automatically delete themselves from the temp directory, as well as the reboot moving/deleting modes of Delete and Rename. Adds about 512 gay bytes..',
    'yes'
  )
)

### Instruction enabling configuration

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_ACTIVEXREG',
    'enables activeX plug-in registration and deregistration, as well as CallInstDLL',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_INTOPTS',
    'enables support for IntCmp, IntCmpU, IntOp, and IntFmt.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_STROPTS',
    'enables support for StrCmp, StrCpy, and StrLen, as well as Get*Local.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_STACK',
    'enables support for the stack (Push, Pop, Exch)',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_FILEFUNCTIONS',
    'enables support for FileOpen,FileClose, FileSeek, FileRead, and FileWrite.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_FINDFIRST',
    'enables support for FindFirst, FindNext, and FindClose.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_CREATESHORTCUT',
    'enables support for CreateShortcut.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_INIFILES',
    'enables support for ReadINIStr and WriteINIStr.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_REGISTRYFUNCTIONS',
    'enables support for ReadRegStr, ReadRegDWORD, WriteRegStr, etc etc etc.',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_COPYFILES',
    'enables support for CopyFiles',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_REBOOT',
    'enables support for Reboot, IfRebootFlag, SetRebootFlag',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_FNUTIL',
    'enables support for GetFullPathName, GetTempFileName, and SearchPath',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_EXECUTE',
    'enables support for Exec and ExecWait',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_SHELLEXECUTE',
    'enables support for ExecShell',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_GETDLLVERSION',
    'enables support for GetDLLVersion',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_GETFILETIME',
    'enables support for GetFileTime',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_HWNDS',
    'enables support for FindWindow, SendMessage, and IsWindow',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_ENVIRONMENT',
    'enables support for ReadEnvStr and ExpandEnvStrings',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_RMDIR',
    'enables support for RMDir',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_FILE',
    'enables support for File (extracting files)',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_DELETE',
    'enables support for Delete (delete files)',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_RENAME',
    'enables support for Rename (rename files)',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_MESSAGEBOX',
    'enables support for MessageBox',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_VERSION_INFO',
    'enables support for version information in the installer',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_FIX_DEFINES_IN_STRINGS',
    'fixes defines inside defines and handles chars $ perfectly',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_SUPPORT_STANDARD_PREDEFINES',
    'enables standard predefines - __FILE__, __LINE__, __DATE__, __TIME__ and __TIMESTAMP__',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_LOCKWINDOW_SUPPORT',
    'enables the LockWindow command',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_PLUGIN_SUPPORT',
    'enables installer plug-ins support',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_FIX_COMMENT_HANDLING',
    'fixes comment handling',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'PTIS_CONFIG_CONST_DATA_PATH',
    'determines if plugins, includes, stubs etc. are located in a constant path set at build-time',
    defenv['PLATFORM'] != 'win32'
  )
)

### Generate help

Help(cfg.GenerateHelpText(defenv))

### Apply configuration

env = Environment()
cfg.Update(env)

def AddValuedDefine(define):
  defenv.Append(PTIS_CPPDEFINES = [(define, env[define])])

def AddBoolDefine(define):
  if env[define]:
    defenv.Append(PTIS_CPPDEFINES = [define])

def AddStringDefine(define):
  defenv.Append(PTIS_CPPDEFINES = [(define, '"%s"' % env[define])])

AddValuedDefine('PTIS_MAX_STRLEN')
AddValuedDefine('PTIS_MAX_INST_TYPES')
AddValuedDefine('PTIS_DEFAULT_LANG')

AddBoolDefine('PTIS_CONFIG_UNINSTALL_SUPPORT')
AddBoolDefine('PTIS_CONFIG_LICENSEPAGE')
AddBoolDefine('PTIS_CONFIG_COMPONENTPAGE')
AddBoolDefine('PTIS_CONFIG_COMPONENTPAGE_ALTERNATIVE')
AddBoolDefine('PTIS_CONFIG_SILENT_SUPPORT')
AddBoolDefine('PTIS_CONFIG_VISIBLE_SUPPORT')
AddBoolDefine('PTIS_CONFIG_ENHANCEDUI_SUPPORT')
AddBoolDefine('PTIS_CONFIG_COMPRESSION_SUPPORT')
AddBoolDefine('PTIS_COMPRESS_BZIP2_SMALLMODE')

AddValuedDefine('PTIS_COMPRESS_BZIP2_LEVEL')

AddBoolDefine('PTIS_CONFIG_CRC_SUPPORT')
AddBoolDefine('PTIS_CONFIG_CRC_ANAL')
AddBoolDefine('PTIS_CONFIG_LOG')
AddBoolDefine('PTIS_CONFIG_LOG_ODS')
AddBoolDefine('PTIS_CONFIG_LOG_STDOUT')
AddBoolDefine('PTIS_CONFIG_LOG_TIMESTAMP')
AddBoolDefine('PTIS_SUPPORT_BGBG')
AddBoolDefine('PTIS_SUPPORT_CODECALLBACKS')
AddBoolDefine('PTIS_SUPPORT_MOVEONREBOOT')
AddBoolDefine('PTIS_SUPPORT_ACTIVEXREG')
AddBoolDefine('PTIS_SUPPORT_INTOPTS')
AddBoolDefine('PTIS_SUPPORT_STROPTS')
AddBoolDefine('PTIS_SUPPORT_STACK')
AddBoolDefine('PTIS_SUPPORT_FILEFUNCTIONS')
AddBoolDefine('PTIS_SUPPORT_FINDFIRST')
AddBoolDefine('PTIS_SUPPORT_CREATESHORTCUT')
AddBoolDefine('PTIS_SUPPORT_INIFILES')
AddBoolDefine('PTIS_SUPPORT_REGISTRYFUNCTIONS')
AddBoolDefine('PTIS_SUPPORT_COPYFILES')
AddBoolDefine('PTIS_SUPPORT_REBOOT')
AddBoolDefine('PTIS_SUPPORT_FNUTIL')
AddBoolDefine('PTIS_SUPPORT_EXECUTE')
AddBoolDefine('PTIS_SUPPORT_SHELLEXECUTE')
AddBoolDefine('PTIS_SUPPORT_GETDLLVERSION')
AddBoolDefine('PTIS_SUPPORT_GETFILETIME')
AddBoolDefine('PTIS_SUPPORT_HWNDS')
AddBoolDefine('PTIS_SUPPORT_ENVIRONMENT')
AddBoolDefine('PTIS_SUPPORT_RMDIR')
AddBoolDefine('PTIS_SUPPORT_FILE')
AddBoolDefine('PTIS_SUPPORT_DELETE')
AddBoolDefine('PTIS_SUPPORT_RENAME')
AddBoolDefine('PTIS_SUPPORT_MESSAGEBOX')
AddBoolDefine('PTIS_SUPPORT_VERSION_INFO')
AddBoolDefine('PTIS_FIX_DEFINES_IN_STRINGS')
AddBoolDefine('PTIS_SUPPORT_STANDARD_PREDEFINES')
AddBoolDefine('PTIS_LOCKWINDOW_SUPPORT')
AddBoolDefine('PTIS_CONFIG_PLUGIN_SUPPORT')
AddBoolDefine('PTIS_FIX_COMMENT_HANDLING')
AddBoolDefine('PTIS_CONFIG_CONST_DATA_PATH')

AddStringDefine('PTIS_VARS_SECTION')
