;
; Common data for stack tracker testing - tail part
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

  MOVREL    R1, PassMsg
  PUSH      R1
  CALL      Print
  POP       R1

Exit:
  RET

section '.data' data readable writeable
  gST:      dq ?
  LPMsg:    du "LocateProtocol: ", 0x00
  CustomProtocolGuid:
            EFI_GUID { 0x9bb363b1, 0xb588, 0x4e45, {0x88, 0x06, 0x5f, 0x69, 0x56, 0xae, 0xad, 0xb4} }
  CustomProtocolInterface:
            rb EFI_CUSTOM_PROTOCOL.__size
  Digits:   du "0123456789ABCDEF"
  HexStr:   du "0x12345678", 0x0D, 0x0A, 0x00
  PassMsg:  du TEST_NAME, " test: PASSED", 0x0D, 0x0A, 0x00
  FailMsg:  du TEST_NAME, " test: FAILED", 0x0D, 0x0A, 0x00
