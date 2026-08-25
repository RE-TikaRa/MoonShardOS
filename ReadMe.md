# MoonShardOS

[![许可证](https://img.shields.io/badge/许可证-Apache--2.0-blue.svg)](LICENSE)

**中文 | [English](ReadMe_EN.md)**

> 对象万岁！ ~~一切皆对象（其实是照着Linux写的啊哈哈哈哈哈）~~

MoonShardOS 是一个面向 `x86_64` 的实验性操作系统项目，使用 C 编写内核，并通过 Limine 完成启动。

项目从底层硬件初始化开始，逐步建立自己的图形、Console、内存、设备、运行时和对象系统，并面向嵌入式设备、机器人以及专用计算设备探索系统设计。

## 当前状态

目前已经具备：

* `x86_64` 内核
* Limine 启动
* Framebuffer 初始化
* 动态 RGB 颜色转换
* 单像素读写
* RGBA 图片显示与 Alpha 混合
* `8×8` 位图字体
* ASCII `0x20 ~ 0x7E` 字符集
* Console 基础输出
* 换行与自动换行
* GDT 初始化
* IDT 初始化
* 0~31 号 CPU 异常入口与基础处理
* ISO 镜像生成
* QEMU 启动支持

当前启动流程：

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
IDT / CPU Exception
```

启动界面使用独立的图形背景、Logo 和标题文字，随后进入内核 Console。

## 项目结构

```text
MoonShardOS/
├── iso/                 # ISO 文件系统
├── limine/              # Limine 启动器子模块
├── assets/
│   └── logo.png         # Logo 源图片
├── src/
│   ├── kernel.c         # 内核入口
│   ├── logo.c            # 由 logo.png 生成的静态像素数组
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
├── build.sh             # 构建脚本
├── tools/png2c.py       # PNG 到 C 像素数组转换工具
├── linker.ld            # 内核链接脚本
├── .gitignore
└── .gitmodules
```

## 图形系统

MoonShardOS 使用 Limine 提供的 framebuffer。

Framebuffer 模块负责：

```text
Framebuffer
├── 初始化
├── RGB 颜色转换
├── 清屏
├── 像素读取
└── 像素写入
```

颜色转换根据 framebuffer 提供的 RGB mask 动态完成，不依赖固定的像素排列。

图片模块使用 `AARRGGBB` 像素数据，并支持 Alpha 混合：

```text
AARRGGBB
    ↓
Alpha 混合
    ↓
Framebuffer 颜色
    ↓
像素输出
```

## 字体系统

当前字体使用 `8×8` 位图字模，字符范围：

```text
0x20 ' '
    ↓
0x7E '~'
```

包含：

```text
A-Z
a-z
0-9
标点
常用符号
```

字体绘制流程：

```text
字符
 ↓
位图字模
 ↓
像素判断
 ↓
framebuffer_put_pixel()
```

后续可以继续扩展更大的字体、Unicode 和 CJK 字库。

## Console

Console 建立在字体和 framebuffer 之上，用于提供内核文字输出。

当前接口：

```c
console_init();
console_clear();
console_putchar();
console_write();
console_set_color();
```

当前支持：

```text
换行
回车
自动换行
屏幕边缘处理
```

输出关系：

```text
console_write()
      ↓
console_putchar()
      ↓
font_draw_char()
      ↓
framebuffer_put_pixel()
```

后续 Console 将逐渐成为系统终端，并作为 Shell 的文字输出基础。

## CPU

当前已经加入基础 GDT、IDT 和 CPU 异常处理。

GDT 包含：

```text
GDT[0]
    空描述符

GDT[1]
    内核代码段
    选择子：0x08

GDT[2]
    内核数据段
    选择子：0x10
```

CPU 初始化流程：

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

IDT 当前安装 CPU 保留的 `0~31` 号异常。`src/cpu/isr.S` 为没有错误码的异常补入 `0`，统一保存异常向量、错误码和通用寄存器，再调用 `src/cpu/isr.c` 中的 C 层处理函数。

Page Fault 通过 `CR2` 读取触发异常的线性地址。当前异常处理器输出提示后停在 `hlt` 循环中；硬件中断、定时器和 TSS 尚未加入。

## 架构方向

MoonShardOS 不以复刻现有桌面操作系统为目标。

项目关注：

```text
硬件
 ↓
内核
 ↓
运行时
 ↓
对象
 ↓
应用程序
```

长期希望建立适用于嵌入式设备、机器人和专用计算设备的轻量系统基础。

## 构建

在 Linux 环境中进入项目目录：

```bash
./build.sh
```

构建流程：

```text
编译内核源文件
      ↓
链接 kernel.elf
      ↓
更新 ISO 文件系统
      ↓
生成 MoonShardOS.iso
      ↓
安装 Limine 启动信息
```

## 使用 QEMU 运行

构建完成后：

```bash
qemu-system-x86_64 \
    -cdrom MoonShardOS.iso \
    -m 256M \
    -vga std
```

Logo 源图片变更后，可以重新生成内核使用的像素数组：

```bash
python3 tools/png2c.py assets/logo.png src/logo.c
```

## 路线图

### 启动与图形

* [x] 内核启动
* [x] Limine 集成
* [x] Framebuffer
* [x] 基础图形绘制
* [x] 图片渲染
* [x] Alpha 混合
* [x] 位图字体
* [x] ASCII 字库

### Console

* [x] 基础文字输出
* [x] 字符绘制
* [x] 换行
* [x] 自动换行
* [ ] 光标
* [ ] 滚屏
* [ ] 输入处理

### CPU

* [x] GDT
* [x] IDT
* [x] CPU 异常处理
* [ ] 中断处理
* [ ] 定时器
* [ ] TSS

### 内存

* [ ] 内存布局解析
* [ ] 物理内存管理
* [ ] 堆分配器
* [ ] 虚拟内存
* [ ] 页分配器

### 内核

* [ ] 调度器
* [ ] 任务抽象
* [ ] 设备抽象
* [ ] 内核对象系统

### 硬件

* [ ] 键盘
* [ ] 存储
* [ ] 串口
* [ ] 网络
* [ ] 通用设备接口

### 运行时

* [ ] Runtime
* [ ] 对象系统
* [ ] 应用程序接口
* [ ] Shell

### 目标平台

* [ ] 嵌入式设备
* [ ] ARM64
* [ ] 微控制器方向运行时
* [ ] 机器人相关 API
* [ ] 专用硬件平台

## 许可证

MoonShardOS 使用 Apache License 2.0 许可证。

Copyright 2026 亓翎_Re-TikaRa.

完整许可证文本见 [LICENSE](LICENSE)。

---

MoonShardOS 是 [RE-TikaRa](https://github.com/RE-TikaRa) 的实验性项目。

只是一时兴起罢了。

**中文 | [English](README_EN.md)**
