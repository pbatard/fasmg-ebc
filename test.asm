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
  MOV R1, @R1(EFI_SYSTEM_TABLE.Hdr)
  MOV R1, @R1(EFI_TABLE_HEADER.HeaderSize)
  MOV R1, @R1(EFI_TABLE_HEADER.Reserved)
  MOV R1, @R1(EFI_SYSTEM_TABLE.ConsoleInHandle)
  MOV R1, @R1(EFI_SYSTEM_TABLE.ConOut)
  BREAK 3
  ; And thank you, intel BUGGY EBC Debugger, for making me believe
  ; we had an issue with CALL[EX] computed indexes... >_<
  CALLEX @R1(EFI_SYSTEM_TABLE.ConOut)
  CALLEX @R1(SIMPLE_TEXT_INPUT_INTERFACE.Reset)

section '.data' data readable writeable

section '.reloc' fixups data discardable
