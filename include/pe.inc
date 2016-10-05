
IMAGE_FILE_MACHINE_UNKNOWN   = 0x0
IMAGE_FILE_MACHINE_AM33      = 0x1D3
IMAGE_FILE_MACHINE_AMD64     = 0x8664
IMAGE_FILE_MACHINE_ARM	     = 0x1C0
IMAGE_FILE_MACHINE_ARMNT     = 0x1C4
IMAGE_FILE_MACHINE_ARM64     = 0xAA64
IMAGE_FILE_MACHINE_EBC	     = 0xEBC
IMAGE_FILE_MACHINE_I386      = 0x14C
IMAGE_FILE_MACHINE_IA64      = 0x200
IMAGE_FILE_MACHINE_M32R      = 0x9041
IMAGE_FILE_MACHINE_MIPS16    = 0x266
IMAGE_FILE_MACHINE_MIPSFPU   = 0x366
IMAGE_FILE_MACHINE_MIPSFPU16 = 0x466
IMAGE_FILE_MACHINE_POWERPC   = 0x1F0
IMAGE_FILE_MACHINE_POWERPCFP = 0x1F1
IMAGE_FILE_MACHINE_R4000     = 0x166
IMAGE_FILE_MACHINE_SH3	     = 0x1A2
IMAGE_FILE_MACHINE_SH3DSP    = 0x1A3
IMAGE_FILE_MACHINE_SH4	     = 0x1A6
IMAGE_FILE_MACHINE_SH5	     = 0x1A8
IMAGE_FILE_MACHINE_THUMB     = 0x1C2
IMAGE_FILE_MACHINE_WCEMIPSV2 = 0x169

IMAGE_FILE_RELOCS_STRIPPED	    = 0x0001
IMAGE_FILE_EXECUTABLE_IMAGE	    = 0x0002
IMAGE_FILE_LINE_NUMS_STRIPPED	    = 0x0004
IMAGE_FILE_LOCAL_SYMS_STRIPPED	    = 0x0008
IMAGE_FILE_AGGRESSIVE_WS_TRIM	    = 0x0010
IMAGE_FILE_LARGE_ADDRESS_AWARE	    = 0x0020
IMAGE_FILE_BYTES_REVERSED_LO	    = 0x0080
IMAGE_FILE_32BIT_MACHINE	    = 0x0100
IMAGE_FILE_DEBUG_STRIPPED	    = 0x0200
IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP  = 0x0400
IMAGE_FILE_NET_RUN_FROM_SWAP	    = 0x0800
IMAGE_FILE_SYSTEM		    = 0x1000
IMAGE_FILE_DLL			    = 0x2000
IMAGE_FILE_UP_SYSTEM_ONLY	    = 0x4000
IMAGE_FILE_BYTES_REVERSED_HI	    = 0x8000

IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE		= 0x0040
IMAGE_DLL_CHARACTERISTICS_FORCE_INTEGRITY	= 0x0080
IMAGE_DLL_CHARACTERISTICS_NX_COMPAT		= 0x0100
IMAGE_DLL_CHARACTERISTICS_NO_ISOLATION		= 0x0200
IMAGE_DLL_CHARACTERISTICS_NO_SEH		= 0x0400
IMAGE_DLL_CHARACTERISTICS_NO_BIND		= 0x0800
IMAGE_DLL_CHARACTERISTICS_WDM_DRIVER		= 0x2000
IMAGE_DLL_CHARACTERISTICS_TERMINAL_SERVER_AWARE = 0x8000

IMAGE_SUBSYSTEM_UNKNOWN 		= 0
IMAGE_SUBSYSTEM_NATIVE			= 1
IMAGE_SUBSYSTEM_WINDOWS_GUI		= 2
IMAGE_SUBSYSTEM_WINDOWS_CUI		= 3
IMAGE_SUBSYSTEM_POSIX_CUI		= 7
IMAGE_SUBSYSTEM_WINDOWS_CE_GUI		= 9
IMAGE_SUBSYSTEM_EFI_APPLICATION 	= 10
IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER = 11
IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER	= 12
IMAGE_SUBSYSTEM_EFI_ROM 		= 13
IMAGE_SUBSYSTEM_XBOX			= 14

