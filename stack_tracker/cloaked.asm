;
; Cloaked - Test "cloaked" stack operations
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

TEST_NAME = "Cloaked"

include 'head.inc'

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1

  ; Save the stack pointer
  MOV       R1, R0
  ; Now push 2 discardable values. Make sure their
  ; types are different from the actual parameters
  PUSH64    R0
  PUSHn     R0
  ; Restore to the original stack pointer
  MOV       R0, R1

  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam5)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed


  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  
  MOVIq     R1, 0xDEADBEEFDEADBEEF
  ; Push a 64-bit and a natural value to discard
  PUSH64    R1
  PUSHn     R1
  ; Save stack pointer
  MOV       R1, R0
  ; Discard using R1
  MOV       R1, R1(+1,+8)
  ; Restore to updated stack pointer
  MOV       R0, R1

  MOVIq     R1, 0x3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam10)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

include 'tail.inc'
