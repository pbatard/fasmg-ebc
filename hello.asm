;
; EBC (EFI Byte Code) assembly 'Hello World'
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

; NB: Note the following changes from the intel syntax:
;     @Rx         -> [Rx]
;     @Rx (+n,+c) -> [Rx] (+n:+c)

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'

format peebc dll efi
entry efi_main

section '.text' code executable readable
efi_main:
  MOVIqq R7, EFI_UNSUPPORTED
  MOVIqq R1, TestData
  MOVIqq [R1] (-1:-8), EFI_NOT_READY
  MOVIqq R1, Indexed
  MOVqq R7, [R1]
  RET

section '.data' data readable writeable
  Indexed:  dq EFI_NOT_FOUND
            dq EFI_NOT_FOUND
  TestData: dq EFI_NOT_READY

section '.reloc' fixups data discardable
