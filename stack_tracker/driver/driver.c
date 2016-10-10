/*
 * Arm EBC stack tracker test driver
 * Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
 */
#include <efi.h>
#include <efilib.h>

typedef INT32 INTN;
typedef UINT32 UINTN;

#define PrintStatusError(str) Print(str L" [%d] %r", Status & 0x7FFFFFFF)

/* Custom protocol definition */
EFI_GUID CustomProtocolGUID = { 0x9bb363b1, 0xb588, 0x4e45, {0x88, 0x06, 0x5f, 0x69, 0x56, 0xae, 0xad, 0xb4} };

typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM0) (UINTN, UINTN, UINTN, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM1) (UINT64, UINTN, UINTN, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM2) (UINTN, UINT64, UINTN, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM3) (UINT64, UINT64, UINTN, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM4) (UINTN, UINTN, UINT64, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM5) (UINT64, UINTN, UINT64, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM6) (UINTN, UINT64, UINT64, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM7) (UINT64, UINT64, UINT64, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM8) (UINTN, UINTN, UINTN, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM9) (UINT64, UINTN, UINTN, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM10) (UINTN, UINT64, UINTN, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM11) (UINT64, UINT64, UINTN, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM12) (UINTN, UINTN, UINT64, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM13) (UINT64, UINTN, UINT64, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM14) (UINTN, UINT64, UINT64, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MULTIPARAM15) (UINT64, UINT64, UINT64, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MAXPARAMS64) (UINT64, UINT64, UINT64, UINT64, UINT64, UINT64,
	UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_MAXPARAMSMIXED) (UINTN, UINT64, UINTN, UINT64, UINTN, UINT64,
	UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64, UINTN, UINT64);

typedef struct {
	EFI_MULTIPARAM0     MultiParam0;
	EFI_MULTIPARAM1     MultiParam1;
	EFI_MULTIPARAM2     MultiParam2;
	EFI_MULTIPARAM3     MultiParam3;
	EFI_MULTIPARAM4     MultiParam4;
	EFI_MULTIPARAM5     MultiParam5;
	EFI_MULTIPARAM6     MultiParam6;
	EFI_MULTIPARAM7     MultiParam7;
	EFI_MULTIPARAM8     MultiParam8;
	EFI_MULTIPARAM9     MultiParam9;
	EFI_MULTIPARAM10    MultiParam10;
	EFI_MULTIPARAM11    MultiParam11;
	EFI_MULTIPARAM12    MultiParam12;
	EFI_MULTIPARAM13    MultiParam13;
	EFI_MULTIPARAM14    MultiParam14;
	EFI_MULTIPARAM15    MultiParam15;
	EFI_MAXPARAMS64     MaxParams64;
	EFI_MAXPARAMSMIXED  MaxParamsMixed;
} EFI_CUSTOM_PROTOCOL;

