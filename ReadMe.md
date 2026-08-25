# MoonShardOS

[![License](https://img.shields.io/badge/License-Apache--2.0-blue.svg)](LICENSE)

> A small experimental operating system written from scratch.

项目目前面向 `x86_64` 平台，使用 C 编写内核，并通过 Limine 完成启动。

当前阶段主要用于探索操作系统底层机制，包括：

* Kernel
* Framebuffer
* Graphics
* Bootloader
* Memory
* Hardware abstraction
* Embedded-oriented system design

MoonShardOS 希望逐渐发展为一个轻量、可移植，并适合嵌入式设备与机器人系统使用的操作系统。

## Current Status

目前已经具备：

* x86_64 Kernel
* Limine Bootloader
* Framebuffer 初始化
* 基础图形绘制
* RGBA 图片显示
* 基础位图字体
* ISO 镜像生成
* QEMU 启动支持

当前内核启动后会初始化 framebuffer，绘制 MoonShardOS Logo、背景以及文字，然后进入空闲状态。

## Architecture

```text
MoonShardOS
│
├── Boot
│   └── Limine
│
├── Kernel
│   └── C
│
├── Graphics
│   ├── Framebuffer
│   ├── Image
│   └── Bitmap Font
│
├── Hardware
│   └── x86_64
│
└── Build System
    ├── GCC
    ├── GNU LD
    └── xorriso
```

## Repository

```text
MoonShardOS/
├── assets/          # Project assets
├── iso/             # ISO filesystem
├── limine/          # Limine bootloader
├── src/             # Kernel source
├── tools/            # Development tools
├── build.sh         # Build script
├── linker.ld        # Kernel linker script
└── .gitmodules
```

## Building

在 Linux 环境中进入项目目录：

```bash
./build.sh
```

构建脚本会依次编译 Kernel、链接 `kernel.elf`、更新 ISO 文件，并使用 `xorriso` 生成 `MoonShardOS.iso`。

## Running with QEMU

构建完成后：

```bash
qemu-system-x86_64 \
    -cdrom MoonShardOS.iso \
    -m 256M \
    -vga std
```

## Philosophy

MoonShardOS 不以复刻现有桌面操作系统为目标。

项目更关注：

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

希望逐渐建立一套适合机器人、嵌入式设备以及专用计算设备的系统基础。

## Roadmap

* [x] Kernel boot
* [x] Limine integration
* [x] Framebuffer
* [x] Basic graphics
* [x] Image rendering
* [x] Bitmap font
* [ ] Memory management
* [ ] Interrupt handling
* [ ] Physical memory manager
* [ ] Virtual memory
* [ ] Scheduler
* [ ] Device abstraction
* [ ] Input system
* [ ] Storage
* [ ] Networking
* [ ] Runtime
* [ ] Object system
* [ ] Embedded targets
* [ ] Robot-oriented APIs

## License

MoonShardOS is licensed under the Apache License 2.0.

Copyright 2026 亓翎_Re-TikaRa.

See the [LICENSE](LICENSE) file for the full license text.

---

MoonShardOS is an experimental project by [RE-TikaRa](https://github.com/RE-TikaRa).

Built from scratch, one subsystem at a time.
