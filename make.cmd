@echo off
set include=include
del /q efi64.efi >NUL 2>&1

if [%1]==[debug] (
  fasmg debug.asm debug.efi
) else (
  fasmg hello.asm hello.efi
)
if not %errorlevel%==0 goto end
if [%1]==[] goto end

set UEFI_EXT=x64
set QEMU_ARCH=x86_64

set QEMU_PATH=C:\Program Files\qemu\
set QEMU_OPTS=-net none -monitor none -parallel none
set QEMU_EXE=qemu-system-%QEMU_ARCH%w.exe
set OVMF_BIOS=OVMF.fd

if not exist %OVMF_BIOS% (
  echo %OVMF_BIOS% is missing!
  goto end
)

if not exist image\efi\boot mkdir image\efi\boot
echo fs0: > image\efi\boot\startup.nsh
echo cd efi\boot\ >> image\efi\boot\startup.nsh
if [%1]==[debug] (
  copy debug.efi image\efi\boot >NUL
  if not exist "EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi" (
    echo EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi is missing!
    goto end
  )
  copy "EBC Debugger\EbcDebugger\%UEFI_EXT%\EbcDebugger.efi" image\efi\boot >NUL
  echo EbcDebugger.efi >> image\efi\boot\startup.nsh
  echo debug.efi >> image\efi\boot\startup.nsh
) else (
  copy hello.efi image\efi\boot >NUL
  echo hello.efi >> image\efi\boot\startup.nsh
)

"%QEMU_PATH%%QEMU_EXE%" %QEMU_OPTS% -L . -bios %OVMF_BIOS% -hda fat:image
del /q trace-* >NUL 2>&1

:end
