;
; Stack - Test EBC stack buffer switching
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;
; Note that, currently, this doesn't work on ANY platform
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

section '.text' code executable readable
EfiMain:
  MOV       R6, R0
  MOVREL    R0, StackTop
  MOV       R0, R6
  RET

section '.data' data readable writeable
  StackBuf: dq 255
  StackTop: dq 1
