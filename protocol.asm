;
; Protocol - Fun with UEFI protocols
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

struct EFI_DRIVER_PROTOCOL
  DataNative        UINTN
  Hello             VOID_PTR
  SingleParam32     VOID_PTR
  SingleParam64     VOID_PTR
  SingleParamNative VOID_PTR
  MultiParamFixed   VOID_PTR
  MultiParamNative  VOID_PTR
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

PrintHex:
  MOVI      R6, 0
  MOV       R3, R6
  NOT       R4, R6
  MOVREL    R5, Digits
  MOVREL    R7, Value
  ADD       R7, R6(8)
  PUSH      @R0(0,+16)
Loop:
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
  JMPcc     Loop
  POP       R1
  MOVREL    R1, Value
  PUSH      R1
  CALL      Print
  POP       R1
  RET

EfiMain:
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  MOVREL    R2, Protocol
  XOR       R3, R3
  MOVREL    R4, Interface
  PUSHn     R4
  PUSHn     R3
  PUSHn     R2
  CALLEX    @R1(EFI_BOOT_SERVICES.LocateProtocol)
  MOV       R0, R0(+3,0)

  CMPI32eq  R7, EFI_SUCCESS
  JMPcs     LocateProtocolOK
  PUSH      R7
  MOVREL    R1, LPMsg
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex
  POP       R7
  RET
LocateProtocolOK:
  MOVREL    R1, Interface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_DRIVER_PROTOCOL.Hello)

  MOVREL    R1, Interface
  MOVn      R1, @R1
  MOVn      R1, @R1(EFI_DRIVER_PROTOCOL.DataNative)
  PUSH      R1
  CALL      PrintHex
  POP       R1

  MOVIq     R1, 0x7777777777777777
  PUSH64    R1
  MOVId     R1, 0x66666666
  PUSH32    R1
  MOVId     R1, 0x55555555
  PUSH32    R1
  MOVIq     R1, 0x4444444444444444
  PUSH64    R1
  MOVIq     R1, 0x3333333333333333
  PUSH64    R1
  MOVIq     R1, 0x2222222222222222
  PUSH64    R1
  MOVId     R1, 0x11111111
  PUSH32    R1

  MOVREL    R1, Interface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_DRIVER_PROTOCOL.MultiParamFixed)
  MOV       R0, R0(+0,+44)

  MOVI      R1, 0xCCCCCCCCCCCCCCCC
  PUSHn     R1
  MOVI      R1, 0xBBBBBBBBBBBBBBBB
  PUSHn     R1
  MOVI      R1, 0xAAAAAAAAAAAAAAAA
  PUSHn     R1
  MOVI      R1, 0x9999999999999999
  PUSHn     R1
  MOVI      R1, 0x8888888888888888
  PUSHn     R1
  MOVI      R1, 0x7777777777777777
  PUSHn     R1
  MOVI      R1, 0x6666666666666666
  PUSHn     R1
  MOVI      R1, 0x5555555555555555
  PUSHn     R1
  MOVI      R1, 0x4444444444444444
  PUSHn     R1
  MOVI      R1, 0x3333333333333333
  PUSHn     R1
  MOVI      R1, 0x2222222222222222
  PUSHn     R1
  MOVI      R1, 0x1111111111111111
  PUSHn     R1

  MOVREL    R1, Interface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_DRIVER_PROTOCOL.MultiParamNative)
  MOV       R0, R0(+12,+0)

  RET

section '.data' data readable writeable
            align 8
  gST:      dq ?
  Event:    dq ?
  Digits:   du "0123456789ABCDEF"
  Value:    du "  0x12345678", 0x0D, 0x0A
            du 0x00
  LPMsg:    du "LocateProtocol: "
            du 0x00
  Protocol:
            EFI_GUID { 0x1e81aff7, 0x5509, 0x4acc, {0xa9, 0x3f, 0x56, 0x55, 0x0d, 0xb1, 0xbd, 0xcc} }
            align 8
  Interface:
            rq 7

section '.reloc' fixups data discardable
