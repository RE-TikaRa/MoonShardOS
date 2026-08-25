#!/bin/bash

set -e

BLUE='\033[38;5;24m'
GOLD='\033[38;5;179m'
GRAY='\033[38;5;75m'
GREEN='\033[38;5;114m'
RESET='\033[0m'

echo

echo -e "${BLUE}"
echo "====================================================================================================="
echo "                                                                                                    "
echo "                                                                                                    "
echo "                                                                                                    "
echo "░███     ░███                                    ░██████   ░██                                    ░██ "
echo "░████   ░████                                   ░██   ░██  ░██                                    ░██ "
echo "░██░██ ░██░██  ░███████   ░███████  ░████████  ░██         ░████████   ░██████   ░██░████  ░████████ "
echo "░██ ░████ ░██ ░██    ░██ ░██    ░██ ░██    ░██  ░████████  ░██    ░██       ░██  ░███     ░██    ░██ "
echo "░██  ░██  ░██ ░██    ░██ ░██    ░██ ░██    ░██        ░██  ░██    ░██  ░███████  ░██      ░██    ░██ "
echo "░██       ░██ ░██    ░██ ░██    ░██ ░██    ░██ ░██   ░██   ░██    ░██ ░██   ░██  ░██      ░██   ░███ "
echo "░██       ░██  ░███████   ░███████  ░██    ░██  ░██████    ░██    ░██  ░█████░██ ░██       ░█████░██ "
echo "                                                                                                    "
echo "                                                                                                    "
echo "                                                                                                    "
echo "====================================================================================================="
echo -e "${RESET}"

echo -e "${GOLD}MoonShardOS Kernel Build${RESET}"
echo

echo -e "${GRAY}[1/4]${RESET} Compiling kernel..."

gcc \
    -ffreestanding \
    -m64 \
    -mno-red-zone \
    -fno-stack-protector \
    -nostdlib \
    -c src/kernel.c \
    -o kernel.o

echo
echo -e "${GRAY}[2/4]${RESET} Linking kernel..."

ld \
    -m elf_x86_64 \
    -nostdlib \
    -z max-page-size=0x1000 \
    -T linker.ld \
    kernel.o \
    -o kernel.elf

echo
echo -e "${GRAY}[3/4]${RESET} Updating ISO files..."

cp kernel.elf iso/boot/kernel.elf

echo
echo -e "${GRAY}[4/4]${RESET} Building ISO..."

rm -f MoonShardOS.iso

xorriso \
    -as mkisofs \
    -b boot/limine-bios-cd.bin \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    --efi-boot boot/BOOTX64.EFI \
    -efi-boot-part \
    --efi-boot-image \
    -o MoonShardOS.iso \
    iso

limine/limine bios-install MoonShardOS.iso

echo
echo -e "${GREEN}========================================${RESET}"
echo -e "${GREEN}        Build successful!${RESET}"
echo -e "${GREEN}========================================${RESET}"
echo
echo -e "${GOLD}ISO:${RESET} MoonShardOS.iso"
echo
echo -e "${GOLD}Run with QEMU:${RESET}"
echo
echo "qemu-system-x86_64 \\"
echo "    -cdrom MoonShardOS.iso \\"
echo "    -m 256M \\"
echo "    -vga std"
echo