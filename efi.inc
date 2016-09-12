;
; Copyright © 2016 Pete Batard <pete@akeo.ie>
; Copyright © 2016 Tomasz Grysztar <tgrysztar@niewidoczna.pl>
;

; Helper macros for automated index computation

macro virtual? setup
  macro org? address
    local a
    a = address
    end virtual
    virtual at a
  end macro
  virtual setup
end macro

macro end?.virtual?
  end virtual
  purge org?
end macro

macro struct? name
  macro ends?!
      end namespace
    end struc
    virtual at 0
      name name
      sizeof.name = $
    end virtual
    purge ends?
  end macro
  struc name
    label .
    namespace .
end macro

element natural_word?

macro dn? value
  match ?, value
    org $ + natural_word
  else
    err 'invalid value'
  end match
end macro

macro rn? count
  org $ + natural_word * count
end macro

struc dn? value
  .: dn value
end struc

struc rn? count
  .: rn count
end struc

; Data definitions, from UEFI specs

struct BOOLEAN
  db ?
ends
struct INT8
  db ?
ends
struct UINT8
  db ?
ends
struct INT16
  dw ?
ends
struct UINT16
  dw ?
ends
struct INT32
  dd ?
ends
struct UINT32
  dd ?
ends
struct INT64
  dq ?
ends
struct UINT64
  dq ?
ends
struct INTN
  dn ?
ends
struct UINTN
  dn ?
ends
struct CHAR8
  db ?
ends
struct CHAR16
  dw ?
ends
struct VOID_PTR
  dn ?
ends
struct EFI_STATUS
  dn ?
ends
struct EFI_HANDLE
  dn ?
ends


; EfiMain() is handled like any other EBC CALL routine, therefore
; we need to skip c=+16 bytes to access our parameters, due to the
; following stack manipulation operations having been carried out:
;   R0 = R0 - 8           ; -> +8
;   PUSH64 ReturnAddress  ; -> +8
; Knowing this, the setup for the two EfiMain() parameters are
; 'ImageHandle' at (+0,+16) and 'SystemTable' at (+1,+16).
struct EFI_MAIN_PARAMETERS
  Reserved                     rb 16
  ImageHandle                  VOID_PTR
  SystemTable                  VOID_PTR
ends

struct EFI_TABLE_HEADER
  Signature                    UINT64
  Revision                     UINT32
  HeaderSize                   UINT32
  CRC32                        UINT32
  Reserved                     UINT32
ends

struct EFI_SYSTEM_TABLE
  Hdr                          EFI_TABLE_HEADER
  FirmwareVendor               VOID_PTR
  ; "Fun" rule of EBC structure index computations (as per UEFI Specs 2.6
  ; Chapter 2.3.1)  "Internal (structure) data are implicitly padded to
  ; achieve natural alignment."
  ; This means that, 'FirmwareRevision' below, which is really an UINT32,
  ; must be handled as a natural member when computed as an index value,
  ; since we may have UINT32 + 4 bytes padding (= 64 bits) on x64, or
  ; UINT32 + no padding (= 32 bits) on IA32 or ARM. 
  ; Since our index computations are not that smart, we cheat by inserting
  ; it as a straight UINTN...
  FirmwareRevision             UINTN
  ConsoleInHandle              EFI_HANDLE
  ConIn                        VOID_PTR
  ConsoleOutHandle             EFI_HANDLE
  ConOut                       VOID_PTR
  StandardErrorHandle          EFI_HANDLE
  StdErr                       VOID_PTR
  RuntimeServices              VOID_PTR
  BootServices	               VOID_PTR
  NumberOfTableEntries         UINTN
  ConfigurationTable           VOID_PTR
ends

struct EFI_RUNTIME_SERVICES
  Hdr                          EFI_TABLE_HEADER
  GetTime                      VOID_PTR
  SetTime                      VOID_PTR
  GetWakeupTime                VOID_PTR
  SetWakeupTime                VOID_PTR
  SetVirtualAddressMap         VOID_PTR
  ConvertPointer               VOID_PTR
  GetVariable                  VOID_PTR
  GetNextVariableName          VOID_PTR
  SetVariable                  VOID_PTR
  GetNextHighMonotonicCount    VOID_PTR
  ResetSystem                  VOID_PTR
  UpdateCapsule                VOID_PTR
  QueryCapsuleCapabilities     VOID_PTR
  QueryVariableInfo            VOID_PTR
ends

