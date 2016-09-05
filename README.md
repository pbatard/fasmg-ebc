fasmg-ebc - EBC (EFI Byte Code) assembler for fasmg
===================================================

_Because programming in assembler for UEFI is easy and nobody should have to
[pay](https://software.intel.com/en-us/articles/intel-c-compiler-for-efi-byte-code-purchase)
to produce EBC executables..._

## Prerequisites

* [flat assembler g (fasmg)](http://flatassembler.net/download.php) (make sure
  to download the 'g' version)
* [QEMU](http://www.qemu.org) __v2.5 or later__ and [OVMF](http://www.tianocore.org/ovmf/) for testing
  (NB: You can find QEMU Windows binaries [here](https://qemu.weilnetz.de/w64/))
* UEFI.org's [EBC Debugger](http://www.uefi.org/node/550) for syntax debugging and validation
* git

## Instructions and syntax

See Chapter 21 (_EFI Byte Code Virtual Machine_) of the [UEFI Specifications](http://www.uefi.org/sites/default/files/resources/UEFI%20Spec%202_6.pdf#page=1001).

## Assembly and testing (on Windows)

* Make sure fasmg is in your path, or copy `fasmg.exe` to the current directory
* Run `make`
* Additionally, if you have Qemu installed, you can extract the [latest x64 OVMF](http://www.tianocore.org/ovmf/)
  into the root directory, and run `make qemu`.
* Also, if you have extracted the EBC Debugger into the root directory, you can add your own assembly
  statements into `debug.asm` and run `make debug` to validate the encoded instructions.
