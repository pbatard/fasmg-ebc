;
; Driver - EBC Driver sample
; Copyright © 2016 Pete Batard <pete@akeo.ie>
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

DriverVersion = 0x11

struct EFI_CUSTOM_PROTOCOL
  Hello                 VOID_PTR
repeat 16 i:0
  MultiParam#i          VOID_PTR
end repeat
  MaxParams64           VOID_PTR
  MaxParamsMixed        VOID_PTR
ends

struct EFI_COMPONENT_NAME_PROTOCOL
  GetDriverName         VOID_PTR
  GetControllerName     VOID_PTR
  SupportedLanguages    VOID_PTR
ends

struct EFI_DRIVER_BINDING_PROTOCOL
  Supported             VOID_PTR
  Start                 VOID_PTR
  Stop                  VOID_PTR
  Version               UINT32
  ImageHandle           EFI_HANDLE
  DriverBindingHandle   EFI_HANDLE
ends

format peebc efiboot
entry DriverInstall

section '.text' code executable readable

; Return a 32 or 64-bit status code for the native arch
ReturnStatus:
  CMP64gte  R7, R6
  JMPcs     @0f
  MOVId     R4, EFI_32BIT_ERROR
  MOVsnd    R5, R4
  CMPlte    R5, R6
  JMPcc     @0f
  OR32      R7, R4
@0:
  RET

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
  PUSH      R4
  MOV       R3, R6
  NOT       R4, R6
  MOVREL    R7, HexStr64
  PUSH      R7
  MOV       R1, @R0(0,+32)
  JMP       PrintHexCommon
PrintHex32:
  PUSH      R4
  MOVI      R3, 8
  NOT32     R4, R6
  MOVREL    R7, HexStr32
  PUSH      R7
  MOV       R1, @R0(0,+32)
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
  ; HexStr32 or HexStr64 is on top of stack
  CALL      Print
  POP       R1
  POP       R4
  RET

CallFailed:
  PUSH      R7
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      PrintHex64
  POP       R7
  JMP       ReturnStatus

GetDriverName:
  MOVn      R1, @R0(+2,+16)
  MOVREL    @R1, DrvName
  MOVI      R7, EFI_SUCCESS
  RET

BindingSupported:
  MOVI      R7, EFI_UNSUPPORTED
  JMP       ReturnStatus

Hello:
  MOVREL    R1, HelloMsg
  PUSH      R1
  CALL      Print
  POP       R1
  MOVI      R7, EFI_SUCCESS
  RET

InvalidParam32:
  MOVREL    R4, PrintHex32
  JMP       InvalidParamCommon
InvalidParam64:
  MOVREL    R4, PrintHex64
InvalidParamCommon:
  MOVREL    R1, IPMsg1
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      R4
  POP       R1
  MOVREL    R1, IPMsg2
  PUSH      R1
  CALL      Print
  POP       R1
  CALL      R4
  POP       R1
  MOVI      R7, EFI_INVALID_PARAMETER
  JMP       ReturnStatus

repeat 16 i:0
MultiParam#i:
  MOVI      R5, i
  JMP       MultiParamCommon
end repeat

MultiParamCommon:
  XOR       R6, R6
  MOVI      R4, 1
  MOVREL    R3, Values
  MOV       R2, R0(+0,+16)
@0:
  PUSH      R5
  AND       R5, R4
  CMPeq     R5, R6
  POP       R5
  JMPcc     @1f ; UINTN or UINT64?
  MOVn      R1, @R3
  CMP32eq   R1, @R2
  JMPcs     @3f
  PUSH      @R2
  PUSH      R1
  JMP       InvalidParam32
@3:
  MOV       R2, R2(+1,+0)
  JMP       @2f
@1:
  MOVqq     R1, @R3
  CMP64eq   R1, @R2
  JMPcs     @3f
  PUSH      @R2
  PUSH      R1
  JMP       InvalidParam64
@3:
  MOV       R2, R2(+0,+8)
@2:
  ADD       R3, R6(8)
  SHL       R4, R6(1)
  CMPIeq    R4, 0x10
  JMPcc     @0b
  MOVI      R7, EFI_SUCCESS
  RET

MaxParams64:
  MOVI      R1, 0x0B0B0B0B0A0A0A0A
  MOVI      R2, 0x1010101010101010
  MOV       R4, R6
  MOV       R5, R0(+0,+16)
@0:
  CMP64eq   R1, @R5
  JMPcs     @1f
  PUSH      @R5
  PUSH      R1
  JMP       InvalidParam64
@1:
  MOV       R5, R5(+0,+8)
  ADD64     R1, R2
  ADD       R4, R6(1)
  CMPlte    R4, R6(15)
  JMPcs     @0b
  MOVI      R7, EFI_SUCCESS
  RET

MaxParamsMixed:
  MOVI      R1, 0x0A0A0A0A
  MOVI      R2, 0x1B1B1B1B1A1A1A1A
  MOVI      R3, 0x2020202020202020
  MOV       R4, R6
  MOV       R5, R0(+0,+16)
