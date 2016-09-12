;
; EBC (EFI Byte Code) assembly test
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'

format peebc efi
entry EfiMain

section '.text' code executable readable
EfiMain:
  MOV    R1, @R1(EFI_SYSTEM_TABLE.ConOut)
  ; And thank you, intel BUGGY EBC Debugger, for making me believe
  ; we had an issue with CALL[EX] computed indexes... >_<
  CALLEX @R1(EFI_SYSTEM_TABLE.ConOut)

section '.data' data readable writeable

section '.reloc' fixups data discardable
