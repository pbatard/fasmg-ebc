;
; Matrix - Test the full matrix of natural/64-bit 4 parameters native calls
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

struct EFI_CUSTOM_PROTOCOL
  MultiParam0       VOID_PTR
  MultiParam1       VOID_PTR
  MultiParam2       VOID_PTR
  MultiParam3       VOID_PTR
  MultiParam4       VOID_PTR
  MultiParam5       VOID_PTR
  MultiParam6       VOID_PTR
  MultiParam7       VOID_PTR
  MultiParam8       VOID_PTR
  MultiParam9       VOID_PTR
  MultiParam10      VOID_PTR
  MultiParam11      VOID_PTR
  MultiParam12      VOID_PTR
  MultiParam13      VOID_PTR
  MultiParam14      VOID_PTR
  MultiParam15      VOID_PTR
  MaxParams64       VOID_PTR
  MaxParamsMixed    VOID_PTR
ends

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

CallFailed:
  PUSH      R7
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex32
  POP       R7
  JMP       Exit
  
Failed:
  MOVREL    R1, FailMsg
  PUSH      R1
  CALL      Print
  POP       R1
  JMP       Exit

EfiMain:
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  MOVREL    R2, CustomProtocolGuid
  XOR       R3, R3
  MOVREL    R4, CustomProtocolInterface
  PUSHn     R4
  PUSHn     R3
  PUSHn     R2
  CALLEX    @R1(EFI_BOOT_SERVICES.LocateProtocol)
  MOV       R0, R0(+3,0)

  MOVREL    R1, LPMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  ; Push an extra 64 bit value so that we don't end up with a
  ; test that passes due to a lucky match with previous entries
  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam0)
  MOV       R0, R0(+4,+8)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam1)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam2)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam3)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam4)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam5)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam6)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam7)
  MOV       R0, R0(+1,+32)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam8)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam9)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam10)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam11)
  MOV       R0, R0(+1,+32)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam12)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam13)
  MOV       R0, R0(+1,+32)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam14)
  MOV       R0, R0(+1,+32)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0x5B5B5B5B5A5A5A5A
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam15)
  MOV       R0, R0(+0,+40)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVREL    R1, PassMsg
  PUSH      R1
  CALL      Print
  POP       R1

Exit:
  RET

section '.data' data readable writeable
  gST:      dq ?
  Event:    dq ?
  CustomProtocolGuid:
            EFI_GUID { 0x9bb363b1, 0xb588, 0x4e45, {0x88, 0x06, 0x5f, 0x69, 0x56, 0xae, 0xad, 0xb4} }
  CustomProtocolInterface:
            rq 7
  Digits:   du "0123456789ABCDEF"
  HexStr:   du "0x12345678", 0x0D, 0x0A, 0x00
  LPMsg:    du "LocateProtocol: ", 0x00
  PassMsg:  du "Matrix test: PASSED", 0x0D, 0x0A, 0x00
  FailMsg:  du "Matrix test: FAILED", 0x0D, 0x0A, 0x00

section '.reloc' fixups data discardable
