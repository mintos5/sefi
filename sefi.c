/**
 * @file sefi.c
 * @author Michal Skuta
 * @date 15 Nov 2017
 * @brief This file contains all needed code for sefi boot manager.
 *
 */

#include <efi.h>
#include <efilib.h>

/** macro for specific max lines of screen output */
#define MAX_LINES 13
/** macro to specify working */
#define EFI_AUTO_MODE 99
/** macro to specify working */
#define EFI_INTERACT_MODE 100
/** macro to specify working */
#define EFI_SHUTDOWN 101
/** macro to specify working */
#define EFI_RESTART 102

/** macro to specify type of input (special characters) */
#define KEY_INPUT_UNK 0
/** macro to specify type of input (numbers)*/
#define KEY_INPUT_NUM 1
/** macro to specify type of input (alphabetic characters)*/
#define KEY_INPUT_CHAR 2


/**
 * @brief Structure for linked list of boot configurations
 *
 * Structure to create linked list of boot configurations used in loading config from specific
 * directory and in creating automatic boot entries in auto_mode.
 */
typedef struct config{
    CHAR16 *name;           /**< configurtation name CONFIG#name */
    CHAR16 *path;           /**< path to efi executable CONFIG#path */
    CHAR16 *options;        /**< options for executable CONFIG#options */
    struct config *next;    /**< pointer for next config on linked list CONFIG#next */
    struct config *previous;/**< pointer for previous config on linked list CONFIG#previous */
}CONFIG;


/**
 * @brief Structure for keyaboard input
 *
 * Structure storing the user input from keyboard. It saves last key character, decimal value of number
 * and type of input.
 */
typedef struct {
    UINTN type;     /**< type of input, using macros KEY_INPUT_UNK KEY_INPUT#type */
    UINTN number;   /**< decimal value of user input KEY_INPUT#number */
    CHAR16 symbol;  /**< last alphabetic character of user input KEY_INPUT#symbol */
} KEY_INPUT;

/**
 * @brief Function to free memory of linked list
 *
 * Function to free memory of linked list. It is needed to traverse the linked list and remove all
 * nodes of this list. To remove one node function needs to remove all variables
 * created with AllocatePool(). We are using function FreePool().
 *
 *
 * @param headConfigList head of linked list of configs
 * @return VOID
 */
static VOID free_config(CONFIG *headConfigList){
    //go to the end of list:
    CONFIG * configList = headConfigList;
    CONFIG *temp;
    if (configList == NULL){
        return;
    }
    //move to the end of linked list
    while (configList->next){
        configList = configList->next;
    }
    //remove all nodes from list
    while (configList){
        temp = configList->previous;
        if (configList->name){
            FreePool(configList->name);
        }
        if (configList->path){
            FreePool(configList->path);
        }
        if (configList->options){
            FreePool(configList->options);
        }
        FreePool(configList);
        configList = temp;
    }
}

static VOID read_line() {
    //todo enable changing of boot options for automatic and interactive mode
}

/**
 * @brief Function to read input from users keyaboard
 *
 * Function writes a prompt '>' and waits for user input. After user accepts the input with enter key
 * function creates KEY_INPUT variable
 *
 * @return KEY_INPUT variable containing user input
 */