static EFI_STATUS common_base(INTN val, UINT64* p)
{
	INTN i, r = 0;
	UINT64 c;
	static UINT64 v[4] = {
		0x1B1B1B1B1A1A1A1AULL,
		0x2B2B2B2B2A2A2A2AULL,
		0x3B3B3B3B3A3A3A3AULL,
		0x4B4B4B4B4A4A4A4AULL,
	};
	for (i = 0; i < 4; i++) {
		c = v[i] & ((val & (1 << i)) ? 0xFFFFFFFFFFFFFFFFULL: 0x00000000FFFFFFFFULL);
		r += (p[i] == c) ? (1 << i) : 0;
	}
	if (r != 0x0F) {
		Print(L"MultiParam%d failed (0x%X):\n", val, r);
		for (i = 0; i < 4; i++)
			Print(L"  p%d = 0x%016llX\n", i, p[i]);
		return EFI_INVALID_PARAMETER;
	}
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI MultiParam0(UINTN p0, UINTN p1, UINTN p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(0, p);
}

EFI_STATUS EFIAPI MultiParam1(UINT64 p0, UINTN p1, UINTN p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(1, p);
}

EFI_STATUS EFIAPI MultiParam2(UINTN p0, UINT64 p1, UINTN p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(2, p);
}

EFI_STATUS EFIAPI MultiParam3(UINT64 p0, UINT64 p1, UINTN p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(3, p);
}

EFI_STATUS EFIAPI MultiParam4(UINTN p0, UINTN p1, UINT64 p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(4, p);
}

EFI_STATUS EFIAPI MultiParam5(UINT64 p0, UINTN p1, UINT64 p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(5, p);
}

EFI_STATUS EFIAPI MultiParam6(UINTN p0, UINT64 p1, UINT64 p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(6, p);
}

EFI_STATUS EFIAPI MultiParam7(UINT64 p0, UINT64 p1, UINT64 p2, UINTN p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(7, p);
}

EFI_STATUS EFIAPI MultiParam8(UINTN p0, UINTN p1, UINTN p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(8, p);
}

EFI_STATUS EFIAPI MultiParam9(UINT64 p0, UINTN p1, UINTN p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(9, p);
}

EFI_STATUS EFIAPI MultiParam10(UINTN p0, UINT64 p1, UINTN p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(10, p);
}

EFI_STATUS EFIAPI MultiParam11(UINT64 p0, UINT64 p1, UINTN p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(11, p);
}

EFI_STATUS EFIAPI MultiParam12(UINTN p0, UINTN p1, UINT64 p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(12, p);
}

EFI_STATUS EFIAPI MultiParam13(UINT64 p0, UINTN p1, UINT64 p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(13, p);
}

EFI_STATUS EFIAPI MultiParam14(UINTN p0, UINT64 p1, UINT64 p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(14, p);
}

EFI_STATUS EFIAPI MultiParam15(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3)
{
	UINT64 p[4] = { p0, p1, p2, p3 };
	return common_base(15, p);
}

EFI_STATUS EFIAPI MaxParams64(UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4, UINT64 p5,
	UINT64 p6, UINT64 p7, UINT64 p8, UINT64 p9, UINT64 p10, UINT64 p11,
	UINT64 p12, UINT64 p13, UINT64 p14, UINT64 p15, UINT64 p16)
{
	INTN i;
	UINT64 p[16] = { p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
		p13, p14, p15, p16 };
	UINT64 v = 0LL;
	for (i = 0; i < 16; i++) {
		if (p[i] != v) {
			Print(L"MaxParams64 p%d: Got 0x%016llX, expected 0x%016llX\n", i, p[i], v);
			return EFI_INVALID_PARAMETER;
		}
		v += 0x1111111111111111ULL;
	}
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI MaxParamsMixed(UINTN p0, UINT64 p1, UINTN p2, UINT64 p3, UINTN p4,
	UINT64 p5, UINTN p6, UINT64 p7, UINTN p8, UINT64 p9, UINTN p10, UINT64 p11,
	UINTN p12, UINT64 p13, UINTN p14, UINT64 p15)
{
	INTN i;
	UINT64 p[16] = { p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
		p12, p13, p14, p15 };
	UINT64 c, v = 0LL;
	for (i = 0; i < 16; i++) {
		c = (i % 2) ? v : (v & 0xFFFFFFFFULL);
		if (p[i] != c) {
			Print(L"MaxParamsMixed p%d: Got 0x%016llX, expected 0x%016llX\n", i, p[i], c);
			return EFI_INVALID_PARAMETER;
		}
		v += 0x1111111111111111LL;
	}
	return EFI_SUCCESS;
}

static EFI_CUSTOM_PROTOCOL CustomProtocol = {
	MultiParam0,
	MultiParam1,
	MultiParam2,
	MultiParam3,
	MultiParam4,
	MultiParam5,
	MultiParam6,
	MultiParam7,
	MultiParam8,
	MultiParam9,
	MultiParam10,
	MultiParam11,
	MultiParam12,
	MultiParam13,
	MultiParam14,
	MultiParam15,
	MaxParams64,
	MaxParamsMixed,
};

/* Handle for our custom protocol */
static EFI_HANDLE CustomProtocolHandle = NULL;

CHAR16 *ShortDriverName = L"custprot";
CHAR16 *FullDriverName = L"Custom Protocol Driver";

/* Return the driver name */
static EFI_STATUS EFIAPI
GetDriverName(EFI_COMPONENT_NAME_PROTOCOL *This,
	CHAR8 *Language, CHAR16 **DriverName)
{
	*DriverName = FullDriverName;
	return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI
GetDriverName2(EFI_COMPONENT_NAME2_PROTOCOL *This,
	CHAR8 *Language, CHAR16 **DriverName)
{
	*DriverName = FullDriverName;
	return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI
BindingSupported(EFI_DRIVER_BINDING_PROTOCOL *This,
	EFI_HANDLE ControllerHandle,
	EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath)
{
	return EFI_UNSUPPORTED;
}

/*
 * The platform determines whether it will support the older Component
 * Name Protocol or the current Component Name2 Protocol, or both.
 * Because of this, it is strongly recommended that you implement both
 * protocols in your driver.
 */
static EFI_COMPONENT_NAME_PROTOCOL ComponentName = {
	.GetDriverName = GetDriverName,
	.SupportedLanguages = (CHAR8 *) "eng"
};

static EFI_COMPONENT_NAME2_PROTOCOL ComponentName2 = {
	.GetDriverName = GetDriverName2,
	.SupportedLanguages = (CHAR8 *) "en"
};

static EFI_DRIVER_BINDING_PROTOCOL DriverBinding = {
	.Supported = BindingSupported,
	.Start = NULL,
	.Stop = NULL,
	.Version = 0x10,
	.ImageHandle = NULL,
	.DriverBindingHandle = NULL
};

/**
 * Uninstall driver
 *
 * @v ImageHandle       Handle identifying the loaded image
 * @ret Status          EFI status code to return on exit
 */
EFI_STATUS EFIAPI
DriverUninstall(EFI_HANDLE ImageHandle)
{
	/* Remove the binding protocols */
	BS->UninstallMultipleProtocolInterfaces(ImageHandle,
		&gEfiDriverBindingProtocolGuid, &DriverBinding,
		&gEfiComponentNameProtocolGuid, &ComponentName,
		&gEfiComponentName2ProtocolGuid, &ComponentName2,
		NULL);

	/* Uninstall our protocol */
	BS->UninstallMultipleProtocolInterfaces(CustomProtocolHandle,
		&CustomProtocolGUID, &CustomProtocol, NULL);

	return EFI_SUCCESS;
}

/**
 * Install driver - The entrypoint for our driver executable
 *
 * @v ImageHandle       Handle identifying the loaded image
 * @v SystemTable       Pointers to EFI system calls
 * @ret Status          EFI status code to return on exit
 */
EFI_STATUS EFIAPI
DriverInstall(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_STATUS Status;
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage = NULL;
	VOID *Interface;

	InitializeLib(ImageHandle, SystemTable);

	Status = BS->LocateProtocol(&CustomProtocolGUID, NULL, &Interface);
	if (Status == EFI_SUCCESS) {
		Print(L"This driver has already been installed\n");
		return EFI_LOAD_ERROR;
	}
	/* The only valid status we expect is NOT FOUND here */
	if (Status != EFI_NOT_FOUND) {
		PrintStatusError(L"Unexpected initial status");
		return Status;
	}
	Status = BS->InstallProtocolInterface(
		&CustomProtocolHandle, &CustomProtocolGUID, EFI_NATIVE_INTERFACE, &CustomProtocol);
	if (EFI_ERROR(Status)) {
		PrintStatusError(L"Could not install protocol");
		return Status;
	}

	/* Grab a handle to this image, so that we can add an unload to our driver */
	Status = BS->OpenProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid,
		(VOID **)&LoadedImage, ImageHandle,
		NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	if (EFI_ERROR(Status)) {
		PrintStatusError(L"Could not open loaded image protocol");
		return Status;
	}

	/* Configure driver binding protocol */
	DriverBinding.ImageHandle = ImageHandle;
	DriverBinding.DriverBindingHandle = ImageHandle;

	/* Install driver */
	Status = BS->InstallMultipleProtocolInterfaces(&DriverBinding.DriverBindingHandle,
		&gEfiDriverBindingProtocolGuid, &DriverBinding,
		&gEfiComponentNameProtocolGuid, &ComponentName,
		&gEfiComponentName2ProtocolGuid, &ComponentName2,
		NULL);

	if (EFI_ERROR(Status)) {
		PrintStatusError(L"Could not bind driver");
		return Status;
	}

	/* Register the uninstall callback */
	LoadedImage->Unload = DriverUninstall;

	return EFI_SUCCESS;
}

/* Designate the driver entrypoint */
EFI_DRIVER_ENTRY_POINT(DriverInstall)
