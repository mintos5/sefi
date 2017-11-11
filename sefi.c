#include <efi.h>
#include <efilib.h>
#define EFI_AUTO_MODE 99
#define EFI_INTERACT_MODE 100

#define KEY_INPUT_UNK 0
#define KEY_INPUT_NUM 1
#define KEY_INPUT_CHAR 2

typedef struct config{
    CHAR16 *path;
    CHAR16 *options;
    struct config *next;
}CONFIG;

typedef struct {
    UINTN type;
    UINTN number;
    CHAR16 symbol;
} KEY_INPUT;

static VOID read_line() {
    //todo with this enable changig of boot options for automatic and interactive mode

}

static KEY_INPUT read_symbol(){
    KEY_INPUT out;
    UINTN index;
    EFI_INPUT_KEY key;

    Print(L">");
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    switch (key.UnicodeChar) {
        case '0':
            out.type = KEY_INPUT_NUM;
            out.number = 0;
            out.symbol = '0';
            break;
        case '1':
            out.type = KEY_INPUT_NUM;
            out.number = 1;
            out.symbol = '1';
            break;
        case '2':
            out.type = KEY_INPUT_NUM;
            out.number = 2;
            out.symbol = '2';
            break;
        case '3':
            out.type = KEY_INPUT_NUM;
            out.number = 3;
            out.symbol = '3';
            break;
        case '4':
            out.type = KEY_INPUT_NUM;
            out.number = 4;
            out.symbol = '4';
            break;
        case '5':
            out.type = KEY_INPUT_NUM;
            out.number = 5;
            out.symbol = '5';
            break;
        case '6':
            out.type = KEY_INPUT_NUM;
            out.number = 6;
            out.symbol = '6';
            break;
        case '7':
            out.type = KEY_INPUT_NUM;
            out.number = 7;
            out.symbol = '7';
            break;
        case '8':
            out.type = KEY_INPUT_NUM;
            out.number = 8;
            out.symbol = '8';
            break;
        case '9':
            out.type = KEY_INPUT_NUM;
            out.number = 9;
            out.symbol = '9';
            break;
        case 'i':
        case 'I':
            out.type = KEY_INPUT_CHAR;
            out.symbol = 'i';
            break;
        case 'a':
        case 'A':
            out.type = KEY_INPUT_CHAR;
            out.symbol = 'a';
            break;
        case 'r':
        case 'R':
            out.type = KEY_INPUT_CHAR;
            out.symbol = 'r';
        case 'o':
        case 'O':
            out.type = KEY_INPUT_CHAR;
            out.symbol = 'o';
        case 's':
        case 'S':
            out.type = KEY_INPUT_CHAR;
            out.symbol = 's';
        case 'h':
        case 'H':
            out.type = KEY_INPUT_CHAR;
            out.symbol = 'h';
        default:
            out.type = KEY_INPUT_UNK;
    }
    return out;
}

static VOID list_directory() {

}

static UINTN list_disks(EFI_HANDLE *diskHandleBuff, UINTN *workingVolumes, UINTN workingVolumesCount){
    //returns selected disk:
    UINTN index;
    CHAR16 *string;
    EFI_DEVICE_PATH *pathDisk;
    Print(L"Please select disk:\n");
    for (index=0;index<workingVolumesCount;index++) {
        pathDisk = DevicePathFromHandle(diskHandleBuff[workingVolumes[index]]);
        string = DevicePathToStr(pathDisk);
        Print(L"%d: ",index);
        Print(L"%s\n",string);
        FreePool(string);
    }
    KEY_INPUT key = read_symbol();
    if (key.type == KEY_INPUT_NUM){
        if (key.number > workingVolumesCount){
            Print(L"Incorrect number selection, selecting first choice");
            return workingVolumes[0];
        }
        else {
            return workingVolumes[key.number];
        }
    }
    else{
        Print(L"Incorrect key selection, selecting first choice");
        return workingVolumes[0];
    }
}

static EFI_STATUS get_config(EFI_HANDLE diskHandle, const CHAR16 *name, CONFIG *configList){
    //todo reading configs from file, first just hard code...
    EFI_STATUS status = 0;
    return status;
}