static KEY_INPUT read_symbol(){
    EFI_STATUS status;
    KEY_INPUT out;
    out.type = KEY_INPUT_UNK;
    out.number = 0;
    UINTN index;
    EFI_INPUT_KEY key;
    //Print prompt for user
    Print(L">");
    while (1){
        status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
        if (EFI_ERROR(status)) {
            uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
            continue;
        }
        else {
            Print(L"%c",key.UnicodeChar);
            if (key.UnicodeChar == CHAR_LINEFEED || key.UnicodeChar == CHAR_CARRIAGE_RETURN){
                Print(L"\n");
                break;
            }
            switch (key.UnicodeChar) {
                case '0':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 0;
                    out.symbol = '0';
                    break;
                case '1':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 1;
                    out.symbol = '1';
                    break;
                case '2':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 2;
                    out.symbol = '2';
                    break;
                case '3':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 3;
                    out.symbol = '3';
                    break;
                case '4':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 4;
                    out.symbol = '4';
                    break;
                case '5':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 5;
                    out.symbol = '5';
                    break;
                case '6':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 6;
                    out.symbol = '6';
                    break;
                case '7':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 7;
                    out.symbol = '7';
                    break;
                case '8':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 8;
                    out.symbol = '8';
                    break;
                case '9':
                    out.type = KEY_INPUT_NUM;
                    out.number = out.number*10;
                    out.number += 9;
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
                    break;
                case 'o':
                case 'O':
                    out.type = KEY_INPUT_CHAR;
                    out.symbol = 'o';
                    break;
                case 's':
                case 'S':
                    out.type = KEY_INPUT_CHAR;
                    out.symbol = 's';
                    break;
                case 'h':
                case 'H':
                    out.type = KEY_INPUT_CHAR;
                    out.symbol = 'h';
                    break;
                case 'n':
                case 'N':
                    out.type = KEY_INPUT_CHAR;
                    out.symbol = 'n';
                    break;
                case 'b':
                case 'B':
                    out.type = KEY_INPUT_CHAR;
                    out.symbol = 'b';
                    break;
                default:
                    out.type = KEY_INPUT_UNK;
            }
        }
    }
    return out;
}

/**
 * @brief Function to list bootable disks/volumes
 *
 * Function outputs all available bootable volumes. After that it waits for user input
 *
 *
 * @param *diskHandleBuff buffer of handles to access disks on system
 * @param *workingVolumes array of working and tested indices for diskHandleBuff
 * @param workingVolumesCount size of workingVolumes array
 * @return index of selected disk
 */
