;
; EBC (EFI Byte Code) assembly 'Hello World'
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'

format peebc dll efi
entry efi_main

; Define a custom EFI error to test successful memory access
EFI_ACCESSED = EFIERR or 0xACCE55ED

section '.text' code executable readable
efi_main:
  MOVIqq R7, EFI_UNSUPPORTED
  MOVIqq R1, TestData
  MOVIqq @R1(-1,-8), EFI_ACCESSED
  MOVIqq R2, Indexed
  MOVqq @R2(+1,+8), @R1(-1,-8)
  MOVq R3, R1
  MOVq R7, @R3
  MOVIqq R6, 0x10
  ADD64 R7, R6(1)
  SUB64 R7, R6
  XOR64 R7, @R3(+1,0)
  RET

section '.data' data readable writeable
  Indexed:  dq EFI_NOT_FOUND
            dq EFI_NOT_FOUND
  TestData: dq EFI_NOT_FOUND
            dq 3

section '.reloc' fixups data discardable
