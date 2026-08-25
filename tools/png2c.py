"""把 RGBA PNG 转换成内核使用的静态像素数组。

内核图片渲染器读取按行排列的 0xAARRGGBB 像素。把转换放在内核之外，
可以避免向 freestanding 内核引入 PNG 解码器和文件系统代码；生成的 C
文件会和其他编译期资源一样进入只读数据段。
"""

from PIL import Image
import sys

# 命令行参数约定为：png2c.py <input.png> <output.c>。
image = Image.open(sys.argv[1]).convert("RGBA")
width, height = image.size

with open(sys.argv[2], "w") as f:
    f.write("#include <stdint.h>\n\n")
    f.write(f"#define IMAGE_WIDTH {width}\n")
    f.write(f"#define IMAGE_HEIGHT {height}\n\n")

    f.write("static const uint32_t image[] = {\n")

    # Pillow 返回 (R, G, B, A)，内核要求打包为 AARRGGBB。
    pixels = image.load()

    # 按 image_draw 使用的 y * width + x 顺序输出每一行。
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            color = (a << 24) | (r << 16) | (g << 8) | b
            f.write(f"0x{color:08X}, ")
        f.write("\n")

    f.write("};\n")