static UINTN list_disks(EFI_HANDLE *diskHandleBuff, UINTN *workingVolumes, UINTN workingVolumesCount){
    UINTN index;
    CHAR16 *string;
    EFI_DEVICE_PATH *pathDisk;
    Print(L"Please select disk:\n");
    for (index=0;index<workingVolumesCount;index++) {
        pathDisk = DevicePathFromHandle(diskHandleBuff[workingVolumes[index]]);
        string = DevicePathToStr(pathDisk);
        Print(L"[%d] ",index);
        Print(L"%s\n",string);
        FreePool(string);
    }
    KEY_INPUT key = read_symbol();
    if (key.type == KEY_INPUT_NUM){
        if (key.number > (workingVolumesCount-1)){
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

/**
 * @brief Function to list all files on specific disk
 *
 * Function list all files in specific directory (root directory) and user can select the file
 * or the next directory.
 *
 * @param *rootDir is handle to acces the files on disk or in directory
 * @param **filePath is string, where we send full path of selected file
 * @return size of CHAR16 string
 */
static UINTN list_directory(EFI_FILE *rootDir,CHAR16 **filePath) {
    EFI_STATUS status;
    KEY_INPUT key;
    UINTN selection,i,linesCounter;
    CHAR16 *file;
    file = NULL;
    EFI_FILE_INFO *info;

    Print(L"Please select boot file:\n");

    UINTN bufSize = SIZE_OF_EFI_FILE_INFO + 1024;
    info = AllocatePool(bufSize);

    status = uefi_call_wrapper(rootDir->SetPosition,2,rootDir,0);
    if (EFI_ERROR(status)) {
        Print(L"Error setting dir position: %r ", status);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return 0;   //bad value
    }
    selection = 0;
    linesCounter = 0;
    while (1){
        //process input
        if (linesCounter>=MAX_LINES){
            Print(L"Press i to cancel selecting files\n");
            //this is not the last file of this directory
            if (bufSize != 0){
                Print(L"Press n to show remaining files\n");
            }
            //read input from keyboard
            key = read_symbol();
            if (key.type == KEY_INPUT_CHAR || key.type == KEY_INPUT_UNK){
                //continue with listing root_directory
                if (key.symbol == 'i'){
                    FreePool(info);
                    return 0;
                }
                linesCounter = 0;
                continue;
            }
            else if (key.type == KEY_INPUT_NUM){
                //check for bigger input
                if (key.number > selection){
                    //bad input
                    Print(L"Wrong number, please select another option\n");
                    continue;
                }
                //reset the file position
                status = uefi_call_wrapper(rootDir->SetPosition,2,rootDir,0);
                if (EFI_ERROR(status)) {
                    Print(L"Error setting dir position: %r \n", status);
                    FreePool(info);
                    return 0;   //bad value
                }
                //move to correct info
                for (i=0;i<=key.number;i++){
                    bufSize = SIZE_OF_EFI_FILE_INFO + 1024;
                    status = uefi_call_wrapper(rootDir->Read,3,rootDir,&bufSize,info);
                    if (EFI_ERROR(status)) {
                        Print(L"Error moving dir to corect position: %r \n", status);
                        FreePool(info);
                        return 0;
                    }
                }
                //check if it is directory and call this function again
                if (info->Attribute & EFI_FILE_DIRECTORY){
                    //get the EFI_FILE
                    EFI_FILE_HANDLE nextDir;
                    status = uefi_call_wrapper(rootDir->Open,5,rootDir,&nextDir,info->FileName,EFI_FILE_MODE_READ,0);
                    if (EFI_ERROR(status)) {
                        Print(L"Error getting next_dir: %r \n", status);
                        FreePool(info);
                        return 0;
                    }
                    CHAR16 *tempFile = NULL;
                    UINTN returnSize = list_directory(nextDir,&tempFile);
                    if (returnSize==0){
                        //some error
                        FreePool(info);
                        return 0;
                    }
                    //combine the paths
                    UINTN pathSize = (1 + StrLen(info->FileName) + returnSize);

                    file = AllocateZeroPool( pathSize * sizeof(CHAR16));
                    SPrint(file,pathSize,L"\\");
                    CopyMem(file+1,info->FileName,StrLen(info->FileName) * sizeof(CHAR16));
                    CopyMem(file+1+StrLen(info->FileName),tempFile,returnSize * sizeof(CHAR16));
                    *filePath = file;
                    FreePool(tempFile);
                    FreePool(info);
                    uefi_call_wrapper(rootDir->Close,1,rootDir);
                    return pathSize;
                }
                //we have the file_name super....
                UINTN pathSize = (2 + StrLen(info->FileName));
                file = AllocateZeroPool( pathSize * sizeof(CHAR16));
                SPrint(file,pathSize,L"\\");
                CopyMem(file+1,info->FileName,StrLen(info->FileName)* sizeof(CHAR16));
                *filePath = file;
                FreePool(info);
                uefi_call_wrapper(rootDir->Close,1,rootDir);
                return pathSize;
            }
        }

        //Listing all files:
        //reading all files from directory
        bufSize = SIZE_OF_EFI_FILE_INFO + 1024;
        status = uefi_call_wrapper(rootDir->Read,3,rootDir,&bufSize,info);
        if (EFI_ERROR(status)) {
            Print(L"Error reading dir: %r ", status);
            FreePool(info);
            return 0;
        }
        //the last file
        if (bufSize == 0) {
            //with linesCounter set to specific We tell the functions to ask for user input
            linesCounter = MAX_LINES;
            continue;
        }
        else {
            Print(L"[%d] ",selection);
            if (info->Attribute & EFI_FILE_DIRECTORY){
                Print(L"dir: ");
            }
            Print(L"%s\n",info->FileName);
        }
        selection++;
        linesCounter++;
    }
}

/**
 * @brief Function to load configs from specific directory
 *
 * Function to load all configurations files from specific folder structure
 *
 *
 * @param diskHandle handle to read from disk
 * @param *name pointer to name of directory containing config files
 * @param *configList pointer to the head of list of configs
 * @return EFI_STATUS representing success or error of this function
 */
static EFI_STATUS get_config(EFI_HANDLE diskHandle, const CHAR16 *name, CONFIG *configList){
    EFI_FILE_INFO *fileInfoDir = NULL;
    UINTN bufSize = SIZE_OF_EFI_FILE_INFO + 1024;
    EFI_FILE *rootDir = NULL;
    EFI_STATUS status;
    CONFIG *actualConfig;
    CONFIG *tempConfig;
    actualConfig = configList;

    CHAR16 *buff;
    UINTN buffLenght;

    fileInfoDir = AllocatePool(bufSize);
    rootDir = LibOpenRoot(diskHandle);
    if (rootDir==NULL){
        return EFI_INVALID_PARAMETER;
    }
    EFI_FILE_HANDLE sefiDir;
    //sefiDir creation
    status = uefi_call_wrapper(rootDir->Open,5,rootDir,&sefiDir,name,EFI_FILE_MODE_READ,0);
    if (EFI_ERROR(status)) {
        Print(L"Error getting to config/options directory: %r \n", status);
        uefi_call_wrapper(rootDir->Close, 1, rootDir);
        return 0;
    }
    uefi_call_wrapper(rootDir->Close, 1, rootDir);

    while (1){
        bufSize = SIZE_OF_EFI_FILE_INFO + 1024;
        status = uefi_call_wrapper(sefiDir->Read,3,sefiDir,&bufSize,fileInfoDir);
        if (EFI_ERROR(status)) {
            Print(L"Error reading dir: %r ", status);
            FreePool(fileInfoDir);
            uefi_call_wrapper(sefiDir->Close, 1, sefiDir);
            return 0;
        }
        //the last file
        if (bufSize == 0) {
            break;
        }
        //work only with directories
        if (fileInfoDir->Attribute & EFI_FILE_DIRECTORY){
            //do not work with this directories
            if ( StrCmp(fileInfoDir->FileName,L"..") == 0){
                continue;
            }
            if (StrCmp(fileInfoDir->FileName,L".") == 0){
                continue;
            }
            //check if actualConfig has something
            if (actualConfig->name){
                tempConfig = AllocateZeroPool(sizeof(CONFIG));
                tempConfig->previous = actualConfig;
                actualConfig->next = tempConfig;
                actualConfig = tempConfig;
            }
            UINTN nameSize = StrLen(fileInfoDir->FileName)+1;

            actualConfig->name = AllocateZeroPool(nameSize * sizeof(CHAR16));
            CopyMem(actualConfig->name,fileInfoDir->FileName,nameSize * sizeof(CHAR16));

            EFI_FILE_HANDLE configDir;
            //configDir creation, getting to directory
            status = uefi_call_wrapper(sefiDir->Open,5,sefiDir,&configDir,fileInfoDir->FileName,EFI_FILE_MODE_READ,0);
            if (EFI_ERROR(status)) {
                Print(L"Error getting to config/options directory: %r \n", status);
                FreePool(fileInfoDir);
                uefi_call_wrapper(sefiDir->Close, 1, sefiDir);
                return 0;
            }
            EFI_FILE_HANDLE handleItem, handleOption;

            //READING the item.conf file
            //handleItem creation
            status = uefi_call_wrapper(configDir->Open, 5, configDir, &handleItem, L"item.conf", EFI_FILE_MODE_READ, 0);
            if (EFI_ERROR(status)){
                Print(L"Error getting to open item.conf in %s: %r \n", actualConfig->name, status);
                FreePool(fileInfoDir);
                uefi_call_wrapper(configDir->Close, 1, configDir);
                continue;
            }
            EFI_FILE_INFO *infoConf;
            infoConf = LibFileInfo(handleItem);
            buffLenght = infoConf->FileSize;
            buff = AllocatePool(buffLenght);
            //buffLenght is changed after calling this function
            status = uefi_call_wrapper(handleItem->Read, 3, handleItem, &buffLenght, buff);
            if (EFI_ERROR(status)){
                Print(L"Error getting to read item.conf: %r \n", status);
                FreePool(infoConf);
                FreePool(fileInfoDir);
                uefi_call_wrapper(handleItem->Close, 1, handleItem);
                uefi_call_wrapper(configDir->Close, 1, configDir);
                continue;
            }
            CHAR16 *finalPath = NULL;
            finalPath = AllocateZeroPool(buffLenght);
            //+1 to remove BOM header
            CopyMem(finalPath,buff+1,buffLenght-2);
            actualConfig->path = finalPath;
            FreePool(buff);
            FreePool(infoConf);
            uefi_call_wrapper(handleItem->Close, 1, handleItem);

            //READING the options.conf file
            //handleOption creation
            status = uefi_call_wrapper(configDir->Open, 5, configDir, &handleOption, L"options.conf", EFI_FILE_MODE_READ, 0);
            if (EFI_ERROR(status)){
                Print(L"Error getting to open options.conf in %s: %r \n", actualConfig->name, status);
                uefi_call_wrapper(configDir->Close, 1, configDir);
                continue;
            }
            infoConf = LibFileInfo(handleOption);
            buffLenght = infoConf->FileSize;
            buff = AllocatePool(buffLenght);
            //buffLenght is changed after calling this function
            status = uefi_call_wrapper(handleOption->Read, 3, handleOption, &buffLenght, buff);
            if (EFI_ERROR(status)){
                Print(L"Error getting to read options.conf: %r \n", status);
                FreePool(infoConf);
                uefi_call_wrapper(handleOption->Close, 1, handleOption);
                uefi_call_wrapper(configDir->Close, 1, configDir);
                continue;
            }
            finalPath = NULL;
            finalPath = AllocateZeroPool(buffLenght);
            //+1 to remove BOM header
            CopyMem(finalPath,buff+1,buffLenght-2);
            actualConfig->options = finalPath;
            FreePool(buff);
            FreePool(infoConf);
            uefi_call_wrapper(handleOption->Close, 1, handleOption);

            uefi_call_wrapper(configDir->Close, 1, configDir);
        }
    }
    FreePool(fileInfoDir);
    uefi_call_wrapper(sefiDir->Close, 1, sefiDir);
    return status;
}

/**
 * @brief Function to search and list all boot entries from config files
 *
 * Function searches for boot entries and list them. Waits for user input.
 * After the boot entry is selected, tries to load it.
 *
 *
 * @param diskHandle handle to read from disk
 * @param *configList pointer to the head of list of configs
 * @param **path pointer specifying full path to efi file
 * @param **options pointer to string of options
 * @return EFI_STATUS representing success or error of this function
 */
static EFI_STATUS automatic_mode (EFI_HANDLE diskHandle, CONFIG *configList, EFI_DEVICE_PATH **path, CHAR16 **options) {
    EFI_DEVICE_PATH *outPath;
    CHAR16 *outOptions;
    EFI_STATUS status = 0;
    KEY_INPUT key;
    CONFIG *actualConfig = configList;
    CONFIG **workingCongigs;
    //check which config is ok
    EFI_FILE *rootDir = NULL;
    EFI_FILE_HANDLE fileHandle;

    rootDir = LibOpenRoot(diskHandle);
    if (rootDir==NULL){
        return EFI_INVALID_PARAMETER;
    }
    //Get the size of list of configs
    UINTN counter = 0;
    while (actualConfig){
        actualConfig = actualConfig->next;
        counter++;
    }
    //Create array of the same size, where the ponters to config will be stored
    workingCongigs = AllocatePool(counter * sizeof(CONFIG*));
    //Test configs
    actualConfig = configList;
    counter = 0;
    while (actualConfig){
        //check if file exists
        if (actualConfig->path){
            status = uefi_call_wrapper(rootDir->Open, 5, rootDir, &fileHandle, actualConfig->path, EFI_FILE_MODE_READ, 0);
            if (EFI_ERROR(status)){
                Print(L"File for conf %s: %r \n",actualConfig->name, status);
            }
            else {
                //working file
                workingCongigs[counter] = actualConfig;
                uefi_call_wrapper(fileHandle->Close, 1, fileHandle);
                //print this file
                Print(L"[%d] ",counter);
                Print(L"%s\n",actualConfig->name);
                counter++;
            }
        }
        actualConfig = actualConfig->next;
    }
    //Clean up
    uefi_call_wrapper(rootDir->Close, 1, rootDir);
    Print(L"[i] to restart to interactive mode\n");
    Print(L"Press corresponding number to boot\n");
    while (1){
        key = read_symbol();
        if (key.type == KEY_INPUT_CHAR){
            if (key.symbol == 'i'){
                status = EFI_INTERACT_MODE;
                break;
            }
        }
        else if (key.type == KEY_INPUT_NUM){
            if (counter==0){
                Print(L"Maybe try interact mode?:\n");
                continue;
            }
            if (key.number > counter) {
                continue;
            }
            else {
                outPath = FileDevicePath(diskHandle,workingCongigs[key.number]->path);
                CHAR16 *tempPointer = workingCongigs[key.number]->options;
                outOptions = AllocateZeroPool( (StrLen(tempPointer)+1) * sizeof(CHAR16));
                CopyMem(outOptions,tempPointer,StrLen(tempPointer) * sizeof(CHAR16));
                *path = outPath;
                *options = outOptions;
                status = EFI_SUCCESS;
                break;
            }
        }
        else {
            //
            status = EFI_INVALID_PARAMETER;
            break;
        }
    }
    FreePool(workingCongigs);
    return status;
}

/**
 * @brief Function to list all files on before selected disk
 *
 * Function list all files on disk. Waits for user input.
 * After the file is selected, tries to load it.
 *
 *
 * @param diskHandle handle to read from disk
 * @param **path pointer specifying full path to efi file
 * @param **options pointer to string of options
 * @return EFI_STATUS representing success or error of this function
 */
static EFI_STATUS interactive_mode (EFI_HANDLE diskHandle, EFI_DEVICE_PATH **path, CHAR16 **options) {
    EFI_DEVICE_PATH *tempPath = NULL;
    CHAR16 *tempOptions = NULL;
    EFI_STATUS status = 0;
    KEY_INPUT key;
    UINTN returnValue;
    EFI_FILE *rootDir = NULL;
    CHAR16 *pathString = NULL;
    //Open rootDir to disk
    rootDir = LibOpenRoot(diskHandle);
    if (rootDir==NULL){
        return EFI_INVALID_PARAMETER;
    }
    returnValue = list_directory(rootDir,&pathString);
    Print(L"selected file: ");
    if (returnValue>0){
        Print(L"%s\n",pathString);
        tempPath = FileDevicePath(diskHandle,pathString);
        *path = tempPath;
        *options = tempOptions;
        FreePool(pathString);
    }
    else {
        Print(L"\n");
    }
    Print(L"[b] to boot selected file\n");
    Print(L"[r] to restart your system\n");
    Print(L"[s] to shutdown your system\n");
    Print(L"[i] to restart interactive mode\n");
    Print(L"[a] to start automatic mode on actual disk\n");


    key = read_symbol();
    if (key.type == KEY_INPUT_CHAR || key.type == KEY_INPUT_NUM){
        switch (key.symbol){
            case 'a':
                status = EFI_AUTO_MODE;
                break;
            case 'i':
                status = EFI_INTERACT_MODE;
                break;
            case 'r':
                status = EFI_RESTART;
                break;
            case 's':
                status = EFI_SHUTDOWN;
                break;
            case 'b':
                if (returnValue>0){
                    status = EFI_SUCCESS;
                }
                else {
                    status = EFI_INVALID_PARAMETER;
                }
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

/**
 * @brief Function to boot image
 *
 * Function to free memory of linked list. It is needed to traverse the linked list and remove all
 * nodes of this list. To remove one node function needs to remove all variables
 * created with AllocatePool(). We are using function FreePool().
 *
 *
 * @param parentImage image of this program (sefi.efi)
 * @param *path full path to efi file
 * @param *options optional options for this file
 * @return EFI_STATUS representing success or error of this function
 */
static EFI_STATUS boot_efi (EFI_HANDLE parentImage, EFI_DEVICE_PATH *path, CHAR16 *options) {
    EFI_STATUS status;
    //Check if path is not NULL
    if (!path) {
        Print(L"Error getting device path.\n");
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        return EFI_INVALID_PARAMETER;
    }
    //Load image
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
    //Check if options is not NULL
    if (options) {
        Print(L"Booting with options:\n%s\n",options);
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
    //Start the loaded image
    status = uefi_call_wrapper(BS->StartImage, 3, nextImage, NULL, NULL);

    //UNLOADING
    uefi_call_wrapper(BS->UnloadImage, 1, nextImage);
    FreePool(options);
    FreePool(path);
    return status;
}

/**
 * @brief Main function of program
 *
 * Main function of program from where all other functions are called
 *
 *
 * @param imageHandle handle of the current (sefi.efi) image
 * @param systemTable UEFI system table
 * @return EFI_STATUS representing success or error of this function
 */
EFI_STATUS EFIAPI efi_main (EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    UINTN index, diskHandleBuffSize;                //integer variables
    EFI_STATUS status;                              //return value from other functions
    EFI_LOADED_IMAGE *loadedImage;                  //variable to access EFI_LOADED_IMAGE_PROTOCOL

    EFI_HANDLE *diskHandleBuff;                     //array of handlers for disks

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
        //Get root directory from disk handle
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
    UINTN selection = 0;                            // variable to specify selected disk
    CONFIG *configList;
    configList = AllocateZeroPool(sizeof(CONFIG));
    configList->name = NULL;
    configList->next = NULL;
    configList->previous = NULL;
    //Get configs from folder
    status = get_config(loadedImage->DeviceHandle,L"\\EFI\\sefi",configList);

    CONFIG *temp = configList;
    Print(L"Founded this configs:\n");
    while (temp){
        Print(L"path: %s\n",temp->path);
        Print(L"option: %s\n",temp->options);
        temp = temp->next;
    }


    if (EFI_ERROR(status)) {
        Print(L"Error: %r ", status);
        uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
        free_config(configList);
    }
    //Calling the help functions like automatic_mode or interactive_mode
    status = EFI_AUTO_MODE;
    while (1){
        if (status == EFI_SHUTDOWN) {
            free_config(configList);
            FreePool(workingVolumes);
            Print(L"Calling Shutdown:");
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            status = uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        }
        if (status == EFI_RESTART) {
            free_config(configList);
            FreePool(workingVolumes);
            uefi_call_wrapper(BS->Stall, 1, 3 * 1000 * 1000);
            status = uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
        }

        if (status == EFI_AUTO_MODE) {
            Print(L"Auto mode:\n");
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
            Print(L"Interactive mode:\n");
            selection = list_disks(diskHandleBuff,workingVolumes,workingVolumesCount);
            status = interactive_mode(diskHandleBuff[selection],&path,&options);
            continue;
        }
        else if (EFI_ERROR(status)) {
            free_config(configList);
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
    //Free used buffers and boot from informations from helper functions
    free_config(configList);
    FreePool(workingVolumes);
    status = boot_efi(imageHandle,path,options);
    return status;
}
