; Copyright (c) 2016,2021 by Thomas A. Early. All rights reserved.
Name "GE User Net12NMR Installer"
OutFile "GE User Net12NMR Installer.exe"

; The default installation directory
InstallDir "$PROGRAMFILES32\Net12NMR Suite"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "SOFTWARE\Net12NMR Suite" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; --------------Pages------------

LicenseData "License.rtf"
Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

!macro AddServerKeys servername homedir hostname username
  WriteRegStr HKCU "SOFTWARE\NMR Lab\Servers\${servername}" "home" "${homedir}"
  WriteRegStr HKCU "SOFTWARE\NMR Lab\Servers\${servername}" "host" "${hostname}"
  WriteRegStr HKCU "SOFTWARE\NMR Lab\Servers\${servername}" "user" "${username}"
!macroend

!macro AddFileAndHelp basename
  File "${basename}.exe"
  File "${basename}.chm"
!macroend

SectionGroup /e "Programs"
  ; The stuff to install
  Section "-Base Data"    ; the hidden section - always install
    SetOutPath $INSTDIR
    ; Write the installation path into the registry
    WriteRegStr HKLM "SOFTWARE\Net12NMR Suite" "Install_Dir" "$INSTDIR"
    
    ; Write the uninstall keys for Windows
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Net12NMR Suite" "DisplayName" "Net12NMR Suite"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Net12NMR Suite" "DisplayIcon" "$INSTDIR\Net12NMR.ico"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Net12NMR Suite" "Publisher" "Thomas A. Early"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Net12NMR Suite" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Net12NMR Suite" "EstimatedSize" 1870
    WriteUninstaller "$INSTDIR\uninstall.exe"
    ; Make shortcut directory
    CreateDirectory "$SMPROGRAMS\Net12NMR Suite"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
    File "Revision History.xlsx"
    File "Net12NMR\res\Net12NMR.ico"
  SectionEnd

  Section Net12NMR  
    ; Set output path to the installation directory.
    SetOutPath $INSTDIR
    
    ; Put file there
    !insertmacro AddFileAndHelp "Release\Net12NMR"
    File "Release\nuctabref.txt"
    File "Release\PeakAnalysis.xlsm"
    File "Release\Solvents.txt"
    
    ; Create shortcut
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\Net12NMR.lnk" "$INSTDIR\Net12NMR.exe" "" "$INSTDIR\Net12NMR.exe" 0
  SectionEnd

  Section ConnectionMgr
    SetOutPath $INSTDIR
    File "Release\ConnectMgr.exe"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\ConnectMgr.lnk" "$INSTDIR\ConnectMgr.exe" "" "$INSTDIR\ConnectMgr.exe" 0
  SectionEnd

  Section ConsoleServer
    SetOutPath $INSTDIR
    File "Release\ConsoleServer.exe"
    File "Release\Using the Net12NMR ConsoleServer.docx"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\ConsoleServer.lnk" "$INSTDIR\ConsoleServer.exe" "" "$INSTDIR\ConsoleServer.exe" 0
    !insertmacro AddServerKeys "My Personal Server" "$DOCUMENTS\" "127.0.0.1" "nmrlab"
  SectionEnd

  Section ILT
    SetOutPath $INSTDIR
    !insertmacro AddFileAndHelp "Release\ILT"
    File "Release\MonteCarlo.xlsm"
    File "Release\T1IRtest.txt"
    File "Release\T2test.txt"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\ILT.lnk" "$INSTDIR\ILT.exe" "" "$INSTDIR\ILT.exe" 0
  SectionEnd
   
  Section NetComPlot
    SetOutPath $INSTDIR
    !insertmacro AddFileAndHelp "Release\NetComPlot"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\NetComPlot.lnk" "$INSTDIR\NetComPlot.exe" "" "$INSTDIR\NetComPlot.exe" 0
  SectionEnd
   
  Section NetPlot
    SetOutPath $INSTDIR
    !insertmacro AddFileAndHelp "Release\NetPlot"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\NetPlot.lnk" "$INSTDIR\NetPlot.exe" "" "$INSTDIR\NetPlot.exe" 0
  SectionEnd

  
  Section NetSpan
    SetOutPath $INSTDIR
    !insertmacro AddFileAndHelp "Release\NetSpan"
    File "Release\NMR Curve Fit.xlsm"
    CreateShortcut "$SMPROGRAMS\Net12NMR Suite\NetSpan.lnk" "$INSTDIR\NetSpan.exe" "" "$INSTDIR\NetSpan.exe" 0
  SectionEnd
SectionGroupEnd

; Write Server Data
; SectionGroup /e "Server Access"
;   Section "My 300"
;     !insertmacro AddServerKeys "My 300" "/path-to-data/" "ip-address" "password"
;   SectionEnd
;   Section "My 600"
;     !insertmacro AddServerKeys "My 600" "/path-to-data/" "ip-address" "password"
;   SectionEnd
; SectionGroupEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  ; Remove registry keys
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Net12NMR Suite"
  DeleteRegKey HKLM "SOFTWARE\Net12NMR Suite"
  DeleteRegKey HKCU "SOFTWARE\NMR Lab"

  ; Remove files and uninstaller
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Net12NMR Suite\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Net12NMR Suite"
  RMDir "$INSTDIR"
SectionEnd
