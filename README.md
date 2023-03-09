fasmg-ebc - EBC (EFI Byte Code) assembler for fasmg
===================================================

_Because programming in assembler for UEFI is easy and nobody should have to
[pay](https://software.intel.com/en-us/articles/intel-c-compiler-for-efi-byte-code-purchase)
to produce EBC executables..._

## FINAL UPDATE - 2023.03.09

This repository is archived, which means that it will not see any changes or any new development.

## Prerequisites

* [QEMU](http://www.qemu.org) __v2.7 or later__
  NB: You can find QEMU Windows binaries [here](https://qemu.weilnetz.de/w64/).  
  Make sure you use QEMU 2.7 or later, as earlier versions may produce a `Synchronous Exception` on AARCH64.
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

Also, if you are using a label with `JMP` or `CALL`, the assembler will convert it to a relative offset.  
For instance, say you have the following code:
```
Label:
   (...)
   JMPcc Label
```
Then `JMPcc` will either be converted to `JMP8cc <offset>` or `JMP32cc R0(<offset>)` or `JMP64cc <offset>`
with `<offset>` being the relative value required for each specific instruction size, to point to `Label`.  

The assembler can also guess the size of indexes or immediates, if you don't explicitly specify one.
For instance `MOVIn R1, (+2,+8)` gets assembled as `MOVInw R1, (+2,+8)` whereas `MOVIn R1, (+2,+4096)`
gets assembled as `MOVInd R1, (+2,+4096)`.

Finally, if you use the `struct` macro to define a set of EFI structures, the assembler will convert member
references to their equivalent indexes. This means that something like this:
```
MOVn R1, @R1(EFI_SYSTEM_TABLE.ConOut)
```
gets assembled as:
```
MOVn R1, @R1(+5,+24)
```
Oh, and this conversion is smart enough to handle alignment computations, such that an `UINT32` (aligned to
natural size) followed by an `UINT64` does generate `(+1,+0)` for the `UINT64` index and not `(+0,+4)`.  
For more, see `efi.inc` and `hello.asm`.

## Assembly and testing (Windows)

* Run `make` to compile the `hello.asm` sample. If not already available, the latest fasmg binary is 
  downloaded automatically.
* Additionally, you can compile one of the other samples by providing its short name (e.g. `make arch`)
* If you have QEMU installed, you can add a `qemu` parameter to run the application (e.g. `make hello qemu`)  
  You may also add one of `x64`, `ia32`, `arm` and `aa64` to run the application against a specific UEFI
  architecture (e.g. `make hello qemu aa64`)
* If you wish to debug the samples with the EBC Debugger, simply replace `qemu` with `debug`
  (e.g. `make arch debug arm`). The EBC Debugger will be automatically downloaded for the required arch.

__Note:__ The assembler itself is really only comprised of the files you can find under `include\`.
  __Everything else__ is just samples or test data.
