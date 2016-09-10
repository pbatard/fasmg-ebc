;
; EBC (EFI Byte Code) assembly 'Hello World'
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc dll efi
entry EfiMain

section '.text' code executable readable
EfiMain:
  ; EfiMain() is handled like any other EBC CALL routine, therefore
  ; we need to skip c=+16 bytes to access our parameters, due to the
  ; following stack manipulation operations having been carried out:
  ;   R0 = R0 - 8           ; -> +8
  ;   PUSH64 ReturnAddress  ; -> +8
  ; Knowing this, the indexes for the two EfiMain() parameters are
  ; 'ImageHandle' at (+0,+16) and 'SystemTable' at (+1,+16).
  MOVn   R1, @R0(+1,+16)    ; SystemTable
  MOVn   R1, @R1(+5,+24)    ; SystemTable->ConOut
  MOVREL R2, Hello
  PUSHn  R2                 ; EX call -> native PUSH (PUSHn)
  PUSHn  R1                 ; CDECL -> Params pushed in reverse order
  CALLEX @R1(+1,+0)         ; SystemTable->ConOut->OutputString()
  POPn   R1
  POPn   R2

  MOVn   R1, @R0(+1,+16)    ; SystemTable
  MOVn   R1, @R1(+3,+24)    ; SystemTable->ConIn
  MOVI   R2, 0              ; FALSE
  PUSHn  R2
  PUSHn  R1
  CALLEX @R1(+0,+0)         ; SystemTable->ConIn->Reset()
  POPn   R1
  POPn   R2

  MOVREL R2, InputKey
WaitForKey:
  PUSHn  R2
  PUSHn  R1
  CALLEX @R1(+1,+0)         ; SystemTable->ConIn->ReadKeyStroke()
  POPn   R1
  POPn   R2
  MOVI   R3, EFI64_NOT_READY
  CMPeq  R7, R3             ; NB: we must test both the 32 and 64-bit status codes
  JMPcs  WaitForKey
  CMPI32deq R7, EFI32_NOT_READY
  JMPcs  WaitForKey

  MOVn   R6, @R0(+1,+16)    ; SystemTable
  MOVn   R6, @R6(+8,+24)    ; SystemTable->RuntimeServices
  MOVI   R1, EfiResetShutdown
  MOVI   R2, EFI64_SUCCESS
  MOVI   R3, 0
  PUSHn  R3
  PUSHn  R3
  PUSHn  R2
  PUSHn  R1
  CALLEX @R6(+10,+24)       ; SystemTable->RuntimeServices->ResetSystem()
  POPn   R1
  POPn   R2
  POPn   R3
  POPn   R3
  RET

section '.data' data readable writeable
  InputKey: dq ?
  Hello:    du 0x0D, 0x0A
            du "Hello EBC World!", 0x0D, 0x0A
            du 0x0D, 0x0A
            du "Press any key to exit", 0x0D, 0x0A
            du 0x00

section '.reloc' fixups data discardable