struct SIMPLE_TEXT_OUTPUT_INTERFACE
  Reset                        VOID_PTR
  OutputString                 VOID_PTR
  TestString                   VOID_PTR
  QueryMode                    VOID_PTR
  SetMode                      VOID_PTR
  SetAttribute                 VOID_PTR
  ClearScreen                  VOID_PTR
  SetCursorPosition            VOID_PTR
  EnableCursor                 VOID_PTR
  Mode                         VOID_PTR
ends

struct SIMPLE_TEXT_INPUT_INTERFACE
  Reset                        VOID_PTR
  ReadKeyStroke                VOID_PTR
  WaitForKey                   VOID_PTR
ends

; Symbols

; 32 and 64 bit version are defined independently as a reminder that, if you
; call an external procedure, you may have to test for both set of return codes.
EFI64_SUCCESS                   = 0x0000000000000000
EFI64_ERROR                     = 0x8000000000000000
EFI64_LOAD_ERROR                = EFI64_ERROR or 1
EFI64_INVALID_PARAMETER         = EFI64_ERROR or 2
EFI64_UNSUPPORTED               = EFI64_ERROR or 3
EFI64_BAD_BUFFER_SIZE           = EFI64_ERROR or 4
EFI64_BUFFER_TOO_SMALL          = EFI64_ERROR or 5
EFI64_NOT_READY                 = EFI64_ERROR or 6
EFI64_DEVICE_ERROR              = EFI64_ERROR or 7
EFI64_WRITE_PROTECTED           = EFI64_ERROR or 8
EFI64_OUT_OF_RESOURCES          = EFI64_ERROR or 9
EFI64_VOLUME_CORRUPTED          = EFI64_ERROR or 10
EFI64_VOLUME_FULL               = EFI64_ERROR or 11
EFI64_NO_MEDIA                  = EFI64_ERROR or 12
EFI64_MEDIA_CHANGED             = EFI64_ERROR or 13
EFI64_NOT_FOUND                 = EFI64_ERROR or 14
EFI64_ACCESS_DENIED             = EFI64_ERROR or 15
EFI64_NO_RESPONSE               = EFI64_ERROR or 16
EFI64_NO_MAPPING                = EFI64_ERROR or 17
EFI64_TIMEOUT                   = EFI64_ERROR or 18
EFI64_NOT_STARTED               = EFI64_ERROR or 19
EFI64_ALREADY_STARTED           = EFI64_ERROR or 20
EFI64_ABORTED                   = EFI64_ERROR or 21
EFI64_ICMP_ERROR                = EFI64_ERROR or 22
EFI64_TFTP_ERROR                = EFI64_ERROR or 23
EFI64_PROTOCOL_ERROR            = EFI64_ERROR or 24

EFI32_SUCCESS                   = 0x00000000
EFI32_ERROR                     = 0x80000000
EFI32_LOAD_ERROR                = EFI32_ERROR or 1
EFI32_INVALID_PARAMETER         = EFI32_ERROR or 2
EFI32_UNSUPPORTED               = EFI32_ERROR or 3
EFI32_BAD_BUFFER_SIZE           = EFI32_ERROR or 4
EFI32_BUFFER_TOO_SMALL          = EFI32_ERROR or 5
EFI32_NOT_READY                 = EFI32_ERROR or 6
EFI32_DEVICE_ERROR              = EFI32_ERROR or 7
EFI32_WRITE_PROTECTED           = EFI32_ERROR or 8
EFI32_OUT_OF_RESOURCES          = EFI32_ERROR or 9
EFI32_VOLUME_CORRUPTED          = EFI32_ERROR or 10
EFI32_VOLUME_FULL               = EFI32_ERROR or 11
EFI32_NO_MEDIA                  = EFI32_ERROR or 12
EFI32_MEDIA_CHANGED             = EFI32_ERROR or 13
EFI32_NOT_FOUND                 = EFI32_ERROR or 14
EFI32_ACCESS_DENIED             = EFI32_ERROR or 15
EFI32_NO_RESPONSE               = EFI32_ERROR or 16
EFI32_NO_MAPPING                = EFI32_ERROR or 17
EFI32_TIMEOUT                   = EFI32_ERROR or 18
EFI32_NOT_STARTED               = EFI32_ERROR or 19
EFI32_ALREADY_STARTED           = EFI32_ERROR or 20
EFI32_ABORTED                   = EFI32_ERROR or 21
EFI32_ICMP_ERROR                = EFI32_ERROR or 22
EFI32_TFTP_ERROR                = EFI32_ERROR or 23
EFI32_PROTOCOL_ERROR            = EFI32_ERROR or 24

FALSE                           = 0
TRUE                            = 1

EfiResetCold                    = 0
EfiResetWarm                    = 1
EfiResetShutdown                = 2
