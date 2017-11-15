Simple EFI boot manager
=======
sEFI is simple boot manager for UEFI capable computers.
It uses simple C language and GNU-EFI library.

Differences from other boot managers:

 - file browser to select efi applications
 - really simple structure of config files

Structure of config directories
-------
Config files are located in folder \EFI\sefi\ in your ESP (EFI System Partition)
For every config you need to create directory containing files **item.conf** and **options.conf**

Structure of config files
-------
Config files needs to be saved in **UTF16** file format and can not have extra symbols after text (for example new_line, tab, etc.) Example of this config files are in the source code (config_example)

**item.conf** contains path to efi application

**options.conf** contains extra options for efi application (used for booting EFI stub kernels)

Compilation and Installation
------

1. use git to clone my repository git clone https://github.com/mintos5/sefi.git
2. cd to created directory
3. cmake .
4. make sefi_manual
5. copy compiled version to folder \EFI\sefi\ in your ESP
6. use efibootmgr to add it to list od your computer boot entries
..* sudo efibootmgr-c -d /dev/sda2 -p 2 -L "sEFI boot manager" -l "\efi\sefi\sefi.efi"