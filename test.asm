;
; EBC (EFI Byte Code) assembly test
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'

format peebc dll efi
entry EfiMain

section '.text' code executable readable
EfiMain:
  JMP -0x8000000000000000
  BREAK  3
  BREAK  3
  CALL32 R1
  CALL EfiMain
  JMP EfiMain
  JMP32  R0(EfiMain)
  CALL32  R0(EfiMain)
  CALL64  0x1234567812345678

section '.data' data readable writeable

section '.reloc' fixups data discardable
