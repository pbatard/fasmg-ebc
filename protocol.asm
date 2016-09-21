;
; Protocol - Fun with native UEFI protocols
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

PrintHex:
  MOVI      R6, 0
  MOV       R3, R6
  NOT       R4, R6
  MOVREL    R5, Digits
  MOVREL    R7, Value
  ADD       R7, R6(20)
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

LocateProtocolFailed:
  PUSH      R7
  MOVREL    R1, LPMsg
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex
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

  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  MOVn      R6, @R1(EFI_CUSTOM_PROTOCOL.Isa)
  PUSH      R6
  CALL      PrintHex
  POP       R6

  ; Because of the nature of EBC, if you are calling into a native function
  ; that uses a mix of 64 and non 64-bit parameters then there's no escaping
  ; understanding how parameters are being passed for each specific
  ; architecture, and compensating for it as required, as the EBC VM can't.
  ; The problem is as follows: Say you have MyCall(UINT32, UINT64, UINT64) to
  ; which you are passing (0x1C1C1C1C, 0x2B2B2B2B2A2A2A2A, 0x3B3B3B3B3A3A3A3A)
  ; In the EBC VM, the paramaters get stacked as (little endian, CDECL):
  ;
  ;   +--------+
  ;   |1C1C1C1C|
  ;   +--------+
  ;   |2A2A2A2A|
  ;   +--------+
  ;   |2B2B2B2B|
  ;   +--------+
  ;   |3A3A3A3A|
  ;   +--------+
  ;   |3B3B3B3B|
  ;   +--------+
  ;   +????????+
  ;   +--------+
  ;
  ; Now, if you are calling into an x86_32 arch, this is no issue, as the
  ; native call reads the parameters off the stack, and finds each one it its
  ; expected location.
  ; But let's say you are calling into an Arm (32 bit) arch. In that case, the
  ; calling convention dictates that the first four 32 bits parameters must be
  ; placed into Arm registers r0-r3, rather than on the stack, and what's more,
  ; that if there exist 64 bit parameters among the register ones, they must
  ; start with an even register (r0 or r2).
  ; What this means is that, with the current EBC handling, which simply maps
  ; the top of the stack onto registers for native CALLEX (as the VM doesn't
  ; know the parameter signature of the function it is calling into) the native
  ; function would get called with the following parameter mapping:
  ;
  ;   +--------+
  ;   |1C1C1C1C|  -> r0 (32-bit first parameter)
  ;   +--------+
  ;   |2A2A2A2A|  -> (r1/unused, as first parameter is 32-bit)
  ;   +--------+
  ;   |2B2B2B2B|  -> r2 (lower half of 64-bit second parameter)
  ;   +--------+
  ;   |3A3A3A3A|  -> r3 (upper half of 64-bit second parameter)
  ;   +--------+
  ;   |3B3B3B3B|  -> lower half of 64-bit third parameter (stack)
  ;   +--------+
  ;   +????????+  -> upper half of 64-bit third parameter (stack)
  ;   +--------+
  ;
  ; The end result is that, as far as the native Arm call is concerned, 
  ; it retrieves the parameters:
  ; (0x1C1C1C1C, 0x3A3A3A3A2B2B2B2B, 0x????????3B3B3B3B)
  ;
  ; The same kind of issue also applies to x86_64 and Arm64, as any of the
  ; first eight parameters to native calls, that are smaller than 64-bit,
  ; also gets extended to 64 bit, when being passed as register.
  ;
  ; Being aware of this, you may have to take arch specific measures, such as
  ; expanding possible register parameters onto the stack, before calling a
  ; native function call.
  ;
  ; in the following section, we provide an example of how this could be
  ; accomplished.
  ; You may also test what happens when not compensating for the above, by
  ; setting NoArchCompensation to 1 in the data section.

  MOVREL    R2, NoArchCompensation
  MOV       R2, @R2
  MOVIq     R1, 0x7B7B7B7B7A7A7A7A
  PUSH64    R1
  MOVId     R1, 0x6C6C6C6C
  PUSH32    R1
  ; Expand to 64 bit for AARCH64 and X64
  CMPIeq    R2, 1 ; NoArchCompensation Override
  JMPcs     NoExtraPushP6
  CMPI32eq  R6, EFI_IMAGE_MACHINE_IA32
  JMPcs     NoExtraPushP6
  CMPI32eq  R6, EFI_IMAGE_MACHINE_ARM
  JMPcs     NoExtraPushP6
  PUSH32    R1
NoExtraPushP6:
  MOVId     R1, 0x5C5C5C5C
  PUSH32    R1
  ; Expand to 64 bit for AARCH64 and X64
  CMPIeq    R2, 1 ; NoArchCompensation Override
  JMPcs     NoExtraPushP5
  CMPI32eq  R6, EFI_IMAGE_MACHINE_IA32
  JMPcs     NoExtraPushP5
  CMPI32eq  R6, EFI_IMAGE_MACHINE_ARM
  JMPcs     NoExtraPushP5
  PUSH32    R1
NoExtraPushP5:
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVId     R1, 0x1C1C1C1C
  ; Expand to 64 bit for ARM, AARCH64 and X64
  CMPIeq    R2, 1 ; NoArchCompensation Override
  JMPcs     NoExtraPushP1
  CMPI32eq  R6, EFI_IMAGE_MACHINE_IA32
  JMPcs     NoExtraPushP1
  PUSH32    R1
NoExtraPushP1:  
  PUSH32    R1

  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParamFixed)
  MOV       R0, R0(+0,+44)
  CMPIeq    R2, 1 ; NoArchCompensation Override
  JMPcs     NoExtraPop
  CMPI32eq  R6, EFI_IMAGE_MACHINE_IA32
  JMPcs     NoExtraPop
  POP32     R1
  CMPI32eq  R6, EFI_IMAGE_MACHINE_ARM
  JMPcs     NoExtraPop
  MOV       R0, R0(+0,+8)
NoExtraPop:

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
  NoArchCompensation:
            dq 0 ; Set to 1 to disable arch specific CALLEX compensation
  gST:      dq ?
  Event:    dq ?
  Digits:   du "0123456789ABCDEF"
  Value:    du "  ISA = 0x12345678", 0x0D, 0x0A
            du 0x00
  LPMsg:    du "LocateProtocol: "
            du 0x00
  CustomProtocolGuid:
            EFI_GUID { 0x1e81aff7, 0x5509, 0x4acc, {0xa9, 0x3f, 0x56, 0x55, 0x0d, 0xb1, 0xbd, 0xcc} }
  CustomProtocolInterface:
            rq 7

section '.reloc' fixups data discardable