static EFI_STATUS automatic_mode (EFI_HANDLE diskHandle, CONFIG *configList, EFI_DEVICE_PATH **path, CHAR16 **options) {
    EFI_STATUS status = 0;
    KEY_INPUT key;


    Print(L"AUTOMATIC_MODE\n");
    key = read_symbol();
    if (key.type == KEY_INPUT_CHAR){
        if (key.symbol == 'i'){
            status = EFI_INTERACT_MODE;
        }
    }
    else if (key.type == KEY_INPUT_NUM){
        status = 0;
    }
    else {
        //
        status = EFI_INVALID_PARAMETER;
    }
    return status;
    //todo free the config list
}

static EFI_STATUS interactive_mode (EFI_HANDLE diskHandle, EFI_DEVICE_PATH **path, CHAR16 **options) {
    //todo list everything
    EFI_STATUS status = 0;
    KEY_INPUT key;

    Print(L"INTERACTIVE_MODE\n");

    key = read_symbol();
    if (key.type == KEY_INPUT_CHAR || key.type == KEY_INPUT_NUM){
        switch (key.symbol){
            case 'a':
                status = EFI_AUTO_MODE;
                break;
            case 'i':
                status = EFI_INTERACT_MODE;
                break;
            default:
                status = EFI_INVALID_PARAMETER;
        }
    }
    else {
        status = EFI_INVALID_PARAMETER;
    }
    return status;
}

static EFI_STATUS boot_efi (EFI_HANDLE parentImage, EFI_DEVICE_PATH *path, CHAR16 *options) {
    EFI_STATUS status;

    if (!path) {
        Print(L"Error getting device path.\n");
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return EFI_INVALID_PARAMETER;
    }

    EFI_HANDLE nextImage;
    status = uefi_call_wrapper(BS->LoadImage, 6, FALSE, parentImage, path, NULL, 0, &nextImage);
    if (EFI_ERROR(status)) {
        Print(L"Error loading testing image: %r\n", status);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        if (options){
            FreePool(options);
        }
        FreePool(path);
        return status;
    }
    if (options) {
        EFI_LOADED_IMAGE *loadedNextImage;

        status = uefi_call_wrapper(BS->OpenProtocol, 6, nextImage, &LoadedImageProtocol, &loadedNextImage,
                                parentImage, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(status)) {
            Print(L"Error getting LoadedImageProtocol handle: %r\n", status);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            uefi_call_wrapper(BS->UnloadImage, 1, nextImage);
            FreePool(options);
            FreePool(path);
            return status;
        }
        loadedNextImage->LoadOptions = options;
        loadedNextImage->LoadOptionsSize = (StrLen(loadedNextImage->LoadOptions)+1) * sizeof(CHAR16);
    }
    status = uefi_call_wrapper(BS->StartImage, 3, nextImage, NULL, NULL);

    //UNLOADING
    uefi_call_wrapper(BS->UnloadImage, 1, nextImage);
    FreePool(options);
    FreePool(path);
    return status;
}

