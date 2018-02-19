import fnmatch
import os
import re

fontsdir = os.path.dirname(os.path.abspath(__file__))


def process(source_file):
    input = open(source_file, "r")

    name = input.readline().strip()

    output = open(name + ".fnt.h", "w")
    output.write("#include \"../font.h\"\n\n")

    width = int(input.readline().strip())
    height = int(input.readline().strip())
    space_width = int(input.readline().strip())

    output.write("fs_font_t " + name + " = (fs_font_t) {\n")
    output.write("    .name = \"" + name + "\",\n")
    output.write("    .max_width = " + str(width) + ",\n")
    output.write("    .height = " + str(height) + ",\n")
    output.write("    .space_width = " + str(space_width) + ",\n")

    output.write("    .glyphs  = (fs_glyph_t[]) {\n")

    eof = False
    line = 3
    while not eof:
        char = ""
        num_emptylines = -1
        while char == "":
            num_emptylines += 1
            if num_emptylines > 100:
                eof = True
                break # eof
            char = input.readline()
            line += 1
            char = char.strip()

        if eof:
            break

        if num_emptylines < 1:
            print "Empty line expected at line " + str(line)
            exit(1)

        rows = [""] * height
        charwidth = 0
        for i in range(0, height):
            rows[i] = input.readline().rstrip()
            line += 1
            if len(rows[i]) > charwidth:
                charwidth = len(rows[i])

        # print "char " + char + " is width " + str(charwidth)
        if char == "'":
            char = "\\'"
        if char == "\\":
            char = "\\\\"

        output.write("        {\n")
        output.write("            .width = " + str(charwidth) + ",\n")
        output.write("            .height = " + str(height) + ",\n")
        output.write("            .character = '" + str(char) + "',\n")
        output.write("            .points = (fs_glyph_pixel_t[]) {\n")
        for y in range(0, height):
            for x, c in enumerate(rows[y]):
                if c != " ":
                    output.write("                { .x = " + str(x) + ", .y = " + str(y) + " },\n")
        output.write("                {-1}\n")
        output.write("            }\n")
        output.write("        },\n")

    output.write("        {0}\n")
    output.write("    }\n")
    output.write("};\n")

    input.close()
    output.close()


source_files = []
for root, dirnames, filenames in os.walk(fontsdir):
    for filename in fnmatch.filter(filenames, '*.fnt'):
        source_files.append(os.path.join(root, filename))

for source_file in source_files:
    process(source_file)
