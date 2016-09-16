;
; Copyright © 2016 Pete Batard <pete@akeo.ie>
; Copyright © 2016 Tomasz Grysztar <tgrysztar@niewidoczna.pl>
;

element S

element BOOLEAN?    : S + 1
element INT8?       : S + 1
element UINT8?      : S + 1
element INT16?      : S + 2
element UINT16?     : S + 2
element INT32?      : S + 4
element UINT32?     : S + 4
element INT64?      : S + 8
element UINT64?     : S + 8
element INTN?       : S - 8 ; negative => natural value
element UINTN?      : S - 8
element VOID_PTR?   : S - 8
element EFI_STATUS? : S - 8
element EFI_HANDLE? : S - 8

; Helper macros for automated index and alignment computation
; (e.g. UINT32 followed by UINT64 results in (+1,+0) rather than (+0,+4)).

; Returns the base element size for constructs like 'rq 4' or 'dw ?'
macro get_element_size size, def&
  local n
  match element =?, def
    n = 1
  else match rt val, def
    n = val
  else
    err 'unsupported declaration'
  end match
  virtual at 0
    def
    size = $ / n
  end virtual
end macro

macro struct? name
  define name 0
  namespace name
  virtual at 0
  name.n = 0 ; current number of natural elements
  name.s = 0 ; current struct size

  struc ? def&
    local n, c, s, i, as_is
    as_is = 0
    i = 0
    match a b, def
      as_is = 1
      get_element_size s, def
    else
      if def metadata 1 relativeto S
        s = def metadata 1 - S
      else
        ; we assert that any reference to a substructure is aligned to natural word
        ; so that we can just add its existing n and c to our computed values
        if ($ mod 8 <> 0)
          err 'substructure must be aligned to natural word'
        end if
        s = def.s
        i = i + def.n
      end if
      if (s < 0)
        i = 1
        s = -s
      end if
    end match
    if (s = 8) & ((($+3) and (not 3)) <> (($+7) and (not 7)))
      name.n = name.n + 1 ; alignment correction required
    end if
    align s
    c = $ - (8 * name.n)
    n = name.n
    assert (n >= 0)
    assert (c >= 0)
    . equ +n,+c
    if (as_is)
      def
    else
      rb s
    end if
    name.n = name.n + i
    name.s = $
  end struc
end macro

macro ends?
  restruc ?
  end virtual
  end namespace
end macro

; Data definitions, from UEFI specs

; EfiMain() is handled like any other EBC CALL routine, therefore
; we need to skip c=+16 bytes to access our parameters, due to the
; following stack manipulation operations having been carried out:
;   R0 = R0 - 8           ; -> +8
;   PUSH64 ReturnAddress  ; -> +8
; Knowing this, the setup for the two EfiMain() parameters are
; 'ImageHandle' at (+0,+16) and 'SystemTable' at (+1,+16).
struct EFI_MAIN_PARAMETERS
  Reserved                     rq 2
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
  FirmwareRevision             UINT32
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

struct EFI_BOOT_SERVICES
  Hdr                          EFI_TABLE_HEADER
  RaiseTPL                     VOID_PTR
  RestoreTPL                   VOID_PTR
  AllocatePages                VOID_PTR
  FreePages                    VOID_PTR
  GetMemoryMap                 VOID_PTR
  AllocatePool                 VOID_PTR
  FreePool                     VOID_PTR
  CreateEvent                  VOID_PTR
  SetTimer                     VOID_PTR
  WaitForEvent                 VOID_PTR
  SignalEvent                  VOID_PTR
  CloseEvent                   VOID_PTR
  CheckEvent                   VOID_PTR
  InstallProtocolInterface     VOID_PTR
  ReinstallProtocolInterface   VOID_PTR
  UninstallProtocolInterface   VOID_PTR
  HandleProtocol               VOID_PTR
  PCHandleProtocol             VOID_PTR
  RegisterProtocolNotify       VOID_PTR
  LocateHandle                 VOID_PTR
  LocateDevicePath             VOID_PTR
  InstallConfigurationTable    VOID_PTR
  LoadImage                    VOID_PTR
  StartImage                   VOID_PTR
  Exit                         VOID_PTR
  UnloadImage                  VOID_PTR
  ExitBootServices             VOID_PTR
  GetNextMonotonicCount        VOID_PTR
  Stall                        VOID_PTR
  SetWatchdogTimer             VOID_PTR
  ConnectController            VOID_PTR
  DisconnectController         VOID_PTR
  OpenProtocol                 VOID_PTR
  CloseProtocol                VOID_PTR
  OpenProtocolInformation      VOID_PTR
  ProtocolsPerHandle           VOID_PTR
  LocateHandleBuffer           VOID_PTR
  LocateProtocol               VOID_PTR
  InstallMultipleProtocolInterfaces    VOID_PTR
  UninstallMultipleProtocolInterfaces  VOID_PTR
  CalculateCrc32               VOID_PTR
  CopyMem                      VOID_PTR
  SetMem                       VOID_PTR
  CreateEventEx                VOID_PTR
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
