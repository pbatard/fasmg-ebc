;
; Matrix - Test the full matrix of natural/64-bit 4 parameters native calls
; Copyright � 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

TEST_NAME = "Matrix"

include 'head.inc'

  ; Push an extra 64 bit value so that we don't end up with a
  ; test that passes due to a lucky match with previous entries
  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam0)
  MOV       R0, R0(+4,+8)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam1)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam2)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3A3A3A3A
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

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam4)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
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
  MOVIq     R1, 0x4A4A4A4A
  PUSHn     R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam6)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4A4A4A4A
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

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam8)
  MOV       R0, R0(+3,+16)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3A3A3A3A
  PUSHn     R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam9)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
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

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3A3A3A3A
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

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam12)
  MOV       R0, R0(+2,+24)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2A2A2A2A
  PUSHn     R1
  MOVIq     R1, 0x1B1B1B1B1A1A1A1A
  PUSH64    R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam13)
  MOV       R0, R0(+1,+32)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
  PUSH64    R1
  MOVIq     R1, 0x4B4B4B4B4A4A4A4A
  PUSH64    R1
  MOVIq     R1, 0x3B3B3B3B3A3A3A3A
  PUSH64    R1
  MOVIq     R1, 0x2B2B2B2B2A2A2A2A
  PUSH64    R1
  MOVIq     R1, 0x1A1A1A1A
  PUSHn     R1
  MOVREL    R1, CustomProtocolInterface
  MOVn      R1, @R1
  CALLEX    @R1(EFI_CUSTOM_PROTOCOL.MultiParam14)
  MOV       R0, R0(+1,+32)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVIq     R1, 0xDEADBEEFDEADBEEF
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

include 'tail.inc'
