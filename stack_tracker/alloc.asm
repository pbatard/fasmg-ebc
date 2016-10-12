;
; Alloc - Test stack tracker for reallocted stack
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;
; Note: This test is not included in our suite because switching stack
; buffers, even temporarily, freezes EBC execution regardless of arch.
; You can confirm this by trying to run the 'stack.asm' sample in the
; main directory.
;

TEST_NAME = "Alloc"

include 'head.inc'

  ; Try with a stack buffer in the data section
  MOV       R6, R0
  MOVREL    R0, StackTop

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
  MOV       R0, R6  ; Revert to old stack
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  ; Try with a newly allocated stack buffer
  MOVREL    R1, Buffer
  PUSHn     R1
  MOVI      R1, 2048
  PUSHn     R1
  MOVI      R1, EfiBootServicesData
  PUSHn     R1
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.AllocatePool)
  MOV       R0, R0(+3,+0)

  MOVREL    R1, APMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed
  
  ; Keep a copy of the old stack pointer
  MOV       R6, R0
  MOVREL    R1, Buffer
  MOV       R0, @R1
  MOV       R0, R0(+0,+2040)

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
  MOV       R0, R6  ; Revert to old stack
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     Failed

  MOVREL    R1, Buffer
  PUSHn     @R1
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.FreePool)
  POPn      R1

include 'tail.inc'

  APMsg:    du "AllocatePool: ", 0x00
  ; Local stack
  StackBuf: dq 255
  StackTop: dq 1
  Buffer:   dq 1

section '.reloc' fixups data discardable