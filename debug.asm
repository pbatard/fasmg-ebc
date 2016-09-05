;
; EBC (EFI Byte Code) assembly test
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'

format peebc dll efi
entry efi_main

section '.text' code executable readable
efi_main:
  MOVsnw @R2(+1,+8), R1 1234

section '.data' data readable writeable

section '.reloc' fixups data discardable
