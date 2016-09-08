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
  MOVIb R1, 0x10000
  MOVIn R1, (+4096,+0)
  MOVREL R1, Test1
  CMPIeq R1, Test2
  JMP8cc (Test2 - Test1)/2
Test1:
  RET
Test2:
  RET

section '.data' data readable writeable

section '.reloc' fixups data discardable
