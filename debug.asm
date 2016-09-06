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
  CALL32 R0(sub)
  RET

sub:
  RET

section '.data' data readable writeable
