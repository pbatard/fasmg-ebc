;
; Arch - Identifies the underlying UEFI firmware architecture, from EBC
; A.k.a. “If you can think of a better way to get ice, I'd like to hear it!”
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

format peebc efi
entry EfiMain

section '.text' code executable readable

Print:
  MOVREL    R1, gST
  MOV       R1, @R1
  MOVn      R1, @R1(EFI_SYSTEM_TABLE.ConOut)
  PUSHn     @R0(0,+16)
  PUSHn     R1
  CALLEX    @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
  MOV       R0, R0(+2,0)
  RET

PrintHex64:
  MOV       R3, R6
  NOT       R4, R6
  MOVREL    R7, HexStr64
  PUSH      R7
  MOV       R1, @R0(0,+24)
  JMP       PrintHexCommon
PrintHex32:
  MOVI      R3, 8
  NOT32     R4, R6
  MOVREL    R7, HexStr32
  PUSH      R7
  MOV       R1, @R0(0,+24)
  AND       R1, R4
PrintHexCommon:
  PUSH      R1
  ADD       R7, R6(4)
  MOVREL    R5, Digits
@0:
  MOV       R1, @R0
  MOVI      R2, 4
  MUL       R2, R3(-15)
  NEG       R2, R2
  SHR       R1, R2
  ADD       R1, R1
  PUSH      R5
  ADD       R5, R1
  MOVw      @R7, @R5
  ADD       R7, R6(2)
  POP       R5
  SHR       R4, R6(4)
  AND       @R0, R4
  ADD       R3, R6(1)
  CMPIgte   R3, 16
  JMPcc     @0b
  POP       R1
  CALL      Print
  POP       R1
  RET

CallFailed:
  PUSH      R7
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex64
  POP       R7
  JMP       FreeBuffer

EfiMain:
  XOR       R6, R6
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

  ; Buckle up kids, coz this is what you need to do to find the UEFI Arch:
  ; 0. First of all, don't foolishly think that you can get it by using the
  ;    LoadedImage protocol on the current EBC image, through an ancestor
  ;    ParentHandle, as none of these attributes, starting with an EBC runtime,
  ;    seem to point to anything native (so all you'll ever get is 0x0EBC).
  ; 1. This means that you somehow have to locate a handle to a running native
  ;    image, that is also friendly to the LoadedImage protocol, since that's
  ;    what we need to access the machine type. Luckily, LocateHandle() allows
  ;    you do do exactly that, when used with LoadedImageProtocolGuid ...except
  ;    you can't just ask it to return one handle - you must get the whole lot.
  ; 2. So, first, you need to issue a dummy call to LocateHandle(), to figure
  ;    out the size your full handle list will require.
  ; 3. Then, after allocating that buffer, you can call LocateHandle() again to
  ;    get that list.
  ; 4. You can now pick the first handle there (which is unlikely to be an
  ;    EBC image, though, if we really wanted to be pedantic, we could also
  ;    test for this), and feed it to OpenProtocol() for LoadedImage access.
  ; 5. From there, you have the ImageBase attribute, which in turn gives you
  ;    access the Machine Type field in the PE header, and that, at last, tells
  ;    you the type of UEFI firmware arch you're running on.
  ; So, as I was saying, “If you can think of a better way to get ice, I'd like
  ; to hear it!”

  ; Dummy LocateHandle() call to get the HandleList size
  MOVREL    R2, HandleList
  PUSHn     @R2
  MOVREL    R2, Size
  PUSHn     R2
  PUSHn     R6
  MOVREL    R2, LoadedImageProtocolGuid
  PUSHn     R2
  MOVI      R2, ByProtocol
  PUSHn     R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.LocateHandle)
  MOV       R0, R0(+5,+0)

  ; Expected status is EFI_BUFFER_TOO_SMALL
  MOVI      R2, EFI_BUFFER_TOO_SMALL
  CMPeq     R7, R2
  JMPcs     @0f
  CMPI32deq R7, EFI_32BIT_ERROR or (EFI_BUFFER_TOO_SMALL and EFI_32BIT_MASK)
  JMPcs     @0f
  MOVREL    R1, LhMsg
  JMP       CallFailed

@0:
  ; Now that we have the size, allocate our buffer
  MOVREL    R2, HandleList
  PUSHn     R2
  MOVREL    R2, Size
  PUSHn     @R2
  MOVI      R2, EfiBootServicesData
  PUSHn     R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.AllocatePool)
  MOV       R0, R0(+3,+0)

  MOVREL    R1, ApMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  ; Real call to retreive our handle list
  MOVREL    R2, HandleList
  PUSHn     @R2
  MOVREL    R2, Size
  PUSHn     R2
  PUSHn     R6
  MOVREL    R2, LoadedImageProtocolGuid
  PUSHn     R2
  MOVI      R2, ByProtocol
  PUSHn     R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.LocateHandle)
  MOV       R0, R0(+5,+0)

  MOVREL    R1, LhMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  ; Pick the first handle from the list, and access the LoadedImage
  MOVREL    R2, HandleList
  MOVn      R2, @R2
  MOVI      R3, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
  PUSHn     R3
  PUSHn     R6
  PUSHn     @R2
  MOVREL    R3, LoadedImage
  PUSHn     R3
  MOVREL    R3, LoadedImageProtocolGuid
  PUSHn     R3
  PUSHn     @R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.OpenProtocol)
  MOV       R0, R0(+6,+0)
  MOVREL    R1, OpMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  ; Navigate the PE header to read the Machine Type
  MOVREL    R5, LoadedImage
  MOV       R5, @R5
  MOV       R5, @R5(EFI_OPEN_PROTOCOL.ImageBase)
  ADD32     R5, @R5(+0,+0x3C)
  MOV       R5, @R5(+0,+4)
  MOVIw     R4, 0xFFFF
  AND       R5, R4
  PUSH      R5
  MOVREL    R1, ArMsg
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex32
  POP       R1

FreeBuffer:
  MOVREL    R1, HandleList
  PUSHn     @R1
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.FreePool)
  POPn      R1
BREAK 3
  RET

section '.data' data readable writeable
  gST:      dq ?
  LoadedImageProtocolGuid:
            EFI_GUID { 0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B} }
  LoadedImage:
            dq ?
  Size:     dq 0
  HandleList:
            dq 0
  Digits:   du "0123456789ABCDEF"
  HexStr32: du "0x12345678", 0x0D, 0x0A, 0x00
  HexStr64: du "0x1234567812345678", 0x0D, 0x0A, 0x00
  ArMsg     du "Detected UEFI Arch: ", 0x00
  OpMsg:    du "OpenProtocol: ", 0x00
  LhMsg:    du "LocateHandle: ", 0x00
  ApMsg:    du "AllocatePool: ", 0x00
