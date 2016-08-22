;
; EBC (EFI Byte Code) assembly 'Hello World'
; Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
;

include 'ebc.inc'
include 'efi.inc'
include 'format.inc'

format peebc dll efi
entry efi_main

section '.text' code executable readable
efi_main:
	MOVIqq R7, EFI_NOT_READY
	RET

section '.data' data readable writeable

section '.reloc' fixups data discardable
