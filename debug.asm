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
  MOVsn R1, @R2(+0,+4096)

section '.data' data readable writeable

section '.reloc' fixups data discardable