@0:
  CMP32eq   R1, @R5
  JMPcs     @1f
  PUSH      @R5
  PUSH      R1
  JMP       InvalidParam32
@1:
  MOV       R5, R5(+1,+0)
  ADD32     R1, R3
  CMP64eq   R2, @R5
  JMPcs     @1f
  PUSH      @R5
  PUSH      R2
  JMP       InvalidParam64
@1:
  MOV       R5, R5(+0,+8)
  ADD64     R2, R3
  ADD       R4, R6(1)
  CMPlte    R4, R6(7)
  JMPcs     @0b
  MOVI      R7, EFI_SUCCESS
  RET

DriverInstall:
  XOR       R6, R6
  MOVREL    R1, ImageHandle
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.ImageHandle)
  MOVREL    R1, gST
  MOVn      @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

  ; Check for an already running driver
  MOVREL    R1, CustomProtocolInterface
  PUSHn     R1
  PUSHn     R6
  MOVREL    R1, CustomProtocolGuid
  PUSHn     R1
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.LocateProtocol)
  MOV       R0, R0(+3,+0)
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     @0f

  MOVREL    R1, AIMsg
  PUSHn     R1
  CALL      Print
  MOV       R0, R0(+1,+0)
  MOVI      R7, EFI_LOAD_ERROR
  RET

@0:
  ; The only valid status we expect is NOT FOUND here
  MOVIq     R1, EFI_NOT_FOUND
  CMP64eq   R1, R7
  JMPcs     @0f
  MOVId     R1, EFI_NOT_FOUND or EFI_32BIT_ERROR
  CMP32eq   R1, R7
  JMPcs     @0f

  MOVREL    R1, USMsg
  PUSHn     R1
  CALL      Print
  MOV       R0, R0(+1,+0)
  RET

@0:
  ; Fill in the Custom Protocol interface
  MOVREL    R1, CustomProtocolInterface
  EXPORT    Hello ; Generate a native -> EBC thunk (in @R7)
  MOVn      @R1(EFI_CUSTOM_PROTOCOL.Hello), @R7
repeat 16 i:0
  EXPORT    MultiParam#i
  MOVn      @R1(EFI_CUSTOM_PROTOCOL.MultiParam#i), @R7
end repeat
  EXPORT    MaxParamsMixed
  MOVn      @R1(EFI_CUSTOM_PROTOCOL.MaxParamsMixed), @R7
  EXPORT    MaxParams64
  MOVn      @R1(EFI_CUSTOM_PROTOCOL.MaxParams64), @R7

  ; Fill in the Component Name interfaces
  MOVREL    R1, ComponentName
  EXPORT    GetDriverName
  MOVn      @R1(EFI_COMPONENT_NAME_PROTOCOL.GetDriverName), @R7
  MOVn      @R1(EFI_COMPONENT_NAME_PROTOCOL.GetControllerName), R6
  MOVREL    R3, Eng
  MOVn      @R1(EFI_COMPONENT_NAME_PROTOCOL.SupportedLanguages), R3
  MOVREL    R1, ComponentName2
  MOVn      @R1(EFI_COMPONENT_NAME_PROTOCOL.GetDriverName), @R7
  MOVn      @R1(EFI_COMPONENT_NAME_PROTOCOL.GetControllerName), R6
  MOVREL    R3, En
  MOVn      @R1(EFI_COMPONENT_NAME_PROTOCOL.SupportedLanguages), R3

  ; Fill in DriverBinding
  MOVREL    R1, DriverBinding
  EXPORT    BindingSupported
  MOVn      @R1(EFI_DRIVER_BINDING_PROTOCOL.Supported), @R7
  MOVn      @R1(EFI_DRIVER_BINDING_PROTOCOL.Start), R6
  MOVn      @R1(EFI_DRIVER_BINDING_PROTOCOL.Stop), R6
  MOVI      R2, DriverVersion
  MOVd      @R1(EFI_DRIVER_BINDING_PROTOCOL.Version), R2
  MOVREL    R2, ImageHandle
  MOVn      @R1(EFI_DRIVER_BINDING_PROTOCOL.ImageHandle), @R2
  MOVn      @R1(EFI_DRIVER_BINDING_PROTOCOL.DriverBindingHandle), @R2

  ; Install the interface
  MOVREL    R1, CustomProtocolInterface
  PUSHn     R1
  PUSHn     R6 ; EFI_NATIVE_INTERFACE = 0
  MOVREL    R1, CustomProtocolGuid
  PUSHn     R1
  MOVREL    R1, CustomProtocolHandle
  PUSHn     R1
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.InstallProtocolInterface)
  MOV       R0, R0(+4,+0)
  MOVREL    R1, IPIMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  ; Grab a handle to this image, so that we can add an unload to our driver
  MOVI      R1, EFI_OPEN_PROTOCOL_GET_PROTOCOL
  PUSHn     R1
  PUSHn     R6
  MOVREL    R2, ImageHandle
  PUSHn     @R2
  MOVREL    R1, LoadedImage
  PUSHn     R1
  MOVREL    R1, gEfiLoadedImageProtocolGuid
  PUSHn     R1
  PUSHn     @R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.OpenProtocol)
  MOV       R0, R0(+6,+0)
  MOVREL    R1, OPMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  ; Install driver
  PUSHn     R6
  MOVREL    R2, ComponentName
  PUSHn     R2
  MOVREL    R1, gEfiComponentName2ProtocolGuid
  PUSHn     R1
  PUSHn     R2
  MOVREL    R1, gEfiComponentNameProtocolGuid
  PUSHn     R1
  MOVREL    R2, DriverBinding
  PUSHn     R2
  MOVREL    R1, gEfiDriverBindingProtocolGuid
  PUSHn     R1
  MOV       R2, R2(EFI_DRIVER_BINDING_PROTOCOL.DriverBindingHandle)
  PUSHn     R2
  MOVREL    R1, gST
  MOV       R1, @R1
  MOV       R1, @R1(EFI_SYSTEM_TABLE.BootServices)
  CALLEX    @R1(EFI_BOOT_SERVICES.InstallMultipleProtocolInterfaces)
  MOV       R0, R0(+8,+0)
  MOVREL    R1, IMPIMsg
  CMPI32eq  R7, EFI_SUCCESS
  JMPcc     CallFailed

  MOVI      R7, EFI_SUCCESS
  RET

