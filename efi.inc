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

FALSE                         = 0
TRUE                          = 1

EFI_SUCCESS                   = 0x0000000000000000
EFI_ERROR                     = 0x8000000000000000
EFI_32BIT_ERROR               = 0x80000000
EFI_32BIT_MASK                = 0xFFFFFFFF
EFI_LOAD_ERROR                = EFI_ERROR or 1
EFI_INVALID_PARAMETER         = EFI_ERROR or 2
EFI_UNSUPPORTED               = EFI_ERROR or 3
EFI_BAD_BUFFER_SIZE           = EFI_ERROR or 4
EFI_BUFFER_TOO_SMALL          = EFI_ERROR or 5
EFI_NOT_READY                 = EFI_ERROR or 6
EFI_DEVICE_ERROR              = EFI_ERROR or 7
EFI_WRITE_PROTECTED           = EFI_ERROR or 8
EFI_OUT_OF_RESOURCES          = EFI_ERROR or 9
EFI_VOLUME_CORRUPTED          = EFI_ERROR or 10
EFI_VOLUME_FULL               = EFI_ERROR or 11
EFI_NO_MEDIA                  = EFI_ERROR or 12
EFI_MEDIA_CHANGED             = EFI_ERROR or 13
EFI_NOT_FOUND                 = EFI_ERROR or 14
EFI_ACCESS_DENIED             = EFI_ERROR or 15
EFI_NO_RESPONSE               = EFI_ERROR or 16
EFI_NO_MAPPING                = EFI_ERROR or 17
EFI_TIMEOUT                   = EFI_ERROR or 18
EFI_NOT_STARTED               = EFI_ERROR or 19
EFI_ALREADY_STARTED           = EFI_ERROR or 20
EFI_ABORTED                   = EFI_ERROR or 21
EFI_ICMP_ERROR                = EFI_ERROR or 22
EFI_TFTP_ERROR                = EFI_ERROR or 23
EFI_PROTOCOL_ERROR            = EFI_ERROR or 24

EfiResetCold                  = 0
EfiResetWarm                  = 1
EfiResetShutdown              = 2