IMAGE_SCN_TYPE_NO_PAD		 = 0x00000008
IMAGE_SCN_CNT_CODE		 = 0x00000020
IMAGE_SCN_CNT_INITIALIZED_DATA	 = 0x00000040
IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080
IMAGE_SCN_LNK_OTHER		 = 0x00000100
IMAGE_SCN_LNK_INFO		 = 0x00000200
IMAGE_SCN_LNK_REMOVE		 = 0x00000800
IMAGE_SCN_LNK_COMDAT		 = 0x00001000
IMAGE_SCN_GPREL 		 = 0x00008000
IMAGE_SCN_MEM_PURGEABLE 	 = 0x00020000
IMAGE_SCN_MEM_16BIT		 = 0x00020000
IMAGE_SCN_MEM_LOCKED		 = 0x00040000
IMAGE_SCN_MEM_PRELOAD		 = 0x00080000
IMAGE_SCN_ALIGN_1BYTES		 = 0x00100000
IMAGE_SCN_ALIGN_2BYTES		 = 0x00200000
IMAGE_SCN_ALIGN_4BYTES		 = 0x00300000
IMAGE_SCN_ALIGN_8BYTES		 = 0x00400000
IMAGE_SCN_ALIGN_16BYTES 	 = 0x00500000
IMAGE_SCN_ALIGN_32BYTES 	 = 0x00600000
IMAGE_SCN_ALIGN_64BYTES 	 = 0x00700000
IMAGE_SCN_ALIGN_128BYTES	 = 0x00800000
IMAGE_SCN_ALIGN_256BYTES	 = 0x00900000
IMAGE_SCN_ALIGN_512BYTES	 = 0x00A00000
IMAGE_SCN_ALIGN_1024BYTES	 = 0x00B00000
IMAGE_SCN_ALIGN_2048BYTES	 = 0x00C00000
IMAGE_SCN_ALIGN_4096BYTES	 = 0x00D00000
IMAGE_SCN_ALIGN_8192BYTES	 = 0x00E00000
IMAGE_SCN_LNK_NRELOC_OVFL	 = 0x01000000
IMAGE_SCN_MEM_DISCARDABLE	 = 0x02000000
IMAGE_SCN_MEM_NOT_CACHED	 = 0x04000000
IMAGE_SCN_MEM_NOT_PAGED 	 = 0x08000000
IMAGE_SCN_MEM_SHARED		 = 0x10000000
IMAGE_SCN_MEM_EXECUTE		 = 0x20000000
IMAGE_SCN_MEM_READ		 = 0x40000000
IMAGE_SCN_MEM_WRITE		 = 0x80000000

IMAGE_REL_BASED_ABSOLUTE = 0
IMAGE_REL_BASED_HIGH	 = 1
IMAGE_REL_BASED_LOW	 = 2
IMAGE_REL_BASED_HIGHLOW  = 3
IMAGE_REL_BASED_HIGHADJ  = 4
IMAGE_REL_BASED_DIR64	 = 10

macro align? boundary,value:?
	if $ relativeto 0 | ( $ relativeto PE.RELOCATION & PE.SECTION_ALIGNMENT mod boundary = 0 )
		db (boundary-1)-(($ scale 0)+boundary-1) mod boundary dup value
	else
		err 'section not aligned enough'
	end if
end macro

PE::

