//
// Created by Bas du Pré on 19-02-18.
//

#ifndef FUNKY_SYSTEM_FONT_H
#define FUNKY_SYSTEM_FONT_H

typedef struct fs_glyph_pixel_t {
    int x, y;
} fs_glyph_pixel_t;

typedef struct fs_glyph_t {
    char character;
    int width;
    int height;
    fs_glyph_pixel_t *points;
} fs_glyph_t;

typedef struct fs_font_t {
    const char* name;
    int max_width;
    int height;
    int space_width;
    fs_glyph_t *glyphs;
} fs_font_t;

#endif //FUNKY_SYSTEM_FONT_H