section '.exports' data readable writeable
  ; Any function that may be called from native must be declared with the EXPORT keyword
  ; in a data section, and provide a C-like call signature in parenthesis.
  ; Then, during application init, EXPORT should be invoked, with the same function name
  ; (but without the signature) so that native -> EBC thunks are created through BREAK 5.
  EXPORT    Hello(VOID)
repeat 16 i:0
  ; Custom export declaration for the MultiParam## calls
  _MultiParam#i:
            dd MultiParam#i - $ - 4
            dd EBC_CALL_SIGNATURE or i
end repeat
  EXPORT    MaxParams64(UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64)
  EXPORT    MaxParamsMixed(UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64)
  EXPORT    BindingSupported(VOID*, VOID*, VOID*)
  EXPORT    GetDriverName(VOID*, CHAR8*, CHAR16*)

section '.data' data readable writeable
  gST:      dq ?
  LoadedImage:
            dq ?
  CustomProtocolHandle:
            dq ?
  ImageHandle:
            dq ?
  Values:
            dq 0x1B1B1B1B1A1A1A1A, 0x2B2B2B2B2A2A2A2A, 0x3B3B3B3B3A3A3A3A, 0x4B4B4B4B4A4A4A4A
  CustomProtocolInterface:
            rb EFI_CUSTOM_PROTOCOL.__size
  ComponentName:
            rb EFI_COMPONENT_NAME_PROTOCOL.__size
  ComponentName2:
            rb EFI_COMPONENT_NAME_PROTOCOL.__size
  DriverBinding:
            rb EFI_DRIVER_BINDING_PROTOCOL.__size
  gEfiLoadedImageProtocolGuid:
            EFI_GUID { 0x5b1b31a1, 0x9562, 0x11d2, { 0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
  gEfiDriverBindingProtocolGuid:
            EFI_GUID { 0x18a031ab, 0xb443, 0x4d1a, { 0xa5, 0xc0, 0x0c, 0x09, 0x26, 0x1e, 0x9f, 0x71 } }
  gEfiComponentNameProtocolGuid:
            EFI_GUID { 0x107a772c, 0xd5e1, 0x11d4, { 0x9a, 0x46, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }
  gEfiComponentName2ProtocolGuid:
            EFI_GUID { 0x6a7a5cff, 0xe8d9, 0x4f70, { 0xba, 0xda, 0x75, 0xab, 0x30, 0x25, 0xce, 0x14 } }
  CustomProtocolGuid:
            EFI_GUID { 0x230aa93e, 0x3d8a, 0x4bbd, { 0x8d, 0x48, 0x77, 0xd3, 0xed, 0x9c, 0xa7, 0x9b } }
  Digits:   du "0123456789ABCDEF"
  HexStr32: du "0x12345678", 0x0D, 0x0A, 0x00
  HexStr64: du "0x1234567812345678", 0x0D, 0x0A, 0x00
  AIMsg:    du "This driver has already been installed", 0x0D, 0x0A, 0x00
  USMsg:    du "Unexpected initial status", 0x0D, 0x0A, 0x00
  OPMsg:    du "Error OpenProtocol: ", 0x00
  IPIMsg:   du "Error InstallProtocolInterface: ", 0x00
  IMPIMsg:  du "Error InstallMultipleProtocolInterfaces: ", 0x00
  HelloMsg: du "Hello from EBC driver", 0x0D, 0x0A, 0x00
  DrvName:  du "EBC Driver v1.1", 0x00
  En:       db "en", 0x00
  Eng:      db "eng", 0x00
  IPMsg1:   du "Expected: ", 0x00
  IPMsg2:   du "Received: ", 0x00

section '.reloc' fixups data discardable
