/*
 * Simple UEFI driver for protocol testing
 * Copyright © 2016 Pete Batard <pete@akeo.ie> - Public Domain
 * See COPYING for the full licensing terms.
 */
#include <efi.h>
#include <efilib.h>

#if defined(_M_IX86) || defined(__i386__) || defined (_M_ARM) || defined(__arm__)
  #define NATIVE_FORMAT L"0x%08X"
#else
  #define NATIVE_FORMAT L"0x%016llX"
#endif

#define PrintStatusError(str) Print(str L" [%d] %r", Status & 0x7FFFFFFF)

/* Custom protocol definition */
EFI_GUID CustomProtocolGUID = { 0x1e81aff7, 0x5509, 0x4acc,{ 0xa9, 0x3f, 0x56, 0x55, 0x0d, 0xb1, 0xbd, 0xcc } };

typedef EFI_STATUS (EFIAPI *EFI_HELLO) (
	VOID
	);

typedef EFI_STATUS(EFIAPI *EFI_SINGLEPARAM_32) (
	UINT32
	);

typedef EFI_STATUS(EFIAPI *EFI_SINGLEPARAM_64) (
	UINT64
	);

typedef EFI_STATUS(EFIAPI *EFI_SINGLEPARAM_NATIVE) (
	UINTN
	);

typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM_FIXED) (
	UINT32,
	UINT64,
	UINT64,
	UINT64,
	UINT32,
	UINT32,
	UINT64
	);

typedef EFI_STATUS(EFIAPI *EFI_MULTIPARAM_NATIVE) (
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN,
	UINTN
	);

typedef struct {
	INTN                    DataNative;
	EFI_HELLO               Hello;
	EFI_SINGLEPARAM_32      SingleParam32;
	EFI_SINGLEPARAM_64      SingleParam64;
	EFI_SINGLEPARAM_NATIVE  SingleParamNative;
	EFI_MULTIPARAM_FIXED    MultiParamFixed;
	EFI_MULTIPARAM_NATIVE   MultiParamNative;
} EFI_CUSTOM_PROTOCOL;

EFI_STATUS Hello(VOID) {
	Print(L"Hello from Custom Protocol Driver\n");
	return EFI_SUCCESS;
}

EFI_STATUS SingleParam32(UINT32 p1) {
	Print(L"SingleParam32:\n");
	Print(L"  p1 = 0x%08X\n", p1);
	return EFI_SUCCESS;
}

EFI_STATUS SingleParam64(UINT64 p1) {
	Print(L"SingleParam64:\n");
	Print(L"  p1 = 0x%016llX\n", p1);
	return EFI_SUCCESS;
}

EFI_STATUS SingleParamNative(UINTN p1) {
	Print(L"SingleParamNative:\n");
	Print(L"  p1 = " NATIVE_FORMAT L"\n", p1);
	return EFI_SUCCESS;
}

EFI_STATUS MultiParamFixed(UINT32 p1, UINT64 p2, UINT64 p3, UINT64 p4, UINT32 p5, UINT32 p6, UINT64 p7) {
	Print(L"CustomMultiParamFixed:\n");
	Print(L"  p1 = 0x%08X\n", p1);
	Print(L"  p2 = 0x%016llX\n", p2);
	Print(L"  p3 = 0x%016llX\n", p3);
	Print(L"  p4 = 0x%016llX\n", p4);
	Print(L"  p5 = 0x%08X\n", p5);
	Print(L"  p6 = 0x%08X\n", p6);
	Print(L"  p7 = 0x%016llX\n", p7);
	return EFI_SUCCESS;
}

EFI_STATUS MultiParamNative(UINTN p1, UINTN p2, UINTN p3, UINTN p4, UINTN p5,
	UINTN p6, UINTN p7, UINTN p8, UINTN p9, UINTN p10, UINTN p11, UINTN p12) {
	Print(L"CustomMultiParamNative:\n");
	Print(L"  p1 = " NATIVE_FORMAT L"\n", p1);
	Print(L"  p2 = " NATIVE_FORMAT L"\n", p2);
	Print(L"  p3 = " NATIVE_FORMAT L"\n", p3);
	Print(L"  p4 = " NATIVE_FORMAT L"\n", p4);
	Print(L"  p5 = " NATIVE_FORMAT L"\n", p5);
	Print(L"  p6 = " NATIVE_FORMAT L"\n", p6);
	Print(L"  p7 = " NATIVE_FORMAT L"\n", p7);
	Print(L"  p8 = " NATIVE_FORMAT L"\n", p8);
	Print(L"  p9 = " NATIVE_FORMAT L"\n", p9);
	Print(L"  p10 = " NATIVE_FORMAT L"\n", p10);
	Print(L"  p11 = " NATIVE_FORMAT L"\n", p11);
	Print(L"  p12 = " NATIVE_FORMAT L"\n", p12);
	return EFI_SUCCESS;
}

static EFI_CUSTOM_PROTOCOL CustomProtocol = { 0xACCE55ED, Hello, SingleParam32, SingleParam64,
	SingleParamNative, MultiParamFixed, MultiParamNative };

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
