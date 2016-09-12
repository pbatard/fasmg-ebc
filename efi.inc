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

;
; Symbols
;

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

EfiResetCold                    = 0
EfiResetWarm                    = 1
EfiResetShutdown                = 2
