#!/bin/bash

# 任一步骤失败都停止，避免把不完整的 kernel.elf 继续打进 ISO。
set -e

# 构建输出使用固定颜色，便于在长日志中区分阶段和文件名。
BLUE='\033[38;5;24m'
GOLD='\033[38;5;179m'
GRAY='\033[38;5;75m'
GREEN='\033[38;5;114m'
BOLD='\033[1m'
RESET='\033[0m'

# 内核是 freestanding 程序，不链接宿主机 C 运行时，也不使用红区。
# -mno-red-zone 对中断/异常路径很重要，因为 CPU 可能在异步入口中覆盖
# 普通用户态代码会使用、但内核异常处理不能假定安全的栈下方区域。
OS_NAME="MoonShardOS"
CFLAGS="-ffreestanding -m64 -mno-red-zone -fno-stack-protector -nostdlib"
LDFLAGS="-m elf_x86_64 -nostdlib -z max-page-size=0x1000 -T linker.ld"

# C 源文件按模块列出；汇编异常入口在下方单独编译。
SOURCES=(
    "src/kernel.c"
    "src/graphics/framebuffer.c"
    "src/graphics/image.c"
    "src/graphics/font.c"
    "src/console/console.c"
    "src/cpu/gdt.c"
    "src/cpu/idt.c"
    "src/cpu/isr.c"
)

# 打印项目横幅，不参与构建结果。
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

# 先把每个源文件编译成目标文件，再统一交给 ld 链接。
echo -e "${GRAY}[1/4] Compiling kernel sources...${RESET}"

OBJECTS=()

for SRC in "${SOURCES[@]}"; do
    OBJ="${SRC##*/}"
    OBJ="${OBJ%.c}.o"

    # isr.c 和 isr.S 的默认文件名都会得到 isr.o，必须给 C 处理器改名。
    if [ "$SRC" = "src/cpu/isr.c" ]; then
        OBJ="isr_handler.o"
    fi

    OBJECTS+=("$OBJ")

    echo -e "  ${BLUE}::${RESET} CC ${SRC}"
    gcc $CFLAGS -c "$SRC" -o "$OBJ"
done

# isr.S 是 CPU 异常入口汇编代码，单独保留为 isr.o，
# 这样可以和改名后的 isr_handler.o 同时参与链接。
echo -e "  ${BLUE}::${RESET} AS src/cpu/isr.S"

gcc $CFLAGS \
    -c src/cpu/isr.S \
    -o isr.o

OBJECTS+=("isr.o")

echo -e "\n${GRAY}[2/4] Linking kernel...${RESET}"

ld $LDFLAGS \
    "${OBJECTS[@]}" \
    -o kernel.elf

echo -e "\n${GRAY}[3/4] Updating ISO structure...${RESET}"

cp kernel.elf iso/boot/kernel.elf

echo -e "\n${GRAY}[4/4] Generating Bootable ISO...${RESET}"

rm -f "${OS_NAME}.iso"

xorriso \
    -as mkisofs \
    -b boot/limine-bios-cd.bin \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    --efi-boot boot/BOOTX64.EFI \
    -efi-boot-part \
    --efi-boot-image \
    -o "${OS_NAME}.iso" \
    iso > /dev/null 2>&1

limine/limine bios-install \
    "${OS_NAME}.iso" > /dev/null 2>&1

# 目标文件和中间 ELF 只服务于本次 ISO 生成，最终保留 ISO 文件。

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
