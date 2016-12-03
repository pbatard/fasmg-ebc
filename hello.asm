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

; Assembly notes:
; - Uses PUSHn (PUSH native) for external calls
; - Function parameters are pushed in reverse order (CDECL)
; - R0 is the stack pointer, so MOV R0, R0(+2,0) is equivalent to 2 x POPn

section '.text' code executable readable
EfiMain:
  MOVn   R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R1, @R1(EFI_SYSTEM_TABLE.ConOut)
  MOVREL R2, Hello
  PUSHn  R2
  PUSHn  R1
  CALLEX @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
  MOV R0, R0(+2,0)

  MOVn   R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R1, @R1(EFI_SYSTEM_TABLE.ConIn)
  MOVI   R2, FALSE
  PUSHn  R2
  PUSHn  R1
  CALLEX @R1(SIMPLE_TEXT_INPUT_INTERFACE.Reset)
  POPn   R1
  POPn   R2

  MOVn   R3, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R3, @R3(EFI_SYSTEM_TABLE.BootServices)
  MOVREL R2, Event
  PUSHn  R2
  MOV    R1, R1(SIMPLE_TEXT_INPUT_INTERFACE.WaitForKey)
  PUSHn  R1
  MOVI   R1, 1
  PUSHn  R1
  CALLEX @R3(EFI_BOOT_SERVICES.WaitForEvent)
  MOV    R0, R0(+3,0)

  MOVn   R6, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  MOVn   R6, @R6(EFI_SYSTEM_TABLE.RuntimeServices)
  MOVI   R1, EfiResetShutdown
  MOVI   R2, EFI_SUCCESS
  MOVI   R3, 0
  PUSHn  R3
  PUSHn  R3
  PUSHn  R2
  PUSHn  R1
  CALLEX @R6(EFI_RUNTIME_SERVICES.ResetSystem)
  MOV    R0, R0(+4,0)
  RET

section '.data' data readable writeable
  Event:    dq ?
  Hello:    du 0x0D, 0x0A
            du "Hello EBC World!", 0x0D, 0x0A
            du 0x0D, 0x0A
            du "Press any key to exit", 0x0D, 0x0A
            du 0x00
