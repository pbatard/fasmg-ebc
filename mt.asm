;
; MT - Demonstrate the use of OpenProtocol() by retrieving the
; Machine Type PE field, from the currently loaded image.
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

PrintHex32:
  XOR       R6, R6
  MOV       R3, R6
  NOT32     R4, R6
  MOVREL    R5, Digits
  MOVREL    R7, HexStr
  ADD       R7, R6(4)
  MOV       R1, @R0(0,+16)
  AND       R1, R4
  PUSH      R1
@1:
  MOV       R1, @R0
  EXTNDD    R2, R6(4)
  MUL       R2, R3(-7)
  NEG       R2, R2
  SHR       R1, R2
  ADD       R1, R1
  PUSH      R5
  ADD       R5, R1
  MOVw      @R7, @R5
  ADD       R7, R6(2)
  POP       R5
  SHR32     R4, R6(4)
  AND       @R0, R4
  ADD       R3, R6(1)
  CMPIgte   R3, 8
  JMPcc     @1b
  POP       R1
  MOVREL    R1, HexStr
  PUSH      R1
  CALL      Print
  POP       R1
  RET

OpenProtocolFailed:
  PUSH      R7
  MOVREL    R1, OpMsg
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex32
  POP       R7
BREAK 3
  RET

EfiMain:
  XOR       R6, R6
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)
  
  MOVn      R2, @R0(EFI_MAIN_PARAMETERS.ImageHandle)
  MOVI      R3, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
  PUSHn     R3
  PUSHn     R6
  PUSHn     R2
  MOVREL    R3, LoadedImage
  PUSHn     R3
  MOVREL    R3, LoadedImageProtocolGuid
  PUSHn     R3
  PUSHn     R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.OpenProtocol)
  MOV       R0, R0(+6,+0)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     OpenProtocolFailed

  MOVREL    R5, LoadedImage     ; Access the loaded executable PE image
  MOV       R5, @R5
  MOV       R5, @R5(EFI_OPEN_PROTOCOL.ImageBase)
  ADD32     R5, @R5(+0,+0x3C)   ; PE header location is at offset 0x3C
  MOV       R5, @R5(+0,+4)      ; Machine Type is a word at PE header + 4
  MOVIw     R4, 0xFFFF
  AND       R5, R4
  PUSH      R5
  MOVREL    R1, MtMsg
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex32
  POP       R1  
BREAK 3
  RET  
  
section '.data' data readable writeable
  gST:      dq ?
  LoadedImageProtocolGuid:
            EFI_GUID { 0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B} }
  LoadedImage:
            dq ?
  Digits:   du "0123456789ABCDEF"
  HexStr:   du "0x12345678", 0x0D, 0x0A, 0x00
  MtMsg     du "PE Machine Type = ", 0x00
  OpMsg:    du "OpenProtocol: ", 0x00

section '.reloc' fixups data discardable
