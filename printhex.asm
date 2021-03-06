;
; PrintHex - Print a 64-bit value as Hex
; Copyright � 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

section '.text' code executable readable

Print:
  MOVREL    R1, gST
  MOV       R1, @R1
  MOVn      R1, @R1(EFI_SYSTEM_TABLE.ConOut)
  PUSHn     @R0(0,+16)
  PUSHn     R1
  CALLEX    @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
  MOV       R0, R0(+2,0)
  RET

PrintHex64:
  MOV       R3, R6
  NOT       R4, R6
  MOVREL    R7, HexStr64
  PUSH      R7
  MOV       R1, @R0(0,+24)
  JMP       PrintHexCommon
PrintHex32:
  MOVI      R3, 8
  NOT32     R4, R6
  MOVREL    R7, HexStr32
  PUSH      R7
  MOV       R1, @R0(0,+24)
  AND       R1, R4
PrintHexCommon:
  PUSH      R1
  ADD       R7, R6(4)
  MOVREL    R5, Digits
@0:
  MOV       R1, @R0
  MOVI      R2, 4
  MUL       R2, R3(-15)
  NEG       R2, R2
  SHR       R1, R2
  ADD       R1, R1
  PUSH      R5
  ADD       R5, R1
  MOVw      @R7, @R5
  ADD       R7, R6(2)
  POP       R5
  SHR       R4, R6(4)
  AND       @R0, R4
  ADD       R3, R6(1)
  CMPIgte   R3, 16
  JMPcc     @0b
  POP       R1
  ; HexStr32 or HexStr64 is on top of stack
  CALL      Print
  POP       R1
  RET

WaitForKeyAndShutdown:
  MOVREL    R1, ExitMsg
  PUSH      R1
  CALL      Print
  POP       R1

  MOVREL    R1, gST
  MOV       R1, @R1
  MOVn      R1, @R1(EFI_SYSTEM_TABLE.ConIn)
  MOVI      R2, FALSE
  PUSHn     R2
  PUSHn     R1
  CALLEX    @R1(SIMPLE_TEXT_INPUT_INTERFACE.Reset)
  POPn      R1
  POPn      R2

  MOVREL    R3, gST
  MOV       R3, @R3
  MOVn      R3, @R3(EFI_SYSTEM_TABLE.BootServices)
  MOVREL    R2, Event
  PUSHn     R2
  MOV       R1, R1(SIMPLE_TEXT_INPUT_INTERFACE.WaitForKey)
  PUSHn     R1
  MOVI      R1, 1
  PUSHn     R1
  CALLEX    @R3(EFI_BOOT_SERVICES.WaitForEvent)
  MOV       R0, R0(+3,0)

  MOVREL    R3, gST
  MOV       R3, @R3
  MOVn      R3, @R3(EFI_SYSTEM_TABLE.RuntimeServices)
  MOVI      R1, EfiResetShutdown
  MOVI      R2, EFI_SUCCESS
  PUSHn     R6
  PUSHn     R6
  PUSHn     R2
  PUSHn     R1
  CALLEX    @R3(EFI_RUNTIME_SERVICES.ResetSystem)
  MOV       R0, R0(+4,0)
  RET

EfiMain:
  XOR       R6, R6
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

  MOVREL    R1, EpMsg
  PUSH      R1
  CALL      Print
  POP       R1

  MOVREL    R1, EfiMain
  PUSH      R1
  CALL      PrintHex64
  POP       R1
 
  JMP       WaitForKeyAndShutdown

section '.data' data readable writeable
  gST:      dq ?
  Event:    dq ?
  Digits:   du "0123456789ABCDEF"
  HexStr32: du "0x12345678", 0x0D, 0x0A, 0x00
  HexStr64: du "0x1234567812345678", 0x0D, 0x0A, 0x00
  EpMsg:    du "Entry point: ", 0x00
  ExitMsg:  du "Press any key to exit", 0x0D, 0x0A, 0x00
