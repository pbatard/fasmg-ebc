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
  MOVnw  R1, @R0(+1,+16)    ; SystemTable
  MOVnw  R1, @R1(+5,+24)    ; SystemTable->ConOut
  MOVnw  R3, @R1(+1,+0)     ; SystemTable->ConOut->OutputString()
  MOVIqq R2, Hello
  PUSHn  R2                 ; CDECL -> Params pushed in reverse order
  PUSHn  R1
  CALLEX R3                 ; Call OutputString()
  POPn   R1
  POPn   R2
  RET

section '.data' data readable writeable
Hello:   du 0x0D, 0x0A
         du "Hello EBC World!", 0x0D, 0x0A
         du 0x0D, 0x0A

section '.reloc' fixups data discardable
