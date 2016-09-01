@echo off
set include=include
del /q efi64.efi >NUL 2>&1
fasmg hello.asm hello.efi
if not %errorlevel%==0 goto end

if not [%1]==[qemu] goto end
set UEFI_EXT=x64
set QEMU_ARCH=x86_64

set QEMU_PATH=C:\Program Files\qemu\
set QEMU_OPTS=-net none -monitor none -parallel none
set QEMU_EXE=qemu-system-%QEMU_ARCH%w.exe
set BOOT_NAME=boot%UEFI_EXT%.efi
set OVMF_BIOS=OVMF.fd

if not exist image\efi\boot mkdir image\efi\boot
copy hello.efi image\efi\boot\boot%UEFI_EXT%.efi >NUL
echo fs0:\efi\boot\boot%UEFI_EXT%.efi > image\efi\boot\startup.nsh
if not exist %OVMF_BIOS% echo %OVMF_BIOS% is missing!
"%QEMU_PATH%%QEMU_EXE%" %QEMU_OPTS% -L . -bios %OVMF_BIOS% -hda fat:image
del /q trace-* >NUL 2>&1
:end
