@echo off
set include=include

set UEFI_EXT=x64
set QEMU_ARCH=x86_64
set QEMU_PATH=C:\Program Files\qemu\
set QEMU_OPTS=-net none -monitor none -parallel none
set FILE=hello
set RUN_QEMU=
set RUN_DEBUGGER=

:loop
if [%1]==[] goto next
if [%1]==[debug] (
  if not exist "EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi" (
    echo EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi is missing!
    goto end
  )
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
) else if [%1]==[aa64] (
  set UEFI_EXT=aa64
  set QEMU_ARCH=aarch64
  set QEMU_OPTS=-M virt -cpu cortex-a57 %QEMU_OPTS%
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
echo fasmg %FILE%.asm %FILE%.efi
fasmg %FILE%.asm %FILE%.efi
if not %errorlevel%==0 goto end

set OVMF_BIOS=OVMF_%UEFI_EXT%.fd
if not exist %OVMF_BIOS% (
  call cscript /nologo "%~dp0download.vbs" %UEFI_EXT%
  if errorlevel 1 goto end
)
set QEMU_EXE=qemu-system-%QEMU_ARCH%w.exe

if [%RUN_QEMU%]==[] goto end

if not exist %OVMF_BIOS% (
  echo %OVMF_BIOS% is missing!
  goto end
)

if [%UEFI_EXT%]==[arm] (
  echo.
  echo Notice: As of 2016.09.18, EBC support for ARM is not integrated into EDK2
  echo which means a specially patched UEFI firmware is required for EBC to work...
  echo.
)

if not exist image\efi\boot mkdir image\efi\boot
del image\efi\boot\boot*.efi > NUL 2>&1
del image\efi\boot\startup.nsh > NUL 2>&1
if not [%RUN_DEBUGGER%]==[] (
  echo fs0: > image\efi\boot\startup.nsh
  echo cd efi\boot\ >> image\efi\boot\startup.nsh
  copy %FILE%.efi image\efi\boot >NUL
  if not exist "EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi" (
    echo EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi was not found
    goto end
  )
  copy "EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi" image\efi\boot\EbcDebugger_%UEFI_EXT%.efi >NUL
  echo EbcDebugger_%UEFI_EXT%.efi >> image\efi\boot\startup.nsh
  echo %FILE%.efi >> image\efi\boot\startup.nsh
) else (
  copy %FILE%.efi image\efi\boot\boot%UEFI_EXT%.efi >NUL
)

"%QEMU_PATH%%QEMU_EXE%" %QEMU_OPTS% -L . -bios %OVMF_BIOS% -hda fat:image
del /q trace-* >NUL 2>&1

:end
