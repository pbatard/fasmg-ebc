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
  MOVsn R1, @R2(+1,+512)

section '.data' data readable writeable

section '.reloc' fixups data discardable