namespace PE

	if defined Settings.Magic
		MAGIC = Settings.Magic
	else
		MAGIC = 0x10B
	end if

	if defined Settings.Machine
	      MACHINE = Settings.Machine
	else
	      MACHINE = IMAGE_FILE_MACHINE_I386
	end if

	if defined Settings.Characteristics
	      CHARACTERISTICS = Settings.Characteristics
	else
	      CHARACTERISTICS = IMAGE_FILE_EXECUTABLE_IMAGE or IMAGE_FILE_32BIT_MACHINE
	end if

	if defined Settings.DllCharacteristics
	      DLL_CHARACTERISTICS = Settings.DllCharacteristics
	else
	      DLL_CHARACTERISTICS = 0
	end if

	if defined Settings.Subsystem
	      SUBSYSTEM = Settings.Subsystem
	else
	      SUBSYSTEM = IMAGE_SUBSYSTEM_WINDOWS_CUI
	end if

	if defined Settings.MajorSubsystemVersion
	      MAJOR_SUBSYSTEM_VERSION = Settings.MajorSubsystemVersion
	else
	      MAJOR_SUBSYSTEM_VERSION = 3
	end if

	if defined Settings.MinorSubsystemVersion
	      MINOR_SUBSYSTEM_VERSION = Settings.MinorSubsystemVersion
	else
	      MINOR_SUBSYSTEM_VERSION = 10
	end if

	if defined Fixups
	      element RELOCATION
	else
	      RELOCATION := 0
	      CHARACTERISTICS = CHARACTERISTICS or IMAGE_FILE_RELOCS_STRIPPED
	end if

	if defined Settings.ImageBase
	      IMAGE_BASE := RELOCATION + Settings.ImageBase
	else
	      IMAGE_BASE := RELOCATION + 400000h
	end if

	if defined Settings.SectionAlignment
	      SECTION_ALIGNMENT := Settings.SectionAlignment
	else
	      SECTION_ALIGNMENT := 1000h
	end if

	if defined Settings.FileAlignment
	      FILE_ALIGNMENT := Settings.FileAlignment
	else
	      FILE_ALIGNMENT := 512
	end if

	NUMBER_OF_DIRECTORIES := 16

	if defined Settings.Stub

		virtual at 0
			file Settings.Stub
			if $ >= 1Ch
				load SIGNATURE : word from 0
				if SIGNATURE = "MZ" | SIGNATURE = "ZM"
					StubTemplate::
				end if
			end if
		end virtual

		if defined StubTemplate

			load .BYTES_IN_LAST_PAGE : word from StubTemplate:2
			load .NUMBER_OF_PAGES : word from StubTemplate:4
			.TEMPLATE_LENGTH = (.NUMBER_OF_PAGES-1) shl 9 + .BYTES_IN_LAST_PAGE

			load .RELOCATIONS_OFFSET : word from StubTemplate:18h
			if .RELOCATIONS_OFFSET >= 40h
				file Settings.Stub,.TEMPLATE_LENGTH
			else
				load .NUMBER_OF_RELOCATIONS : word from StubTemplate:6
				.RELOCATIONS_LENGTH = .NUMBER_OF_RELOCATIONS shl 2
				load .NUMBER_OF_HEADER_PARAGRAPHS : word from StubTemplate:8
				.TEMPLATE_HEADER_LENGTH = .NUMBER_OF_HEADER_PARAGRAPHS shl 4

				file Settings.Stub,1Ch
				rb 40h - $
				file Settings.Stub:.RELOCATIONS_OFFSET,.RELOCATIONS_LENGTH
				align 16
				.HEADER_LENGTH = $
				file Settings.Stub:.TEMPLATE_HEADER_LENGTH,.TEMPLATE_LENGTH-.TEMPLATE_HEADER_LENGTH
				.LENGTH = $

				store 40h : word at 18h
				store .HEADER_LENGTH shr 4 : word at 8
				store .LENGTH and 1FFh : word at 2
				store (.LENGTH-1) shr 9 + 1 : word at 4
			end if

			store Header : dword at 3Ch

		else

			Stub:
			.signature		       dw "MZ"
			.bytes_in_last_page	       dw .LENGTH and 1FFh
			.number_of_pages	       dw (.LENGTH-1) shr 9 + 1
			.number_of_relocations	       dw 0
			.number_of_header_paragraphs   dw .HEADER_LENGTH shr 4
			.minimum_heap		       dw (10000h - (.LENGTH-.HEADER_LENGTH)) shr 4
			.maximum_heap		       dw 0FFFFh
			.initial_ss		       dw (-100h) shr 4
			.initial_sp		       dw 0FFFEh
			.checksum		       dw 0
			.initial_ip		       dw 100h
			.initial_cs		       dw (-100h) shr 4
			.relocations_offset	       dw 40h
			.overlay_number 	       dw 0
						       rb 3Ch - $
			.new_header_offset	       dd Header

			.HEADER_LENGTH = $

			file Settings.Stub

			.LENGTH = $

		end if

	else

		Stub:
		.signature		       dw "MZ"
		.bytes_in_last_page	       dw .LENGTH and 1FFh
		.number_of_pages	       dw (.LENGTH-1) shr 9 + 1
		.number_of_relocations	       dw 0
		.number_of_header_paragraphs   dw .HEADER_LENGTH shr 4
		.minimum_heap		       dw .STACK_LENGTH shr 4
		.maximum_heap		       dw 0FFFFh
		.initial_ss		       dw 0
		.initial_sp		       dw .LENGTH - .HEADER_LENGTH + .STACK_LENGTH
		.checksum		       dw 0
		.initial_ip		       dw 0
		.initial_cs		       dw 0
		.relocations_offset	       dw 40h
		.overlay_number 	       dw 0
					       rb 3Ch - $
		.new_header_offset	       dd Header

		.HEADER_LENGTH = $
		.STACK_LENGTH = 100h

		namespace Stub

		      include '../8086.inc'

		start:
		      push    cs
		      pop     ds
		      mov     dx,message - start
		      mov     ah,9
		      int     21h
		      mov     ax,4C01h
		      int     21h

		message db 'This program cannot be run in DOS mode.',0Dh,0Ah,24h

		end namespace

		align 16

		.LENGTH = $

	end if

	align 8

	Header:
	.Signature		      dw "PE",0
	.Machine		      dw MACHINE
	.NumberOfSections	      dw NUMBER_OF_SECTIONS
	.TimeDateStamp		      dd %t
	.PointerToSymbolTable	      dd 0
	.NumberOfSymbols	      dd 0
	.SizeOfOptionalHeader	      dw SectionTable - OptionalHeader
	.Characteristics	      dw CHARACTERISTICS

	OptionalHeader:
	.Magic			      dw MAGIC
	.MajorLinkerVersion	      db 2
	.MinorLinkerVersion	      db 0
	.SizeOfCode		      dd 0
	.SizeOfInitializedData	      dd 0
	.SizeOfUninitializedData      dd 0
	.AddressOfEntryPoint	      dd 0
	.BaseOfCode		      dd 0
	if MAGIC <> 0x20B
	 .BaseOfData		      dd 0
	 .ImageBase		      dd IMAGE_BASE - RELOCATION
	else
	 .ImageBase		      dq IMAGE_BASE - RELOCATION
	end if
	.SectionAlignment	      dd SECTION_ALIGNMENT
	.FileAlignment		      dd FILE_ALIGNMENT
	.MajorOperatingSystemVersion  dw 1
	.MinorOperatingSystemVersion  dw 0
	.MajorImageVersion	      dw 0
	.MinorImageVersion	      dw 0
	.MajorSubsystemVersion	      dw MAJOR_SUBSYSTEM_VERSION
	.MinorSubsystemVersion	      dw MINOR_SUBSYSTEM_VERSION
	.Win32VersionValue	      dd 0
	.SizeOfImage		      dd SIZE_OF_IMAGE
	.SizeOfHeaders		      dd SIZE_OF_HEADERS
	.CheckSum		      dd 0
	.Subsystem		      dw SUBSYSTEM
	.DllCharacteristics	      dw DLL_CHARACTERISTICS
	if MAGIC <> 0x20B
	 .SizeOfStackReserve	      dd 1000h
	 .SizeOfStackCommit	      dd 1000h
	 .SizeOfHeapReserve	      dd 10000h
	 .SizeOfHeapCommit	      dd 0
	else
	 .SizeOfStackReserve	      dq 1000h
	 .SizeOfStackCommit	      dq 1000h
	 .SizeOfHeapReserve	      dq 10000h
	 .SizeOfHeapCommit	      dq 0
	end if
	.LoaderFlags		      dd 0
	.NumberOfRvaAndSizes	      dd NUMBER_OF_DIRECTORIES
	RvaAndSizes:
	.Rva			      dd 0
	.Size			      dd 0
	.ENTRY_LENGTH = $ - RvaAndSizes
				      db (NUMBER_OF_DIRECTORIES-1)*RvaAndSizes.ENTRY_LENGTH dup 0
	SectionTable:
	.Name			      dq '.flat'
	.VirtualSize		      dd 0
	.VirtualAddress 	      dd 0
	.SizeOfRawData		      dd 0
	.PointerToRawData	      dd 0
	.PointerToRelocations	      dd 0
	.PointerToLineNumbers	      dd 0
	.NumberOfRelocations	      dw 0
	.NumberOfLineNumbers	      dw 0
	.Characteristics	      dd IMAGE_SCN_MEM_EXECUTE + IMAGE_SCN_MEM_READ + IMAGE_SCN_MEM_WRITE
	.ENTRY_LENGTH = $ - SectionTable
				      db (NUMBER_OF_SECTIONS-1)*SectionTable.ENTRY_LENGTH dup 0

	HeadersEnd:
	define CheckSumBlocks PE,0,HeadersEnd

	SECTION_INDEX = 0
	RELOCATION_INDEX = 0
	DEFINED_SECTION = 0
	SECTION_DIRECTORIES = 0
	align SECTION_ALIGNMENT
	FIRST_SECTION_RVA:
	section $%%
	align FILE_ALIGNMENT,0
	SIZE_OF_HEADERS = $%
	FILE_OFFSET = $%
	SECTION_BASE = IMAGE_BASE + FIRST_SECTION_RVA
	org SECTION_BASE

	store SECTION_BASE-IMAGE_BASE at PE:OptionalHeader.AddressOfEntryPoint
	store SECTION_BASE-IMAGE_BASE at PE:SectionTable.VirtualAddress
	store FILE_OFFSET at PE:SectionTable.PointerToRawData

	virtual at 0
	     relocated_addresses:: rd NUMBER_OF_RELOCATIONS
	end virtual

	virtual at 0
	     relocation_types:: rw NUMBER_OF_RELOCATIONS
	end virtual

