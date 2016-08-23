fasmg-ebc - EBC (EFI Byte Code) compiler for fasmg
==================================================

_Because programing in assembler for UEFI is easy and nobody should have to
[pay](https://software.intel.com/en-us/articles/intel-c-compiler-for-efi-byte-code-purchase)
to produce EBC executables..._

## Prerequisites

* [flat assembler g (fasmg)](http://flatassembler.net/download.php) (make sure
  to download the 'g' version)
* [QEMU](http://www.qemu.org) __v2.5 or later__ for testing
  (NB: You can find QEMU Windows binaries [here](https://qemu.weilnetz.de/w64/))
* git

## Assembly and testing (on Windows)

* Make sure fasmg is in your path, or copy `fasmg.exe` to the current directory
* Run `make`
* Additionally, you can download the [latest OVMF x64](http://www.tianocore.org/ovmf/)
  and extract it into the `examples\x86\` directory, and run `make qemu`.
  Note you may have to edit `make.cmd` to set your qemu directory.
