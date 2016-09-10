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
  MOVI R1, 0xFFFF
  MOVREL R1, Test
Test:
  MOVInw R1, (+12,+50)
  MOVInw R1, (+0,+0)
  JMP -0x100 + 2
  JMP -0x80000000 + 6
  MOVREL R0, -32768
  JMPcs EfiMain

section '.data' data readable writeable

section '.reloc' fixups data discardable
