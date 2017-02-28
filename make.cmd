@echo off
set include=include

set UEFI_EXT=x64
set QEMU_ARCH=x86_64
set QEMU_PATH=C:\Program Files\qemu\
set QEMU_OPTS=-net none -monitor none -parallel none
set FIRMWARE_BASENAME=OVMF
set FILE=hello
set RUN_QEMU=
set RUN_DEBUGGER=
set COPY_SRC=
set FW_EXT=
set HDA=fat:image
set QEMU_EXTRA=

:loop
if [%1]==[] goto next
if [%1]==[debug] (
  set RUN_QEMU=1
  set RUN_DEBUGGER=1
) else if [%1]==[qemu] (
  if not exist "%QEMU_PATH%" (
    echo %QEMU_PATH% is missing!
    goto end
  )
  set RUN_QEMU=1
) else if [%1]==[x64] (
  rem nothing to do
) else if [%1]==[ia32] (
  set UEFI_EXT=ia32
  set QEMU_ARCH=i386
) else if [%1]==[arm] (
  set UEFI_EXT=arm
  set QEMU_ARCH=arm
  set QEMU_OPTS=-M virt -cpu cortex-a15 %QEMU_OPTS%
  set FIRMWARE_BASENAME=QEMU_EFI
) else if [%1]==[aa64] (
  set UEFI_EXT=aa64
  set QEMU_ARCH=aarch64
  set QEMU_OPTS=-M virt -cpu cortex-a57 %QEMU_OPTS%
  set FIRMWARE_BASENAME=QEMU_EFI
) else if [%1]==[serial] (
  set QEMU_OPTS=%QEMU_OPTS% -serial file:serial_%UEFI_EXT%.log
) else if [%1]==[ntfs] (
  set FW_EXT=_ntfs
  set HDA=ntfs.vhd
  set QEMU_EXTRA=-hdb fat:image
  echo This is a FAT file system > image/THIS_IS_FAT.txt
  if not exist ntfs.vhd (
    call cscript /nologo "%~dp0download.vbs" http://efi.akeo.ie/EBC/FAT ntfs.zip ntfs.vhd ntfs.vhd "An NTFS test VHD including a Fat EBC driver"
    if errorlevel 1 goto end
    echo.
  )
) else if [%1]==[copy] (
  set COPY_SRC=\\vm-debian\src\edk2\Build\ArmVirtQemu-ARM\RELEASE_GCC5\FV\QEMU_EFI.fd
) else (
  set FILE=%1
  if not exist "%1.asm" (
    echo %1.asm does not exist!
    goto end
  )
)
shift
goto loop

:next
if not exist fasmg.exe (
  call cscript /nologo "%~dp0download.vbs" https://flatassembler.net fasmg.zip fasmg.exe fasmg.exe "The latest fasmg assembler"
  if errorlevel 1 goto end
  echo.
)

echo fasmg %FILE%.asm %FILE%.efi
fasmg %FILE%.asm %FILE%.efi
if not %errorlevel%==0 goto end

if [%RUN_QEMU%]==[] goto end

set UEFI_EXT_UPPERCASE=X64
if [%UEFI_EXT%]==[ia32] (
  set UEFI_EXT_UPPERCASE=IA32
) else if [%UEFI_EXT%]==[arm] (
  set UEFI_EXT_UPPERCASE=ARM
) else if [%UEFI_EXT%]==[aa64] (
  set UEFI_EXT_UPPERCASE=AA64
)

set QEMU_FIRMWARE=%FIRMWARE_BASENAME%_%UEFI_EXT_UPPERCASE%%FW_EXT%.fd
set QEMU_EXE=qemu-system-%QEMU_ARCH%w.exe
set ZIP_FILE=%FIRMWARE_BASENAME%-%UEFI_EXT_UPPERCASE%%FW_EXT%.zip
if not [%COPY_SRC%]==[] (
  echo Copy %COPY_SRC% to %QEMU_FIRMWARE%
  copy %COPY_SRC% %QEMU_FIRMWARE%
) else if not exist %QEMU_FIRMWARE% (
  call cscript /nologo "%~dp0download.vbs" http://efi.akeo.ie/%FIRMWARE_BASENAME% %ZIP_FILE% %FIRMWARE_BASENAME%.fd %QEMU_FIRMWARE% "The UEFI firmware file, needed for QEMU,"
  if errorlevel 1 goto end
)

if [%UEFI_EXT%]==[arm] (
  echo.
  echo Notice: EBC support for ARM is not yet integrated into EDK2
  echo This means that a specially patched UEFI firmware is required
  echo for EBC to work on ARM...
)

if not exist image\efi\boot mkdir image\efi\boot
del image\efi\boot\boot*.efi > NUL 2>&1
del image\efi\boot\startup.nsh > NUL 2>&1
if not [%RUN_DEBUGGER%]==[] (
  echo fs0: > image\efi\boot\startup.nsh
  if not exist "Ebc Debugger\EbcDebugger_%UEFI_EXT%.efi" (
    if not exist "EBC Debugger" mkdir "EBC Debugger"
    call cscript /nologo "%~dp0download.vbs" http://efi.akeo.ie/EBC/Debugger EbcDebugger-%UEFI_EXT_UPPERCASE%.zip EbcDebugger.efi "EBC Debugger\EbcDebugger_%UEFI_EXT%.efi" "The EBC Debugger"
    if errorlevel 1 goto end  
  )
  copy "EBC Debugger\EbcDebugger_%UEFI_EXT%.efi" image\EbcDebugger_%UEFI_EXT%.efi >NUL
  echo load EbcDebugger_%UEFI_EXT%.efi >> image\efi\boot\startup.nsh
)
if [%FILE%]==[protocol] (
  copy %FILE%.efi image\%FILE%.efi >NUL
  copy protocol_driver\driver_%UEFI_EXT%.efi image > NUL
  if not exist "image\efi\boot\startup.nsh" (
    echo fs0: > image\efi\boot\startup.nsh
  )
  echo load driver_%UEFI_EXT%.efi >> image\efi\boot\startup.nsh
  echo %FILE%.efi >> image\efi\boot\startup.nsh
) else if [%FILE%]==[driver] (
  copy %FILE%.efi image\driver_ebc.efi >NUL
  if not exist "image\efi\boot\startup.nsh" (
    echo fs0: > image\efi\boot\startup.nsh
  )
  echo load driver_ebc.efi >> image\efi\boot\startup.nsh
  rem echo drivers >> image\efi\boot\startup.nsh
  copy native\native_%UEFI_EXT%.efi image >NUL
  echo native_%UEFI_EXT%.efi >> image\efi\boot\startup.nsh
) else (
  if exist "image\efi\boot\startup.nsh" (
    copy %FILE%.efi image >NUL
    echo %FILE%.efi >> image\efi\boot\startup.nsh
  ) else (
    copy %FILE%.efi image\efi\boot\boot%UEFI_EXT%.efi >NUL
  )
)

echo.
echo %QEMU_EXE% %QEMU_OPTS% -L . -bios %QEMU_FIRMWARE% -hda %HDA% %QEMU_EXTRA%
"%QEMU_PATH%%QEMU_EXE%" %QEMU_OPTS% -L . -bios %QEMU_FIRMWARE% -hda %HDA% %QEMU_EXTRA%
rem del /q trace-* >NUL 2>&1

:end
