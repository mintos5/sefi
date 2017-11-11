#include <efi.h>
#include <efilib.h>

static VOID list_directory() {

}

EFI_STATUS EFIAPI efi_main (EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    UINTN index, handleBuffSize;
    EFI_STATUS err;
    EFI_INPUT_KEY key;

    EFI_LOADED_IMAGE *loadedImage;

    EFI_HANDLE *handleBuff;
    EFI_DEVICE_PATH *pathDisk;
    EFI_FILE *root_dir;
    EFI_FILE_INFO *Info;
    UINTN BufSize = SIZE_OF_EFI_FILE_INFO + 1024;


    InitializeLib(imageHandle, systemTable);
    Info = AllocatePool(BufSize);

    //Initialize loadedImage
    err = uefi_call_wrapper(BS->OpenProtocol, 6, imageHandle, &LoadedImageProtocol, &loadedImage,
                            imageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(err)) {
        Print(L"Error getting a LoadedImageProtocol handle: %r ", err);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return err;
    }

    Print(L"Testing access to disks:");
    err = uefi_call_wrapper(BS->LocateHandleBuffer,5,ByProtocol,&DiskIoProtocol,NULL,
                            &handleBuffSize,&handleBuff);
    if (EFI_ERROR(err)) {
        Print(L"Error getting to disks handles: %r ", err);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return err;
    }
    Print(L"Founded %d disks\n",handleBuffSize);
    CHAR16 *str;
    for (index=0;index < handleBuffSize; index++) {
        pathDisk = DevicePathFromHandle(handleBuff[index]);
        str = DevicePathToStr(pathDisk);
        Print(L"%s\n",str);
        FreePool(str);

        root_dir = LibOpenRoot(handleBuff[index]);
        if (!root_dir) {
            Print(L"Unable to open root directory:\n");
            //uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        }
        else {
            Print(L"Opened the root directory\n");
            Print(L"Printing the content of root directory\n");


            err = uefi_call_wrapper(root_dir->SetPosition,2,root_dir,0);
            if (EFI_ERROR(err)) {
                Print(L"Error setting root_dir possition: %r ", err);
                uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
                continue;
            }

            while (1){
                BufSize = SIZE_OF_EFI_FILE_INFO + 1024;
                err = uefi_call_wrapper(root_dir->Read,3,root_dir,&BufSize,Info);
                if (EFI_ERROR(err)) {
                    Print(L"Error opening root_dir: %r ", err);
                    uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
                    break;
                }
                if (BufSize == 0) {
                    break;
                }
                if (Info->Attribute & EFI_FILE_DIRECTORY){
                    Print(L"directory: ");
                }
                Print(L"%s\n",Info->FileName);
            }

            uefi_call_wrapper(root_dir->Close, 1, root_dir);
        }

    }
    FreePool(Info);


    //Write and read from console
    Print(L"\nSEFI boot-manager prototype\n");
    Print(L"\n");
    Print(L"Using disk: UUID\n");
    Print(L"Found this automatic EFI entries:\n");
    Print(L"1: \\EFI\\sefi\\test.efi \n");
    Print(L"2: \\EFI\\Microsoft\\Boot\\bootmgfw.efi\n");
    Print(L"3: \\EFI\\ubuntu\\grubx64.efi\n");
    Print(L"4: \\EFI\\sefi\\kernel.efi\n");
    Print(L"Press numeric key to select founded efi application\n");
    Print(L"Press any key to manually search for efi executable");
    Print(L">");
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
        case '8':
            FreePool(path);
            err = uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            Print(L"Error calling ResetSystem: %r", err);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            return err;
            break;
        case '9':
            FreePool(path);
            err = uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
            Print(L"Error calling ResetSystem: %r", err);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            return err;
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
