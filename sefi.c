#include <efi.h>
#include <efilib.h>
 
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    UINTN index;
    EFI_STATUS err;
    EFI_INPUT_KEY key;

    EFI_LOADED_IMAGE *loadedImage;

    InitializeLib(imageHandle, systemTable);

    //Initialize loadedImage
    err = uefi_call_wrapper(BS->OpenProtocol, 6, imageHandle, &LoadedImageProtocol, &loadedImage,
                            imageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(err)) {
        Print(L"Error getting a LoadedImageProtocol handle: %r ", err);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return err;
    }

    //Write and read from console
    Print(L"First prototype, to boot hard coded efi application press key 1,2,3,4:\n");
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

    EFI_DEVICE_PATH *path;
    CHAR16 *options;
    options = NULL;
    path = FileDevicePath(loadedImage->DeviceHandle, L"\\EFI\\sefi\\test.efi");

    switch (key.UnicodeChar) {
        case '1':
            ;
            break;
        case '2':
            path = FileDevicePath(loadedImage->DeviceHandle, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi");
            break;
        case '3':
            path = FileDevicePath(loadedImage->DeviceHandle, L"\\EFI\\ubuntu\\grubx64.efi");
            break;
        case '4':
            path = FileDevicePath(loadedImage->DeviceHandle, L"\\EFI\\sefi\\kernel.efi");
            //options = PoolPrint(L"root=UUID=5601cc87-5feb-4a8b-b848-5c830add82fc ro quiet rootfstype=ext4 add_efi_memmap initrd=\\EFI\\sefi\\initrd.img");
            options = PoolPrint(L"root=UUID=bd70ce49-fc7c-4f62-94ad-cde47df26371 ro quiet rootfstype=ext4 add_efi_memmap initrd=\\EFI\\sefi\\initrd.img");
            break;
        default:
            ;
    }
    Print(L"DEBUG1");
    if (!path) {
        Print(L"Error getting device path.");
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return EFI_INVALID_PARAMETER;
    }

    EFI_HANDLE nextImage;
    err = uefi_call_wrapper(BS->LoadImage, 6, FALSE, imageHandle, path, NULL, 0, &nextImage);
    if (EFI_ERROR(err)) {
        Print(L"Error loading testing image: %r", err);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        if (options){
            FreePool(options);
        }
        FreePool(path);
        return err;
    }
    Print(L"DEBUG2");
    if (options) {
        EFI_LOADED_IMAGE *loadedNextImage;

        err = uefi_call_wrapper(BS->OpenProtocol, 6, nextImage, &LoadedImageProtocol, &loadedNextImage,
                                imageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(err)) {
            Print(L"Error getting LoadedImageProtocol handle: %r", err);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            uefi_call_wrapper(BS->UnloadImage, 1, nextImage);
            FreePool(options);
            FreePool(path);
            return err;
        }
        loadedNextImage->LoadOptions = options;
        loadedNextImage->LoadOptionsSize = (StrLen(loadedNextImage->LoadOptions)+1) * sizeof(CHAR16);
    }
    Print(L"DEBUG3");
    err = uefi_call_wrapper(BS->StartImage, 3, nextImage, NULL, NULL);

    //UNLOADING
    //When I go here and when i do not go here...
    Print(L"END1");
    uefi_call_wrapper(BS->UnloadImage, 1, nextImage);
    FreePool(options);
    FreePool(path);
    Print(L"END2");
    return err;
}
