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
  JMP64acs 0x40000000
Test:
  JMPcc EfiMain
  JMP R1(1234)
  BREAK 0

section '.data' data readable writeable

section '.reloc' fixups data discardable