end namespace

RVA? equ -PE.IMAGE_BASE +

macro entry? address*
	namespace PE
		store address-IMAGE_BASE at PE:OptionalHeader.AddressOfEntryPoint
	end namespace
end macro

macro stack? reserve*,commit:1000h
	namespace PE
		store reserve at PE:OptionalHeader.SizeOfStackReserve
		store commit at PE:OptionalHeader.SizeOfStackCommit
	end namespace
end macro

macro heap? reserve*,commit:0
	namespace PE
		store reserve at PE:OptionalHeader.SizeOfHeapReserve
		store commit at PE:OptionalHeader.SizeOfHeapCommit
	end namespace
end macro

macro section?
	namespace PE

		repeat SECTION_DIRECTORIES
			end data
		end repeat

		local AREA,DATA_START,DATA_END
		AREA::
		DATA_START = $$
		DATA_END = $-($%-$%%)
		CheckSumBlocks equ CheckSumBlocks,AREA,DATA_START,DATA_END

		SECTION_SIZE = $ - SECTION_BASE
		store SECTION_SIZE at PE:SectionTable.VirtualSize+SECTION_INDEX*SectionTable.ENTRY_LENGTH
		align SECTION_ALIGNMENT
		SECTION_BASE = $
		section $%%
		align FILE_ALIGNMENT,0
		RAW_DATA_SIZE = $% - FILE_OFFSET
		store RAW_DATA_SIZE at PE:SectionTable.SizeOfRawData+SECTION_INDEX*SectionTable.ENTRY_LENGTH
		FILE_OFFSET = $%
		org SECTION_BASE

		if SECTION_SIZE > 0 & RAW_DATA_SIZE = 0
			load SECTION_CHARACTERISTICS from PE:SectionTable.Characteristics+SECTION_INDEX*SectionTable.ENTRY_LENGTH
			SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_CNT_UNINITIALIZED_DATA
			store SECTION_CHARACTERISTICS at PE:SectionTable.Characteristics+SECTION_INDEX*SectionTable.ENTRY_LENGTH
		end if

		if DEFINED_SECTION | SECTION_SIZE > 0
			SECTION_INDEX = SECTION_INDEX + 1
		end if

	end namespace
