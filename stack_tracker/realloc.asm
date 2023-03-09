;
; Realloc - Test stack tracker reallocation
; Copyright � 2017 Pete Batard <pete@akeo.ie> - Public Domain
;

TEST_NAME = "Realloc"

; As per EbcInt.h
STACK_POOL_SIZE = (1024 * 1020)

include 'head.inc'

  ; Repeat the test a few times. While reallocation of the stack
  ; only occurs on first pass, this allows us to confirm that
  ; tracking (and de-tracking) 512KB of stack data doesn't negatively
  ; impact performance
  MOVI      R1, 9
Test:
  ; The stack tracker is 1/32th of STACK_POOL_SIZE at init.
  ; If we reserve space for more than half STACK_POOL_SIZE, we
  ; will force our stack tracker to realloc.
  MOV       R0, R0(-0, -(STACK_POOL_SIZE/2 + 1024))
  ; Need to push a Natural for our Index to move
  PUSHn     R1
  POPn      R1
  MOV       R0, R0(+0, +(STACK_POOL_SIZE/2 + 1024))
  SUB       R1, R6(1)
  CMPIgte   R1, 0
  JMPcs     Test

include 'tail.inc'
