; o3prmeditor.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "O3prmEditorInstaller"

; The file to write
OutFile "bin\O3prmEditorInstaller.exe"

; The default installation directory
InstallDir $PROGRAMFILES\O3prmEditor

; Request application privileges for Windows Vista
RequestExecutionLevel none

;--------------------------------

; Pages

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "O3prmEditor (required)"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File bin\Release\o3prmeditor.exe
  File $%QTDIR%\lib\qscintilla2.dll
  File $%QTDIR%\bin\QtCore4.dll
  File $%QTDIR%\bin\QtGui4.dll
  File $%QTDIR%\bin\mingwm10.dll
  File $%QTDIR%\bin\libgcc_s_dw2-1.dll
  
  WriteUninstaller "uninstall.exe"
  
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\O3prmEditor"
  CreateShortCut "$SMPROGRAMS\O3prmEditor\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\O3prmEditor\O3prmEditor.lnk" "$INSTDIR\O3prmEditor.exe" "" "$INSTDIR\O3prmEditor.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKCU  "Software\Lip6\O3prmEditor"

  ; Remove files and uninstaller
  Delete $INSTDIR\*.exe
  Delete $INSTDIR\*.dll

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\O3prmEditor\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\O3prmEditor"
  RMDir "$INSTDIR"

SectionEnd

