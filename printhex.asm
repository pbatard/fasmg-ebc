;
; PrintHex - Print a 64-bit value as Hex
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
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

PrintHex:
  MOVI      R6, 0
  MOV       R3, R6
  NOT       R4, R6
  MOVREL    R5, Digits
  MOVREL    R7, Value
  ADD       R7, R6(4)
  PUSH      @R0(0,+16)
Loop:
  MOV       R1, @R0
  EXTNDD    R2, R6(4)
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
  JMPcc     Loop
  POP       R1
  MOVREL    R1, Value
  PUSH      R1
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

  MOVREL    R6, gST
  MOV       R6, @R6
  MOVn      R6, @R6(EFI_SYSTEM_TABLE.RuntimeServices)
  MOVI      R1, EfiResetShutdown
  MOVI      R2, EFI_SUCCESS
  MOVI      R3, 0
  PUSHn     R3
  PUSHn     R3
  PUSHn     R2
  PUSHn     R1
  CALLEX    @R6(EFI_RUNTIME_SERVICES.ResetSystem)
  MOV       R0, R0(+4,0)
  RET

EfiMain:
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

  MOVREL    R1, InitMsg
  PUSH      R1
  CALL      Print
  POP       R1

  MOVREL    R1, EfiMain
  PUSH      R1
  CALL      PrintHex
  POP       R1
 
  JMP       WaitForKeyAndShutdown

section '.data' data readable writeable
  gST:      dq ?
  Event:    dq ?
  Digits:   du "0123456789ABCDEF"
  Value:    du "0x1234567812345678", 0x0D, 0x0A
            du 0x00
  InitMsg:  du "Entry point: "
            du 0x00
  ExitMsg:  du "Press any key to exit", 0x0D, 0x0A
            du 0x00

section '.reloc' fixups data discardable