end macro

macro section? declaration*
	namespace PE

		section

		DEFINED_SECTION = 1

		store SECTION_BASE-IMAGE_BASE at PE:SectionTable.VirtualAddress+SECTION_INDEX*SectionTable.ENTRY_LENGTH
		store FILE_OFFSET at PE:SectionTable.PointerToRawData+SECTION_INDEX*SectionTable.ENTRY_LENGTH

		SECTION_DIRECTORIES = 0

		match name attributes, declaration

			store name:qword at PE:SectionTable.Name+SECTION_INDEX*SectionTable.ENTRY_LENGTH

			SECTION_CHARACTERISTICS = 0

			local seq,list
			define seq attributes
			while 1
				match car cdr, seq
					define list car
					define seq cdr
				else
					match any, seq
						define list any
					end match
					break
				end match
			end while
			irpv attribute, list
				match =readable?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_MEM_READ
				else match =writeable?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_MEM_WRITE
				else match =executable?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_MEM_EXECUTE
				else match =discardable?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_MEM_DISCARDABLE
				else match =shareable?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_MEM_SHARED
				else match =import?, attribute
					SECTION_DIRECTORIES = SECTION_DIRECTORIES + 1
					data import
				else match =export?, attribute
					SECTION_DIRECTORIES = SECTION_DIRECTORIES + 1
					data export
				else match =resource?, attribute
					SECTION_DIRECTORIES = SECTION_DIRECTORIES + 1
					data resource
				else match =fixups?, attribute
					SECTION_DIRECTORIES = SECTION_DIRECTORIES + 1
					data fixups
				else match =code?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_CNT_CODE
				else match =data?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_CNT_INITIALIZED_DATA
				else match =udata?, attribute
					SECTION_CHARACTERISTICS = SECTION_CHARACTERISTICS or IMAGE_SCN_CNT_UNINITIALIZED_DATA
				else
					err 'invalid argument'
				end match
			end irpv

			store SECTION_CHARACTERISTICS at PE:SectionTable.Characteristics+SECTION_INDEX*SectionTable.ENTRY_LENGTH
		else

			store declaration:qword at PE:SectionTable.Name+SECTION_INDEX*SectionTable.ENTRY_LENGTH

		end match

	end namespace
