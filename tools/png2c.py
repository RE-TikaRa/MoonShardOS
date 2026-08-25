from PIL import Image
import sys

image = Image.open(sys.argv[1]).convert("RGBA")
width, height = image.size

with open(sys.argv[2], "w") as f:
    f.write("#include <stdint.h>\n\n")
    f.write(f"#define IMAGE_WIDTH {width}\n")
    f.write(f"#define IMAGE_HEIGHT {height}\n\n")

    f.write("static const uint32_t image[] = {\n")

    pixels = image.load()

    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            color = (a << 24) | (r << 16) | (g << 8) | b
            f.write(f"0x{color:08X}, ")
        f.write("\n")

    f.write("};\n")