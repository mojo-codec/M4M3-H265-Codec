;M4M3 H.265 codec

!include "MUI2.nsh"

!define ProgramName "M4M3 H.265 Codec"
!define Ver "0.5.0507R"
!define ProductVersion "0.5.0.7"

!define MUI_ICON  .\M4M3logo.ico
!define MUI_UNICON  .\M4M3logo.ico

OutFile "M4M3 Codec filter_${ProductVersion}.exe"
RequestExecutionLevel admin
XPStyle on

InstallDir "$SYSDIR"

;page-----------------------------
;Page license
;Page instfiles
;UninstPage uninstConfirm
;UninstPage instfiles

!insertmacro MUI_PAGE_LICENSE licence.txt
!insertmacro MUI_PAGE_INSTFILES
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
;---------------------------------

;--------------------------------
;Languages
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "TradChinese"
;--------------------------------


; First is default
;LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
;LoadLanguageFile "${NSISDIR}\Contrib\Language files\TradChinese.nlf"

; License data
; Not exactly translated, but it shows what's needed
LicenseLangString License ${LANG_ENGLISH} "licence.txt"
LicenseLangString License ${LANG_TRADCHINESE} "licence.txt"
LicenseData $(License)

;LangString LoginMesg ${LANG_ENGLISH} "Please login as Administrator's rights !!"
;LangString LoginMesg ${LANG_TRADCHINESE} "請用系統管理者帳號安裝!!"


Name $(ProgramName)

;Section--------------------------------
;Language selection Sections
;Section !$(Sec1Name) sec1
;	MessageBox MB_OK $(Message)
;SectionEnd

;Installer Sections
Section "-Required"

	SetOutPath "$INSTDIR"

	;Create uninstaller
	WriteUninstaller "$INSTDIR\${ProgramName}-uninstall.exe"

	
	File "..\_Build\M4M3DS\win32\Release\M4M3DS.ax"
	File "..\_Build\M4M3VFW\win32\Release\M4M3VFW.dll"
	
	RegDLL $INSTDIR\M4M3DS.ax
	CallInstDLL $INSTDIR\M4M3VFW.dll RegM4M3VFW
	
	; Write the uninstall keys for Windows
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ProgramName}" "UninstallString" "$INSTDIR\${ProgramName}-uninstall.exe"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ProgramName}" "DisplayIcon" "$INSTDIR\${ProgramName}-uninstall.exe"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ProgramName}" "DisplayName" "${ProgramName}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ProgramName}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ProgramName}" "NoRepair" 1
SectionEnd

;Uninstaller Section
Section "Uninstall"
	UnRegDLL $INSTDIR\M4M3DS.ax
	CallInstDLL $INSTDIR\M4M3VFW.dll UnRegM4M3VFW
  
	Delete /REBOOTOK "$INSTDIR\M4M3DS.ax"
	Delete /REBOOTOK "$INSTDIR\M4M3VFW.dll"
	Delete /REBOOTOK "$INSTDIR\${ProgramName}-uninstall.exe"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ProgramName}"
SectionEnd
;--------------------------------------------------------


;function--------------------------------
Function .onInit
	Var /GLOBAL LoginMesg
	;Language selection dialog
	Push ""
	Push ${LANG_ENGLISH}
	Push English
	Push ${LANG_TRADCHINESE}
	Push "繁體中文"
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
	StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
		StrCpy $LoginMesg "Please login as Administrator's rights !!"
	StrCmp $LANGUAGE ${LANG_TRADCHINESE} 0 +2
		StrCpy $LoginMesg "請用系統管理者帳號安裝!!"
		 
	; Check Administrator's rights
	ClearErrors
	UserInfo::GetAccountType
	IfErrors NotAdmin
	Pop $R0
	StrCmp $R0 "Admin" admin
	NotAdmin:
	; No Administrator's rights
	MessageBox MB_OK $LoginMesg 
	Abort
	admin:
FunctionEnd
;-----------------------------------------------------------------------------

;Version--------------------------------------------------------------------
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${ProgramName}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "M4M3 H.265 codec VFW and Directshow filter"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "mojo office"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "M4M3 codec"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C) 2002~ 2015 mojo  All rights reserved"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${ProgramName}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" ${Ver}
VIProductVersion ${ProductVersion}
;---------------------------------------------------------------------------