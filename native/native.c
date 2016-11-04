/*
 * native - Native application to test calls into an EBC protocol
 *          To be used against the EBC driver built from driver.asm
 * Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
 */
#include <efi.h>
#include <efilib.h>

#define PrintStatusError(str) Print(str L" [%d] %r", Status & 0x7FFFFFFF)

typedef EFI_STATUS(EFIAPI *EFI_HELLO) (VOID);

typedef struct {
	EFI_HELLO               Hello;
} EFI_CUSTOM_PROTOCOL;

/* Custom protocol definition */
EFI_GUID CustomProtocolGUID = { 0x230aa93e, 0x3d8a, 0x4bbd,{ 0x8d, 0x48, 0x77, 0xd3, 0xed, 0x9c, 0xa7, 0x9b } };

// Application entrypoint (must be set to 'efi_main' for gnu-efi crt0 compatibility)
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;
	UINTN Event;
	EFI_CUSTOM_PROTOCOL *CustomProtocol = NULL;

#if defined(_GNU_EFI)
	InitializeLib(ImageHandle, SystemTable);
#endif

	// Locate the protocol
	Status = gBS->LocateProtocol(&CustomProtocolGUID, NULL, (VOID**) &CustomProtocol);

	if EFI_ERROR(Status) {
		PrintStatusError(L"Could not locate custom protocol");
		return Status;
	}

	Print(L"%ECalled from native:%N ");
	CustomProtocol->Hello();

	Print(L"\nPress any key to exit.\n");
	SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
	SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Event);
	SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

	return EFI_SUCCESS;
}
