#!/bin/bash

set -e

OS_NAME="MoonShardOS"

BLUE='\033[38;5;24m'
GOLD='\033[38;5;179m'
GRAY='\033[38;5;75m'
GREEN='\033[38;5;114m'
RED='\033[38;5;167m'
BOLD='\033[1m'
RESET='\033[0m'

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

echo -e "${GOLD}${BOLD}${OS_NAME} Build Termination${RESET}\n"

echo -e "${GRAY}[1/3] Searching for build processes...${RESET}"

FOUND=0

for PROCESS in make cmake ninja limine; do
    if pgrep -x "$PROCESS" > /dev/null 2>&1; then
        echo -e "  ${RED}::${RESET} Found ${PROCESS}"
        pkill -x "$PROCESS" 2>/dev/null || true
        FOUND=1
    fi
done

if [ "$FOUND" -eq 0 ]; then
    echo -e "  ${GRAY}::${RESET} No active build processes found"
fi

echo -e "\n${GRAY}[2/3] Removing build artifacts...${RESET}"

FILES=(
    "kernel.elf"
    "${OS_NAME}.iso"
    "iso/boot/kernel.elf"
)

for FILE in "${FILES[@]}"; do
    if [ -f "$FILE" ]; then
        rm -f "$FILE"
        echo -e "  ${BLUE}::${RESET} Removed ${FILE}"
    fi
done

for FILE in *.o; do
    if [ -f "$FILE" ]; then
        rm -f "$FILE"
        echo -e "  ${BLUE}::${RESET} Removed ${FILE}"
    fi
done

echo -e "\n${GRAY}[3/3] Cleaning complete...${RESET}"

echo -e "\n${GREEN}========================================${RESET}"
echo -e "${GREEN}${BOLD}      Build terminated successfully!${RESET}"
echo -e "${GREEN}========================================${RESET}\n"

echo -e "${GOLD}Project:${RESET} ${OS_NAME}"
echo -e "${GOLD}Status:${RESET}  Build artifacts removed\n"