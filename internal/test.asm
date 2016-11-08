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
  MOVI  R6, EFIAPI(INT64, INT32, CHAR8*, UINT64, INT32)
  MOVsn @R0(+1,+8), R1(12345678)

section '.data' data readable writeable

  Test: EFIAPI(UINT64, VOID*, INT64, INT32)

section '.reloc' fixups data discardable
