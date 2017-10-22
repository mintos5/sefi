cmake_minimum_required(VERSION 3.8)
project(sefi)

set(GNUEFI_INC /usr/include/efi)
set(GNUEFI_LIB /usr/lib)
set(EFI_LDS /usr/lib/elf_x86_64_efi.lds)

include_directories(${GNUEFI_INC} ${GNUEFI_INC}/x86_64 ${GNUEFI_INC}/protocol)

set(COMMONFLAGS "-fno-stack-protector -fshort-wchar -mno-red-zone -Wall")
set(LDFLAGS "-nostdlib -znocombreloc -T ${EFI_LDS} -shared -Bsymbolic -L ${GNUEFI_LIB} -lgnuefi -lefi")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c11 ${COMMONFLAGS}")

set(SOURCE_FILES sefi.c)
add_library(efiloader_broken SHARED ${SOURCE_FILES})
set(CMAKE_SHARED_LINKER_FLAGS "${LDFLAGS}")

add_custom_target(efiloader_manual COMMAND make -f Makefile.manual
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
