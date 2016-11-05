/*
 * native - Native application to test calls into an EBC protocol
 *          To be used against the EBC driver built from driver.asm
 * Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
 */
#include <efi.h>
#include <efilib.h>

#define PrintStatusError(str) Print(str L" [%x] %r\n", Status & 0x7FFFFFFF, Status)

typedef EFI_STATUS(EFIAPI *EFI_HELLO) (VOID);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM0) (UINTN, UINTN, UINTN, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM1) (UINT64, UINTN, UINTN, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM2) (UINTN, UINT64, UINTN, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM3) (UINT64, UINT64, UINTN, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM4) (UINTN, UINTN, UINT64, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM5) (UINT64, UINTN, UINT64, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM6) (UINTN, UINT64, UINT64, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM7) (UINT64, UINT64, UINT64, UINTN);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM8) (UINTN, UINTN, UINTN, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM9) (UINT64, UINTN, UINTN, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM10) (UINTN, UINT64, UINTN, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM11) (UINT64, UINT64, UINTN, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM12) (UINTN, UINTN, UINT64, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM13) (UINT64, UINTN, UINT64, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM14) (UINTN, UINT64, UINT64, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM15) (UINT64, UINT64, UINT64, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MAXPARAMS64) (UINT64, UINT64, UINT64, UINT64, UINT64, UINT64,
	UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MAXPARAMSMIXED) (UINTN, UINT64, UINTN, UINT64, UINTN, UINT64,
	UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64);
typedef EFI_STATUS(EFIAPI *EFI_MAXPARAMSNATURAL) (UINTN, UINTN, UINTN, UINTN, UINTN, UINTN,
	UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN);

typedef struct {
	EFI_HELLO            Hello;
	EFI_MULTIPARAM0      MultiParam0;
	EFI_MULTIPARAM1      MultiParam1;
	EFI_MULTIPARAM2      MultiParam2;
	EFI_MULTIPARAM3      MultiParam3;
	EFI_MULTIPARAM4      MultiParam4;
	EFI_MULTIPARAM5      MultiParam5;
	EFI_MULTIPARAM6      MultiParam6;
	EFI_MULTIPARAM7      MultiParam7;
	EFI_MULTIPARAM8      MultiParam8;
	EFI_MULTIPARAM9      MultiParam9;
	EFI_MULTIPARAM10     MultiParam10;
	EFI_MULTIPARAM11     MultiParam11;
	EFI_MULTIPARAM12     MultiParam12;
	EFI_MULTIPARAM13     MultiParam13;
	EFI_MULTIPARAM14     MultiParam14;
	EFI_MULTIPARAM15     MultiParam15;
} EFI_CUSTOM_PROTOCOL;

EFI_STATUS EFIAPI MultiParamCall0(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam0(0x1A1A1A1A, 0x2A2A2A2A, 0x3A3A3A3A, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall1(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam1(0x1B1B1B1B1A1A1A1AULL, 0x2A2A2A2A, 0x3A3A3A3A, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall2(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam2(0x1A1A1A1A, 0x2B2B2B2B2A2A2A2AULL, 0x3A3A3A3A, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall3(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam3(0x1B1B1B1B1A1A1A1AULL, 0x2B2B2B2B2A2A2A2AULL, 0x3A3A3A3A, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall4(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam4(0x1A1A1A1A, 0x2A2A2A2A, 0x3B3B3B3B3A3A3A3AULL, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall5(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam5(0x1B1B1B1B1A1A1A1AULL, 0x2A2A2A2A, 0x3B3B3B3B3A3A3A3AULL, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall6(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam6(0x1A1A1A1A, 0x2B2B2B2B2A2A2A2AULL, 0x3B3B3B3B3A3A3A3AULL, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall7(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam7(0x1B1B1B1B1A1A1A1AULL, 0x2B2B2B2B2A2A2A2AULL, 0x3B3B3B3B3A3A3A3AULL, 0x4A4A4A4A);
}
EFI_STATUS EFIAPI MultiParamCall8(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam8(0x1A1A1A1A, 0x2A2A2A2A, 0x3A3A3A3A, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall9(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam9(0x1B1B1B1B1A1A1A1AULL, 0x2A2A2A2A, 0x3A3A3A3A, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall10(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam10(0x1A1A1A1A, 0x2B2B2B2B2A2A2A2AULL, 0x3A3A3A3A, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall11(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam11(0x1B1B1B1B1A1A1A1AULL, 0x2B2B2B2B2A2A2A2AULL, 0x3A3A3A3A, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall12(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam12(0x1A1A1A1A, 0x2A2A2A2A, 0x3B3B3B3B3A3A3A3AULL, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall13(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam13(0x1B1B1B1B1A1A1A1AULL, 0x2A2A2A2A, 0x3B3B3B3B3A3A3A3AULL, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall14(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam14(0x1A1A1A1A, 0x2B2B2B2B2A2A2A2AULL, 0x3B3B3B3B3A3A3A3AULL, 0x4B4B4B4B4A4A4A4AULL);
}
EFI_STATUS EFIAPI MultiParamCall15(EFI_CUSTOM_PROTOCOL *CustomProtocol) {
	return CustomProtocol->MultiParam15(0x1B1B1B1B1A1A1A1AULL, 0x2B2B2B2B2A2A2A2AULL, 0x3B3B3B3B3A3A3A3AULL, 0x4B4B4B4B4A4A4A4AULL);
}

typedef EFI_STATUS(EFIAPI *MULTIPARAMCALL)(EFI_CUSTOM_PROTOCOL *CustomProtocol);
MULTIPARAMCALL MultiParamCallTable[16] = {
	MultiParamCall0,
	MultiParamCall1,
	MultiParamCall2,
	MultiParamCall3,
	MultiParamCall4,
	MultiParamCall5,
	MultiParamCall6,
	MultiParamCall7,
	MultiParamCall8,
	MultiParamCall9,
	MultiParamCall10,
	MultiParamCall11,
	MultiParamCall12,
	MultiParamCall13,
	MultiParamCall14,
	MultiParamCall15,
};

/* Custom protocol definition */
EFI_GUID CustomProtocolGUID = { 0x230aa93e, 0x3d8a, 0x4bbd,{ 0x8d, 0x48, 0x77, 0xd3, 0xed, 0x9c, 0xa7, 0x9b } };

// Application entrypoint (must be set to 'efi_main' for gnu-efi crt0 compatibility)
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;
	UINTN i, Event;
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

	// Call into our EBC driver
	Print(L"%E");
	Status = CustomProtocol->Hello();
	Print(L"%N");
	if EFI_ERROR(Status) {
		PrintStatusError(L"ERROR");
		return Status;
	}

	// Test all four combinations of UINTN and UINT64 parameters
	for (i = 0; i < 16; i++) {
		Status = MultiParamCallTable[i](CustomProtocol);
		Print(L"MultiParam%d: %s", i, EFI_ERROR(Status) ? L"FAIL" : L"PASS\n");
		if EFI_ERROR(Status) {
			Print(L" - [%x] %r\n", Status & 0x7FFFFFFF, Status);
			return Status;
		}
	}

	Print(L"\nPress any key to exit.\n");
	SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
	SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Event);
	SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

	return EFI_SUCCESS;
}
