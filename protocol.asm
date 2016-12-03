;
; Protocol - Invocation of native UEFI protocols
; See protocol_driver/driver.c for more details
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

EFI_IMAGE_MACHINE_IA32 = 0x014c
EFI_IMAGE_MACHINE_X64  = 0x8664
EFI_IMAGE_MACHINE_ARM  = 0x01C2
EFI_IMAGE_MACHINE_AA64 = 0xAA64

struct EFI_CUSTOM_PROTOCOL
  Isa               UINTN
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
  CALL      Print
  POP       R1
  RET

LocateProtocolFailed:
  PUSH      R7
  MOVREL    R1, LPMsg
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex64
  POP       R7
  RET

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

  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     LocateProtocolFailed

  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.Hello)

  MOVREL    R1, ISAMsg
  PUSH      R1
  CALL      Print
  POP       R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  MOVn      R6, @R1(EFI_CUSTOM_PROTOCOL.Isa)
  PUSH      R6
  CALL      PrintHex32
  POP       R6

  ; From UEFI 2.6, 21.9.3:
  ;
  ; All parameters are stored or accessed as natural size (using naturally
  ; sized instruction) except 64-bit integers, which are pushed as 64-bit
  ; values. 32-bit integers are pushed as natural size (since they should be
  ; passed as 64-bit parameter values on 64-bit machines).
  ;
  MOVIq     R1, 0x7B7B7B7B7A7A7A7A
  PUSH64    R1
  MOVId     R1, 0x6C6C6C6C
  PUSHn     R1
  MOVId     R1, 0x5C5C5C5C
  PUSHn     R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVId     R1, 0x1C1C1C1C
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParamFixed)
  MOV       R0, R0(+3,+32)

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

  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParamNative)
  MOV       R0, R0(+12,+0)

  RET

section '.data' data readable writeable
  gST:      dq ?
  Event:    dq ?
  CustomProtocolGuid:
            EFI_GUID { 0x1e81aff7, 0x5509, 0x4acc, {0xa9, 0x3f, 0x56, 0x55, 0x0d, 0xb1, 0xbd, 0xcc} }
  CustomProtocolInterface:
            rb EFI_CUSTOM_PROTOCOL.__size
  Digits:   du "0123456789ABCDEF"
  ISAMsg:   du "  ISA = ", 0x00
  HexStr32: du "0x12345678", 0x0D, 0x0A, 0x00
  HexStr64: du "0x1234567812345678", 0x0D, 0x0A, 0x00
  LPMsg:    du "LocateProtocol: ", 0x00
