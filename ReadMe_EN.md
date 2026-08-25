# MoonShardOS

[![License](https://img.shields.io/badge/License-Apache--2.0-blue.svg)](LICENSE)

**[中文](ReadMe.md) | English**

> Long live objects! ~~Everything is an object (actually, I just copied Linux LOL)~~


MoonShardOS is an experimental operating system targeting `x86_64`. The kernel is written in C and booted through Limine.

The project starts from low-level hardware initialization and gradually builds its own graphics, Console, memory, device, runtime, and object systems, with a long-term focus on embedded devices, robotics, and dedicated computing systems.

## Current Status

MoonShardOS currently provides:

* `x86_64` kernel
* Limine boot support
* Framebuffer initialization
* Dynamic RGB color conversion
* Single-pixel read/write
* RGBA image rendering with alpha blending
* `8×8` bitmap font
* ASCII character set from `0x20` to `0x7E`
* Basic Console output
* Newline and automatic line wrapping
* GDT initialization
* IDT initialization
* Basic entries and handling for CPU exceptions `0~31`
* ISO image generation
* QEMU boot support

Current startup flow:

```text
Limine
   ↓
Kernel
   ↓
GDT
   ↓
Framebuffer
   ↓
Image
   ↓
Bitmap Font
   ↓
Console
   ↓
IDT / CPU Exceptions
```

The startup screen uses a dedicated graphical background, Logo, and title before entering the kernel Console.

## Project Structure

```text
MoonShardOS/
├── iso/                 # ISO filesystem
├── limine/              # Limine bootloader submodule
├── assets/
│   └── logo.png         # Logo source image
├── src/
│   ├── kernel.c         # Kernel entry
│   ├── logo.c            # Static pixel array generated from logo.png
│   ├── graphics/
│   │   ├── framebuffer.c
│   │   ├── framebuffer.h
│   │   ├── image.c
│   │   ├── image.h
│   │   ├── font.c
│   │   └── font.h
│   ├── console/
│   │   ├── console.c
│   │   └── console.h
│   └── cpu/
│       ├── gdt.c
│       ├── gdt.h
│       ├── idt.c
│       ├── idt.h
│       ├── isr.c
│       └── isr.S
├── build.sh             # Build script
├── tools/png2c.py       # PNG-to-C pixel conversion tool
├── linker.ld            # Kernel linker script
├── .gitignore
└── .gitmodules
```

## Graphics

MoonShardOS uses the framebuffer provided by Limine.

The framebuffer module provides:

```text
Framebuffer
├── Initialization
├── RGB color conversion
├── Clear
├── Pixel read
└── Pixel write
```

Color conversion uses the RGB masks provided by the framebuffer and does not depend on a fixed pixel layout.

The image module uses `AARRGGBB` pixel data and supports alpha blending:

```text
AARRGGBB
    ↓
Alpha blending
    ↓
Framebuffer color
    ↓
Pixel output
```

## Font System

The current font uses `8×8` bitmap glyphs covering:

```text
0x20 ' '
    ↓
0x7E '~'
```

The set includes:

```text
A-Z
a-z
0-9
Punctuation
Common symbols
```

Rendering flow:

```text
Character
    ↓
Bitmap glyph
    ↓
Pixel test
    ↓
framebuffer_put_pixel()
```

Future work may include larger fonts, Unicode support, and CJK fonts.

## Console

The Console is built on top of the font and framebuffer layers and provides kernel text output.

Current interfaces:

```c
console_init();
console_clear();
console_putchar();
console_write();
console_set_color();
```

Current features:

```text
New lines
Carriage returns
Automatic line wrapping
Screen edge handling
```

Output flow:

```text
console_write()
      ↓
console_putchar()
      ↓
font_draw_char()
      ↓
framebuffer_put_pixel()
```

The Console will gradually become the system terminal and provide the text output foundation for the Shell.

## CPU

A basic GDT, IDT, and CPU exception path are now included.

The current GDT contains:

```text
GDT[0]
    Null descriptor

GDT[1]
    Kernel code segment
    Selector: 0x08

GDT[2]
    Kernel data segment
    Selector: 0x10
```

CPU initialization flow:

```text
Limine
   ↓
kmain()
   ↓
gdt_init()
   ↓
framebuffer_init()
   ↓
console_init()
   ↓
idt_init()
```

The IDT currently installs the CPU-reserved vectors `0~31`. `src/cpu/isr.S` adds a zero error code where the CPU does not provide one, saves the vector, error code, and general-purpose registers, and calls the C handler in `src/cpu/isr.c`.

For a Page Fault, `CR2` provides the linear address that caused the exception. The current handler prints a diagnostic message and stops in a `hlt` loop; hardware interrupts, timers, and the TSS are not configured yet.

## Architecture Direction

MoonShardOS is not intended to reproduce an existing desktop operating system.

The project focuses on:

```text
Hardware
   ↓
Kernel
   ↓
Runtime
   ↓
Objects
   ↓
Applications
```

The long-term goal is to build a lightweight system foundation suitable for embedded devices, robots, and dedicated computing systems.

## Building

In a Linux environment, enter the project directory:

```bash
./build.sh
```

The build process is:

```text
Compile kernel sources
      ↓
Link kernel.elf
      ↓
Update ISO filesystem
      ↓
Generate MoonShardOS.iso
      ↓
Install Limine boot information
```

## Running with QEMU

After building:

```bash
qemu-system-x86_64 \
    -cdrom MoonShardOS.iso \
    -m 256M \
    -vga std
```

After changing the Logo source image, regenerate the static pixel array with:

```bash
python3 tools/png2c.py assets/logo.png src/logo.c
```

## Roadmap

### Boot and Graphics

* [x] Kernel boot
* [x] Limine integration
* [x] Framebuffer
* [x] Basic graphics
* [x] Image rendering
* [x] Alpha blending
* [x] Bitmap font
* [x] ASCII font

### Console

* [x] Basic text output
* [x] Character rendering
* [x] New lines
* [x] Automatic line wrapping
* [ ] Cursor
* [ ] Scrolling
* [ ] Input handling

### CPU

* [x] GDT
* [x] IDT
* [x] CPU exception handling
* [ ] Interrupt handling
* [ ] Timer
* [ ] TSS

### Memory

* [ ] Memory map parsing
* [ ] Physical memory manager
* [ ] Heap allocator
* [ ] Virtual memory
* [ ] Page allocator

### Kernel

* [ ] Scheduler
* [ ] Task abstraction
* [ ] Device abstraction
* [ ] Kernel object system

### Hardware

* [ ] Keyboard
* [ ] Storage
* [ ] Serial
* [ ] Networking
* [ ] Generic device interfaces

### Runtime

* [ ] Runtime
* [ ] Object system
* [ ] Application interface
* [ ] Shell

### Target Platforms

* [ ] Embedded devices
* [ ] ARM64
* [ ] Microcontroller-oriented runtime
* [ ] Robot-oriented APIs
* [ ] Dedicated hardware targets

## License

MoonShardOS is licensed under the Apache License 2.0.

Copyright 2026 亓翎_Re-TikaRa.

See the [LICENSE](LICENSE) file for the full license text.

---

MoonShardOS is an experimental project by [RE-TikaRa](https://github.com/RE-TikaRa).

Just a passing whim~ qwq

**[中文](ReadMe.md) | English**
