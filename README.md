fasmg-ebc - EBC (EFI Byte Code) assembler for fasmg
===================================================

_Because programming in assembler for UEFI is easy and nobody should have to
[pay](https://software.intel.com/en-us/articles/intel-c-compiler-for-efi-byte-code-purchase)
to produce EBC executables..._

## Prerequisites

* [flat assembler g (fasmg)](http://flatassembler.net/download.php).  
  Please make sure to download the 'g' version of fasm.
* [QEMU](http://www.qemu.org) __v2.5 or later__ and [OVMF](http://www.tianocore.org/ovmf/) for testing.  
  NB: You can find QEMU Windows binaries [here](https://qemu.weilnetz.de/w64/).
* UEFI.org's [EBC Debugger](http://www.uefi.org/node/550) for syntax debugging and validation.  
  Note however that __the EBC Debugger [IS](https://github.com/tianocore/edk/blob/master/Sample/Universal/Ebc/Dxe/EbcDebugger/EdbDisasmSupport.c#L191)
  [buggy](https://github.com/tianocore/edk/blob/master/Sample/Universal/Ebc/Dxe/EbcDebugger/EdbDisasmSupport.c#L228)
  when it comes to displaying 32 or 64bit indexes__, so please don't report that fasmg-ebc is not
  encoding indexes properly, when it's really the debugger that is not decoding them properly.
* git (e.g. [TortoiseGit](https://tortoisegit.org/) for Windows).

## Instructions and syntax

See Chapter 21 (_EFI Byte Code Virtual Machine_) of the [UEFI Specifications](http://www.uefi.org/sites/default/files/resources/UEFI%20Spec%202_6.pdf#page=1001).

This assembler accepts not specifying a size for arithmetic or comparison operations, in which
case the 64-bit version is used (since all EBC registers are 64 bit). This means that `XOR R1, R2`
will be converted to `XOR64 R1, R2`.

For `JMP`/`CALL[EX]` operations where you do not specify a size, the assembler will also insert
the most appropriate version.  
For instance `CALL @R1` will be converted to `CALL32 @R1` whereas `CALL 0x1234` will be converted
to `CALL64 0x1234`.

Finally, the assembler also guesses the size of indexes or immediates, where possible, if you
don't specify any. E.g. `MOVIn R1, (+2,+4096)` is assembled as `MOVInd R1, (+2,+4096)`.

## Assembly and testing (on Windows)

* Make sure fasmg is in your path, or copy `fasmg.exe` to the current directory
* Run `make`
* Additionally, if you have Qemu installed, you can extract the [latest x64 OVMF](http://www.tianocore.org/ovmf/)
  into the root directory, and run `make qemu`.
* Also, if you have extracted the EBC Debugger into the root directory, you can add your own assembly
  statements into `debug.asm` and run `make debug` to validate the encoded instructions.
