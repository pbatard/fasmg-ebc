;
; Modified from http://x86asm.net/articles/uefi-programming-first-steps/
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

struc int8
  . db ?
end struc

struc int16
  align 2
  . dw ?
end struc

struc int32
  align 4
  . dd ?
end struc

struc int64
  align 8
  . dq ?
end struc

struc intn
  align 8
  . dq ?
end struc

struc dptr
  align 8
  . dq ?
end struc

;symbols

EFIERR                          = 0x8000000000000000
EFI_SUCCESS                     = 0
EFI_LOAD_ERROR                  = EFIERR or 1
EFI_INVALID_PARAMETER           = EFIERR or 2
EFI_UNSUPPORTED                 = EFIERR or 3
EFI_BAD_BUFFER_SIZE             = EFIERR or 4
EFI_BUFFER_TOO_SMALL            = EFIERR or 5
EFI_NOT_READY                   = EFIERR or 6
EFI_DEVICE_ERROR                = EFIERR or 7
EFI_WRITE_PROTECTED             = EFIERR or 8
EFI_OUT_OF_RESOURCES            = EFIERR or 9
EFI_VOLUME_CORRUPTED            = EFIERR or 10
EFI_VOLUME_FULL                 = EFIERR or 11
EFI_NO_MEDIA                    = EFIERR or 12
EFI_MEDIA_CHANGED               = EFIERR or 13
EFI_NOT_FOUND                   = EFIERR or 14
EFI_ACCESS_DENIED               = EFIERR or 15
EFI_NO_RESPONSE                 = EFIERR or 16
EFI_NO_MAPPING                  = EFIERR or 17
EFI_TIMEOUT                     = EFIERR or 18
EFI_NOT_STARTED                 = EFIERR or 19
EFI_ALREADY_STARTED             = EFIERR or 20
EFI_ABORTED                     = EFIERR or 21
EFI_ICMP_ERROR                  = EFIERR or 22
EFI_TFTP_ERROR                  = EFIERR or 23
EFI_PROTOCOL_ERROR              = EFIERR or 24

EfiResetCold                    = 0
EfiResetWarm                    = 1
EfiResetShutdown                = 2

; Helper macro for definition of relative structure member offsets

macro struct name
  virtual at 0
    name name
  end virtual
end macro

; Structures

struc EFI_TABLE_HEADER
  .Signature                    int64
  .Revision                     int32
  .HeaderSize                   int32
  .CRC32                        int32
  .Reserved                     int32
end struc
struct EFI_TABLE_HEADER

struc EFI_SYSTEM_TABLE
  .Hdr                          EFI_TABLE_HEADER
  .FirmwareVendor               dptr
  .FirmwareRevision             int32
  .ConsoleInHandle              dptr
  .ConIn                        dptr
  .ConsoleOutHandle             dptr
  .ConOut                       dptr
  .StandardErrorHandle          dptr
  .StdErr                       dptr
  .RuntimeServices              dptr
  .BootServices	                dptr
  .NumberOfTableEntries         intn
  .ConfigurationTable           dptr
end struc
struct EFI_SYSTEM_TABLE

struc EFI_RUNTIME_SERVICES
  .Hdr                          EFI_TABLE_HEADER
  .GetTime                      dptr
  .SetTime                      dptr
  .GetWakeupTime                dptr
  .SetWakeupTime                dptr
  .SetVirtualAddressMap         dptr
  .ConvertPointer               dptr
  .GetVariable                  dptr
  .GetNextVariableName          dptr
  .SetVariable                  dptr
  .GetNextHighMonotonicCount    dptr
  .ResetSystem                  dptr
  .UpdateCapsule                dptr
  .QueryCapsuleCapabilities     dptr
  .QueryVariableInfo            dptr
end struc
struct EFI_RUNTIME_SERVICES

struc SIMPLE_TEXT_OUTPUT_INTERFACE
  .Reset                        dptr
  .OutputString                 dptr
  .TestString                   dptr
  .QueryMode                    dptr
  .SetMode                      dptr
  .SetAttribute                 dptr
  .ClearScreen                  dptr
  .SetCursorPosition            dptr
  .EnableCursor                 dptr
  .Mode                         dptr
end struc
struct SIMPLE_TEXT_OUTPUT_INTERFACE

struc SIMPLE_TEXT_INPUT_INTERFACE
  .Reset                        dptr
  .ReadKeyStroke                dptr
  .WaitForKey                   dptr
end struc
struct SIMPLE_TEXT_INPUT_INTERFACE

struc EFI_INPUT_KEY
  .ScanCode                     int16
  .UnicodeChar                  int16
end struc
struct EFI_INPUT_KEY