EFI_STATUS EFIAPI efi_main (EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    UINTN index, diskHandleBuffSize;                    //integer variables
    EFI_STATUS status;                              //return value from other functions
    EFI_LOADED_IMAGE *loadedImage;                  //variable to access EFI_LOADED_IMAGE_PROTOCOL

    EFI_HANDLE *diskHandleBuff;                         //array of handlers for disks

    InitializeLib(imageHandle, systemTable);

    //Initialize EFI_LOADED_IMAGE_PROTOCOL from passed handle
    status = uefi_call_wrapper(BS->OpenProtocol, 6, imageHandle, &LoadedImageProtocol, &loadedImage,
                            imageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(status)) {
        Print(L"Error getting a LoadedImageProtocol handle: %r \n", status);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return status;
    }
    //Get all handles for disks
    status = uefi_call_wrapper(BS->LocateHandleBuffer,5,ByProtocol,&DiskIoProtocol,NULL,
                               &diskHandleBuffSize,&diskHandleBuff);
    if (EFI_ERROR(status)) {
        Print(L"Error getting to disks handles: %r \n", status);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return status;
    }
    EFI_FILE *rootDir;                             //root directory for ESP partition
    UINTN *workingVolumes;
    UINTN workingVolumesCount = 0;
    workingVolumes = AllocatePool(diskHandleBuffSize * sizeof(INTN));
    for (index=0;index < diskHandleBuffSize; index++) {

        rootDir = LibOpenRoot(diskHandleBuff[index]);
        if (rootDir) {
            workingVolumes[workingVolumesCount] = index;
            workingVolumesCount++;
            uefi_call_wrapper(rootDir->Close, 1, rootDir);
        }
    }
    EFI_DEVICE_PATH *path = NULL;
    CHAR16 *options = NULL;
    UINTN firstRun = 1;
    UINTN selection = 0;
    CONFIG *configList;
    configList = AllocatePool(sizeof(CONFIG));
    configList->next = NULL;

    status = get_config(loadedImage->DeviceHandle,L"\\EFI\\sefi\\sefi.conf",configList);
    if (EFI_ERROR(status)) {
        Print(L"Error: %r ", status);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        FreePool(configList);
    }
    status = EFI_AUTO_MODE;
    while (1){
        if (status == EFI_AUTO_MODE) {
            if (firstRun) {
                status = automatic_mode(loadedImage->DeviceHandle,configList,&path,&options);
                firstRun = 0;
                continue;
            }
            else {
                status = automatic_mode(diskHandleBuff[selection],configList,&path,&options);
                continue;
            }
        }
        else if (status == EFI_INTERACT_MODE) {
            UINTN selection;
            selection = list_disks(diskHandleBuff,workingVolumes,workingVolumesCount);
            status = interactive_mode(diskHandleBuff[selection],&path,&options);
            continue;
        }
        else if (EFI_ERROR(status)) {
            FreePool(configList);
            FreePool(workingVolumes);
            Print(L"Error: %r ", status);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            return status;
        }
        else {
            //everything is awesome
            break;
        }
    }
    //Free used buffers
    FreePool(configList);
    FreePool(workingVolumes);
    status = boot_efi(imageHandle,path,options);
    return status;

    /*EFI_DEVICE_PATH *pathDisk;
    EFI_FILE *root_dir;
    EFI_FILE_INFO *Info;
    UINTN BufSize = SIZE_OF_EFI_FILE_INFO + 1024;

    Info = AllocatePool(BufSize);
    Print(L"Founded %d disks\n",diskHandleBuffSize);
    CHAR16 *str;
    for (index=0;index < diskHandleBuffSize; index++) {
        pathDisk = DevicePathFromHandle(diskHandleBuff[index]);
        str = DevicePathToStr(pathDisk);
        Print(L"%s\n",str);
        FreePool(str);

        root_dir = LibOpenRoot(diskHandleBuff[index]);
        if (!root_dir) {
            Print(L"Unable to open root directory:\n");
            //uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        }
        else {
            Print(L"Opened the root directory\n");
            Print(L"Printing the content of root directory\n");


            status = uefi_call_wrapper(root_dir->SetPosition,2,root_dir,0);
            if (EFI_ERROR(status)) {
                Print(L"Error setting root_dir possition: %r ", status);
                uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
                continue;
            }

            while (1){
                BufSize = SIZE_OF_EFI_FILE_INFO + 1024;
                status = uefi_call_wrapper(root_dir->Read,3,root_dir,&BufSize,Info);
                if (EFI_ERROR(status)) {
                    Print(L"Error opening root_dir: %r ", status);
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

    EFI_INPUT_KEY key;
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
            status = uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            Print(L"Error calling ResetSystem: %r", status);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            return status;
            break;
        case '9':
            FreePool(path);
            status = uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
            Print(L"Error calling ResetSystem: %r", status);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            return status;
            break;
        default:
            ;
    }
    FreePool(workingVolumes);
    status = boot_efi(imageHandle,path,options);*/
    return status;
}
