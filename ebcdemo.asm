; ebcdemo.asm
; Taken from EBCDebugger's EbcDemo/EbcDemo.cod
; See http://www.uefi.org/node/550

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

section '.text' code executable readable
EfiMain:
  MOVqw     R0, R0(+0,-32)
  MOVnw     @R0, R6
  MOVnw     R7, @R0
  CMPIugte  R7, 40
  JMP8cs    _B1_4
_B1_3:
  MOVnw     R7, @R0
  MOVqw     R4, R0(+0,+8)
  MOVnw     R5, @R0
  ADD       R4, R5
  MOVbw     @R4, R7
  MOVnw     R7, @R0
  ADD       R7, R6(+0,+1)
  MOVnw     @R0, R7
  MOVnw     R7, @R0
  CMPIugte  R7, 40
  JMP8cc    _B1_3
_B1_4:
  MOVqd     R7, R6
  MOVqw     R0, R0(+0,+32)
  RET

section '.data' data readable writeable

section '.reloc' fixups data discardable
