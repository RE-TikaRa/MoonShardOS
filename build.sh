#!/bin/bash
set -e

# --- 1. Colors & Styles ---
BLUE='\033[38;5;24m'
GOLD='\033[38;5;179m'
GRAY='\033[38;5;75m'
GREEN='\033[38;5;114m'
BOLD='\033[1m'
RESET='\033[0m'

# --- 2. Build Configurations ---
OS_NAME="MoonShardOS"
CFLAGS="-ffreestanding -m64 -mno-red-zone -fno-stack-protector -nostdlib"
LDFLAGS="-m elf_x86_64 -nostdlib -z max-page-size=0x1000 -T linker.ld"

# Source files array
SOURCES=(
    "src/kernel.c"
    "src/graphics/framebuffer.c"
    "src/graphics/image.c"
    "src/graphics/font.c"
    "src/console/console.c"
    "src/cpu/gdt.c"
)

# --- 3. Print Banner ---
echo -e "${BLUE}"
cat << 'EOF'
=====================================================================================================
                                                                                                     
                                                                                                     
                                                                                                     
░███     ░███                                    ░██████   ░██                                    ░██ 
░████   ░████                                   ░██   ░██  ░██                                    ░██ 
░██░██ ░██░██  ░███████   ░███████  ░████████  ░██         ░████████   ░██████   ░██░████  ░████████ 
░██ ░████ ░██ ░██    ░██ ░██    ░██ ░██    ░██  ░████████  ░██    ░██      ░██  ░███      ░██    ░██ 
░██  ░██  ░██ ░██    ░██ ░██    ░██ ░██    ░██         ░██  ░██    ░██  ░███████  ░██       ░██    ░██ 
░██       ░██ ░██    ░██ ░██    ░██ ░██    ░██ ░██   ░██   ░██    ░██ ░██   ░██  ░██       ░██   ░███ 
░██       ░██  ░███████   ░███████  ░██    ░██  ░██████     ░██    ░██  ░█████░██ ░██        ░█████░██ 
                                                                                                     
=====================================================================================================
EOF
echo -e "${RESET}"

echo -e "${GOLD}${BOLD}${OS_NAME} Kernel Build${RESET}\n"

# --- 4. Build Process ---
echo -e "${GRAY}[1/4] Compiling kernel sources...${RESET}"
OBJECTS=()

for SRC in "${SOURCES[@]}"; do
    OBJ="${SRC##*/}"
    OBJ="${OBJ%.c}.o"
    OBJECTS+=("$OBJ")

    echo -e "  ${BLUE}::${RESET} CC ${SRC}"
    gcc $CFLAGS -c "$SRC" -o "$OBJ"
done

echo -e "\n${GRAY}[2/4] Linking kernel...${RESET}"
ld $LDFLAGS "${OBJECTS[@]}" -o kernel.elf

echo -e "\n${GRAY}[3/4] Updating ISO structure...${RESET}"
cp kernel.elf iso/boot/kernel.elf

echo -e "\n${GRAY}[4/4] Generating Bootable ISO...${RESET}"
rm -f "${OS_NAME}.iso"

xorriso -as mkisofs -b boot/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot boot/BOOTX64.EFI -efi-boot-part --efi-boot-image \
    -o "${OS_NAME}.iso" iso > /dev/null 2>&1

limine/limine bios-install "${OS_NAME}.iso" > /dev/null 2>&1

# --- 5. Cleanup & Finish ---
rm -f "${OBJECTS[@]}" kernel.elf

echo -e "\n${GREEN}========================================${RESET}"
echo -e "${GREEN}${BOLD}      Build successful!${RESET}"
echo -e "${GREEN}========================================${RESET}\n"

echo -e "${GOLD}ISO Output:${RESET} ${OS_NAME}.iso\n"
echo -e "${GOLD}Run with QEMU:${RESET}"
echo -e "  qemu-system-x86_64 \\"
echo -e "      -cdrom ${OS_NAME}.iso \\"
echo -e "      -m 256M \\"
echo -e "      -vga std\n"