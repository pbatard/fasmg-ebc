;
; EBC (EFI Byte Code) assembly 'Hello World'
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

section '.text' code executable readable
EfiMain:
  MOVn   R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R1, @R1(EFI_SYSTEM_TABLE.ConOut)
  MOVREL R2, Hello
  PUSHn  R2                 ; EX call -> native PUSH (PUSHn)
  PUSHn  R1                 ; CDECL -> Params pushed in reverse order
  CALLEX @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
  POPn   R1
  POPn   R2

  MOVn   R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R1, @R1(EFI_SYSTEM_TABLE.ConIn)
  MOVI   R2, FALSE
  PUSHn  R2
  PUSHn  R1
  CALLEX @R1(0,0) ; (SIMPLE_TEXT_INPUT_INTERFACE.Reset)
  POPn   R1
  POPn   R2

  MOVREL R2, InputKey
WaitForKey:
  PUSHn  R2
  PUSHn  R1
  ; NB: There also exists a WaitForKey()...
  CALLEX @R1(SIMPLE_TEXT_INPUT_INTERFACE.ReadKeyStroke)
  POPn   R1
  POPn   R2
  MOVI   R3, EFI64_NOT_READY
  CMPeq  R7, R3             ; Must test BOTH the 32 and 64-bit status codes
  JMPcs  WaitForKey
  CMPI32deq R7, EFI32_NOT_READY
  JMPcs  WaitForKey

  MOVn   R6, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R6, @R6(EFI_SYSTEM_TABLE.RuntimeServices)
  MOVI   R1, EfiResetShutdown
  MOVI   R2, EFI64_SUCCESS
  MOVI   R3, 0
  PUSHn  R3
  PUSHn  R3
  PUSHn  R2
  PUSHn  R1
  CALLEX @R6(EFI_RUNTIME_SERVICES.ResetSystem)
  POPn   R1
  POPn   R2
  POPn   R3
  POPn   R3
  RET

section '.data' data readable writeable
  InputKey: dq ?
  Hello:    du 0x0D, 0x0A
            du "Hello EBC World!", 0x0D, 0x0A
            du 0x0D, 0x0A
            du "Press any key to exit", 0x0D, 0x0A
            du 0x00

section '.reloc' fixups data discardable