end macro

macro data? type*
	namespace PE
		local number
		match =export?, type
			number = 0
		else match =import?, type
			number = 1
		else match =resource?, type
			number = 2
		else match =fixups?, type
			number = 5
		else
			number = type
		end match
		define DATA_DIRECTORY number
		load DATA_BASE:dword from PE:RvaAndSizes.Rva+DATA_DIRECTORY*RvaAndSizes.ENTRY_LENGTH
		if DATA_BASE = 0
			store $-IMAGE_BASE:dword at PE:RvaAndSizes.Rva+DATA_DIRECTORY*RvaAndSizes.ENTRY_LENGTH
			if DATA_DIRECTORY = 5
				Fixups:
				PAGE_RVA = 0
				BLOCK_HEADER = 0
				BLOCK_SIZE = 0
				repeat NUMBER_OF_RELOCATIONS
					load ADDRESS:dword from relocated_addresses:(%-1) shl 2
					if ADDRESS and not 0FFFh <> PAGE_RVA
						if BLOCK_HEADER
							if BLOCK_SIZE and 11b
								dw 0
								BLOCK_SIZE = BLOCK_SIZE + 2
							end if
							store BLOCK_SIZE:dword at BLOCK_HEADER+4
						end if
						PAGE_RVA = ADDRESS and not 0FFFh
						BLOCK_HEADER = $
						dd PAGE_RVA
						dd 0
						BLOCK_SIZE = 8
					end if
					load TYPE:word from relocation_types:(%-1) shl 1
					dw (ADDRESS and 0FFFh) or (TYPE shl 12)
					BLOCK_SIZE = BLOCK_SIZE + 2
				end repeat
				if BLOCK_HEADER
					if BLOCK_SIZE and 11b
						dw 0
						BLOCK_SIZE = BLOCK_SIZE + 2
					end if
					store BLOCK_SIZE:dword at BLOCK_HEADER+4
				end if
			end if
		else
			err 'data already defined'
		end if
	end namespace
