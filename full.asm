;
; EBC (EFI Byte Code) full instruction set test
;

include 'ebc.inc'
include 'format.inc'

format peebc dll efi
entry EfiMain

section '.text' code executable readable

EfiMain:
  ADD32 R1, R0
  AND32 @R1, @R0
  ASHR32 @R1, R0(1234)
  DIV32 R1, @R0(+12,+24)
  DIVU64 R1, R0
  EXTNDB64 R1, @R0
  EXTNDD64 R1, R0(5678)
  EXTNDW64 @R1, @R0(+12,+24)
  MOD R2, R6
  MODU @R2, R3
  MUL R5, @R5
  MULU @R6, R6(4321)
  NEG R7, R6(0xFFFF)
  NOT32 R3, R1(-56)
  OR64 @R4, R1(-0x8000)
  SHL @R2, R0(-0x12,-0x30)
  SHR32 R1, @R1(0,EfiMain-$)
  SUB64 @R0, R1(0x1000*0x08)
  XOR32 @R1, @R1(-12, EfiMain-Exit)
  ADD R7, R6(+0,+1)

  CALL32 R1
  CALL32 R1(EfiMain)
  CALL64 0x1234567812345678
  CALL 0x123456
  CALLEX Exit + 2
  CALLEX32 @R0(Exit - EfiMain, 1)

  CMP32eq R1, R0
  CMP64lte R1, @R0
  CMPgte R1, R4(-5678)
  CMP32ulte R1, @R4(-12,-124)
  CMP64ugte R2, R4(-0,+60)
  CMPI32weq @R1, 1234 - 5678
  CMPI64dlte R2, 0x8000000
  CMPIgte @R3(+12,+68), 0x8000000
  CMPI32wulte @R2(-12,16-32), 1234-5678
  CMPI64dugte R2, EfiMain

  JMP Exit
  JMP32cc R1(Exit)
  JMP32cc R1(0x12345678)
  JMP64cs 0x8000000000000
  JMP @R1(0x80000+0x800000,7*9-56)
  JMP8cc Exit
  JMP -0x8000000000000000

  LOADSP [Flags], R7
  STORESP R1, [IP]
  STORESP R2, [Flags]
  POP32 R1
  POP64 R3(1234)
  POP32 R1(+1,+8)
  POP @R4(512,2)
  POPn @R5(-11,-32)
  PUSH R0
  PUSH64 @R5
  PUSH32 R6(1024)
  PUSHn @R1(-10,-200)
  
Exit:
  MOVbw @R1(+7,+128), @R2
  MOVbd @R1(-128,-512), @R2((1-8),(2-5))
  MOVb @R1(0x800,0x800), R3
  MOVww @R1, @R3(-12,-64)
  MOVqq @R2, @R3
  MOVqq @R1(-0xFFFFFFFF,-0xFFFFFFF), R2
  MOV @R1(-0xFFFFFFFF,-0xFFFFFFF), R2
  MOVqq @R1(0x8000000,0x8000000), @R2(0x8000000,0x8000000)
  MOV @R1(0x8000000,0x8000000), @R2(0x8000000,0x8000000)
  MOVqq @R1(-0x7FFFFFF,-0x7FFFFFF), @R2(-0x7FFFFFFF,-0x7FFFFFF)
  MOVqd @R1(-0x1FFF, -0xFFF), R2
  MOVdd @R1, @R2
  MOV @R1, @R2
  MOVqw R0, R0(+0,+112)

  MOVIww R1, 0xFFFF
  MOVIwd @R1, -0x7FFF
  MOVIw @R1, Exit
  MOVI @R1(-0xF, -0xF), (EfiMain - Exit)*8
  MOVIq @R1(+5,+9), 0x123456789ABCD
  MOVI @R1(+5,+9), 0x123456789ABCD
  MOVInw R1, (+12,+50)
  MOVInd @R1, (-0x1FFF,-0xFFF)
  MOVIn @R1, (-0x1FFF,-0xFFF)
  MOVInq @R2(+8,+16), (-0x7FFFFFFF,-0x7FFFFFF)
  MOVIn @R2(+8,+16), (-0x7FFFFFFF,-0x7FFFFFF)

  MOVnw R0, R1
  MOVnd @R0, @R1
  MOVnw @R0(55,44), R1
  MOVnd R0, R0(+2222,+111)
  MOVn @R0(1234,5678), @R2(2345,6789)
  MOVsnw R0, R1(1235)
  MOVsnd @R0(45,67), R1((EfiMain - Exit)*8)
  MOVsn @R0(+1,+8), R1(12345678)
  MOVsnd @R0(-0x1FFF,-0xFFF), R1(0xFFFFFFFF)
  MOVsn @R0(-0x1FFF,-0xFFF), @R0(-0x1FFF,-0xFFF)
  
  MOVRELw R1, -12
  MOVRELd @R1, EfiMain
  MOVREL @R1(+64,+4), 0x123456789ABCD

  RET
  BREAK 12

section '.data' data readable writeable

section '.reloc' fixups data discardable
