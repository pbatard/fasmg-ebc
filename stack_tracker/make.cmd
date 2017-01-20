@echo off
setlocal EnableDelayedExpansion
set include=..\include

set FILE_LIST=matrix max cloaked realloc shutdown
set UEFI_EXT=arm
set QEMU_ARCH=arm
set QEMU_PATH=C:\Program Files\qemu\
set QEMU_OPTS=-net none -monitor none -parallel none
set FIRMWARE_BASENAME=QEMU_EFI
set RUN_QEMU=
set COPY_SRC=

:loop
if [%1]==[] goto next
if [%1]==[qemu] (
  if not exist "%QEMU_PATH%" (
    echo %QEMU_PATH% is missing!
    goto end
  )
  set RUN_QEMU=1
) else if [%1]==[ia32] (
  set UEFI_EXT=ia32
  set QEMU_ARCH=i386
  set FIRMWARE_BASENAME=OVMF
) else if [%1]==[serial] (
  set QEMU_OPTS=%QEMU_OPTS% -serial file:serial_%UEFI_EXT%.log
) else if [%1]==[copy] (
  set COPY_SRC=\\debian\src\edk2\Build\ArmVirtQemu-ARM\RELEASE_GCC5\FV\QEMU_EFI.fd
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
  call cscript /nologo "%~dp0\..\download.vbs" https://flatassembler.net fasmg.zip fasmg.exe fasmg.exe "The latest fasmg assembler"
  if errorlevel 1 goto end
  echo.
)

for %%f in (%FILE_LIST%) do (
  echo fasmg %%f.asm %%f.efi
  fasmg %%f.asm %%f.efi
  if errorlevel 1 goto end
)

if [%RUN_QEMU%]==[] goto end

if [%QEMU_ARCH%]==[arm] set QEMU_OPTS=%QEMU_OPTS% -M virt -cpu cortex-a15 %QEMU_OPTS%

set UEFI_EXT_UPPERCASE=ARM
if [%UEFI_EXT%]==[ia32] (
  set UEFI_EXT_UPPERCASE=IA32
)

set QEMU_FIRMWARE=%FIRMWARE_BASENAME%_%UEFI_EXT_UPPERCASE%.fd
set QEMU_EXE=qemu-system-%QEMU_ARCH%w.exe
set ZIP_FILE=%FIRMWARE_BASENAME%-%UEFI_EXT_UPPERCASE%.zip
if not [%COPY_SRC%]==[] (
  echo Copy %COPY_SRC% to %QEMU_FIRMWARE%
  copy %COPY_SRC% %QEMU_FIRMWARE%
) else if not exist %QEMU_FIRMWARE% (
  call cscript /nologo "%~dp0\..\download.vbs" http://efi.akeo.ie/%FIRMWARE_BASENAME% %ZIP_FILE% %FIRMWARE_BASENAME%.fd %QEMU_FIRMWARE% "The UEFI firmware file, needed for QEMU,"
  if errorlevel 1 goto end
)

if not exist image\efi\boot mkdir image\efi\boot
del image\efi\boot\boot*.efi > NUL 2>&1
del image\efi\boot\startup.nsh > NUL 2>&1

for %%f in (%FILE_LIST%) do (
  copy %%f.efi image\%%f.efi >NUL
)  
copy driver\driver_%UEFI_EXT%.efi image > NUL
echo fs0: > image\efi\boot\startup.nsh
echo load driver_%UEFI_EXT%.efi >> image\efi\boot\startup.nsh
for %%f in (%FILE_LIST%) do (
  echo %%f.efi >> image\efi\boot\startup.nsh
)

"%QEMU_PATH%%QEMU_EXE%" %QEMU_OPTS% -L . -bios %QEMU_FIRMWARE% -hda fat:image
del /q trace-* >NUL 2>&1

:end
