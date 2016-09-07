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
  STORESP R1, [IP]

section '.data' data readable writeable

section '.reloc' fixups data discardable