end macro

macro end?.data?
	namespace PE
		load DATA_BASE:dword from PE:RvaAndSizes.Rva+DATA_DIRECTORY*RvaAndSizes.ENTRY_LENGTH
		store $-IMAGE_BASE-DATA_BASE:dword at PE:RvaAndSizes.Size+DATA_DIRECTORY*RvaAndSizes.ENTRY_LENGTH
		restore DATA_DIRECTORY
	end namespace
end macro

macro dword? value
    local v
    v = value
    if ~ v relativeto 0 & v relativeto PE.RELOCATION
	store $-PE.IMAGE_BASE:dword at PE.relocated_addresses:PE.RELOCATION_INDEX shl 2
	store IMAGE_REL_BASED_HIGHLOW:word at PE.relocation_types:PE.RELOCATION_INDEX shl 1
	dd v - PE.RELOCATION
	PE.RELOCATION_INDEX = PE.RELOCATION_INDEX + 1
    else
	dd v
    end if
end macro

macro qword? value
    local v
    v = value
    if ~ v relativeto 0 & v relativeto PE.RELOCATION
	store $-PE.IMAGE_BASE:dword at PE.relocated_addresses:PE.RELOCATION_INDEX shl 2
	store IMAGE_REL_BASED_DIR64:word at PE.relocation_types:PE.RELOCATION_INDEX shl 1
	dq v - PE.RELOCATION
	PE.RELOCATION_INDEX = PE.RELOCATION_INDEX + 1
    else
	dq v
    end if
end macro

macro dd? definitions&
    iterate value,definitions
	match ?, value
	    dd ?
	else match n =dup? ?, value
	    dd n dup ?
	else match n =dup? v, value
	    repeat n
		dword v
	    end repeat
	else
	    dword value
	end match
    end iterate
end macro

struc dd? definitions&
    label . : dword
    iterate value,definitions
	match ?, value
	    dd ?
	else match n =dup? ?, value
	    dd n dup ?
	else match n =dup? v, value
	    repeat n
		dword v
	    end repeat
	else
	    dword value
	end match
    end iterate
end struc

macro dq? definitions&
    iterate value,definitions
	match ?, value
	    dq ?
	else match n =dup? ?, value
	    dq n dup ?
	else match n =dup? v, value
	    repeat n
		qword v
	    end repeat
	else
	    qword value
	end match
    end iterate
end macro

struc dq? definitions&
    label . : qword
    iterate value,definitions
	match ?, value
	    dq ?
	else match n =dup? ?, value
	    dq n dup ?
	else match n =dup? v, value
	    repeat n
		qword v
	    end repeat
	else
	    qword value
	end match
    end iterate
end struc

postpone
	purge section?
	section
	namespace PE
		SIZE_OF_IMAGE := SECTION_BASE - IMAGE_BASE
		NUMBER_OF_SECTIONS := SECTION_INDEX
		NUMBER_OF_RELOCATIONS := RELOCATION_INDEX
		if defined Settings.ComputeCheckSum & Settings.ComputeCheckSum
			match list, CheckSumBlocks
				CHECKSUM = 0
				iterate <AREA,DATA_START,DATA_END>,list
					repeat (DATA_END-DATA_START) shr 1, POSITION:0
						load H:word from AREA:DATA_START+POSITION shl 1
						CHECKSUM = CHECKSUM + H
					end repeat
					if (DATA_END-DATA_START) and 1
						load H:byte from AREA:DATA_END-1
						CHECKSUM = CHECKSUM + H
					end if
					while CHECKSUM shr 16
						CHECKSUM = CHECKSUM shr 16 + CHECKSUM and 0FFFFh
					end while
				end iterate
				CHECKSUM = CHECKSUM + $%
				store CHECKSUM at PE:OptionalHeader.CheckSum
			end match
		end if
	end namespace
end postpone
