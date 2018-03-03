#include <string.h>

#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"
#include "../font.h"

#include "Funky5.fnt.h"
#include "Funky7.fnt.h"
#include "Funky9.fnt.h"

static fs_display_t *display = NULL;

struct color_t {
    Uint8 r, g, b, a;
};

static struct color_t colors[] = {
        { .r = 252, .g = 92, .b = 101, .a = 255 }, { .r = 253, .g = 150, .b = 68, .a = 255 }, { .r = 254, .g = 211, .b = 48, .a = 255 },
        { .r = 38, .g = 222, .b = 129, .a = 255 }, { .r = 43, .g = 203, .b = 186, .a = 255 }, { .r = 69, .g = 170, .b = 242, .a = 255 },
        { .r = 75, .g = 123, .b = 236, .a = 255 }, { .r = 165, .g = 94, .b = 234, .a = 255 }, { .r = 235, .g = 59, .b = 90, .a = 255 },
        { .r = 250, .g = 130, .b = 49, .a = 255 }, { .r = 247, .g = 183, .b = 49, .a = 255 }, { .r = 32, .g = 191, .b = 107, .a = 255 },
        { .r = 15, .g = 185, .b = 177, .a = 255 }, { .r = 45, .g = 152, .b = 218, .a = 255 }, { .r = 56, .g = 103, .b = 214, .a = 255 },
        { .r = 136, .g = 84, .b = 208, .a = 255 }, { .r = 186, .g = 36, .b = 62, .a = 255 }, { .r = 255, .g = 204, .b = 204, .a = 255 },
        { .r = 190, .g = 125, .b = 80, .a = 255 }, { .r = 29, .g = 150, .b = 86, .a = 255 }, { .r = 34, .g = 108, .b = 161, .a = 255 },
        { .r = 209, .g = 216, .b = 224, .a = 255 }, { .r = 119, .g = 140, .b = 163, .a = 255 }, { .r = 50, .g = 50, .b = 56, .a = 255 },
        { .r = 129, .g = 26, .b = 44, .a = 255 }, { .r = 255, .g = 184, .b = 184, .a = 255 }, { .r = 143, .g = 90, .b = 54, .a = 255 },
        { .r = 27, .g = 113, .b = 68, .a = 255 }, { .r = 9, .g = 74, .b = 120, .a = 255 }, { .r = 165, .g = 177, .b = 194, .a = 255 },
        { .r = 75, .g = 101, .b = 132, .a = 255 }, { .r = 0, .g = 0, .b = 0, .a = 255 }, { .r = 0, .g = 0, .b = 0, .a = 0 }
};

fs_font_t *current_font = &Funky5;

//   clip([x, y, w, h]) -- set screen clipping region
//   fget(n, [f]) -- get values of sprite flags
//   flip() -- flip screen back buffer (30fps)
//   fset(n, [f], v) -- set values of sprite flags
//   pal(c0, c1, [p]) -- switch colour 0 to colour 1; p = 0 = draw palette; p = 1 = screen palette
//   palt(col, t) -- set transparency for colour to t (bool)
//   print(str, [x, y, [col]]) -- print string
//   sget(x, y) -- get spritesheet pixel colour
//   spr(n, x, y, [w, h], [flip_x], [flip_y]) -- draw sprite
//   sset(x, y, [col]) -- set spritesheet pixel colour
//   sspr(sx, sy, sw, sh, dx, dy, [dw, dh], [flip_x], [flip_y]) -- draw texture from spritesheet

#define SET_COLOR_FOR_CALLS(vm_val, calls) { \
    struct color_t __oldColor; \
    if ((vm_val)->type != VM_TYPE_EMPTY) { \
       SDL_GetRenderDrawColor(display->renderer, &__oldColor.r, &__oldColor.g, &__oldColor.b, &__oldColor.a); \
       struct color_t *__color = &colors[(vm_val)->int_value]; \
       SDL_SetRenderDrawColor(display->renderer, __color->r, __color->g, __color->b, __color->a); \
    } \
    SDL_SetRenderDrawBlendMode(display->renderer, SDL_BLENDMODE_NONE); \
    {calls} \
    SDL_SetRenderDrawBlendMode(display->renderer, SDL_BLENDMODE_BLEND); \
    if ((vm_val)->type != VM_TYPE_EMPTY) \
        SDL_SetRenderDrawColor(display->renderer, __oldColor.r, __oldColor.g, __oldColor.b, __oldColor.a); \
}

static void camera(CPU_State *state) {
    display->camera = (SDL_Point) {
            .x = STACK_VALUE(state, -1)->int_value,
            .y = STACK_VALUE(state, -0)->int_value
    };
}

static void pixel(CPU_State *state) {
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        SDL_RenderDrawPoint(display->renderer,
                            STACK_VALUE(state, -2)->int_value - display->camera.x,
                            STACK_VALUE(state, -1)->int_value - display->camera.y
        );
    });
}

static void getPixel(CPU_State *state) {
    SDL_Rect rect = {
        /* x */ STACK_VALUE(state, -1)->int_value - display->camera.x,
        /* y */ STACK_VALUE(state, -0)->int_value - display->camera.y,
        1, 1
    };
    Uint8 pixel[4];
    SDL_RenderReadPixels(display->renderer, &rect, SDL_PIXELFORMAT_BGRA8888, pixel, sizeof(pixel));
    for (int i = 0; i < 33; i++) {
        if (colors[i].r == pixel[1] && colors[i].g == pixel[2] && colors[i].b == pixel[3] && colors[i].a == pixel[0]) {
            state->rr.int_value = i;
            state->rr.type = VM_TYPE_INT;
            return;
        }
    }
    state->rr.int_value = -1;
    state->rr.type = VM_TYPE_INT;
}

static void rect(CPU_State *state) {
    SDL_Rect rect = {
            /* x */ STACK_VALUE(state, -4)->int_value - display->camera.x,
            /* y */ STACK_VALUE(state, -3)->int_value - display->camera.y,
            /* w */ STACK_VALUE(state, -2)->int_value,
            /* h */ STACK_VALUE(state, -1)->int_value
    };

    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        if (rect.w == 1 && rect.h == 1) {
            SDL_RenderDrawPoint(display->renderer, rect.x, rect.y);
        } else {
            SDL_RenderDrawRect(display->renderer, &rect);
        }
    });
}

static void fillRect(CPU_State *state) {
    SDL_Rect fillRect = {
            /* x */ STACK_VALUE(state, -4)->int_value - display->camera.x,
            /* y */ STACK_VALUE(state, -3)->int_value - display->camera.y,
            /* w */ STACK_VALUE(state, -2)->int_value,
            /* h */ STACK_VALUE(state, -1)->int_value
    };
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        SDL_RenderFillRect(display->renderer, &fillRect);
    });
}

static void setColor(CPU_State *state) {
    struct color_t *color = &colors[STACK_VALUE(state, -0)->int_value];
    SDL_SetRenderDrawColor(display->renderer, color->r, color->g, color->b, color->a);
}

static void cls(CPU_State *state) {
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        SDL_RenderClear(display->renderer);
    });
}

static inline void _drawCircle(int x0, int y0, int radius) {
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawPoint(display->renderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(display->renderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(display->renderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(display->renderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(display->renderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(display->renderer, x0 - y, y0 - x);
        SDL_RenderDrawPoint(display->renderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(display->renderer, x0 + x, y0 - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

static inline void _fillCircle(int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius) - 1) {
                SDL_RenderDrawPoint(display->renderer, x + dx, y + dy);
            }
        }
    }
}

static void circle(CPU_State *state) {
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        _drawCircle(
                /* x */ STACK_VALUE(state, -3)->int_value - display->camera.x,
                /* y */ STACK_VALUE(state, -2)->int_value - display->camera.y,
                /* r */ STACK_VALUE(state, -1)->int_value
        );
    });
}

static void fillCircle(CPU_State *state) {
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        _fillCircle(
                /* x */ STACK_VALUE(state, -3)->int_value - display->camera.x,
                /* y */ STACK_VALUE(state, -2)->int_value - display->camera.y,
                /* r */ STACK_VALUE(state, -1)->int_value
        );
    });
}

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a number */

static void line(CPU_State *state) {
    int x1 = STACK_VALUE(state, -4)->int_value - display->camera.x;
    int y1 = STACK_VALUE(state, -3)->int_value - display->camera.y;
    int x2 = STACK_VALUE(state, -2)->int_value - display->camera.x;
    int y2 = STACK_VALUE(state, -1)->int_value - display->camera.y;

    int dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    dx = x2-x1;      /* the horizontal distance of the line */
    dy = y2-y1;      /* the vertical distance of the line */
    dxabs = abs(dx);
    dyabs = abs(dy);
    sdx = sgn(dx);
    sdy = sgn(dy);
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x1;
    py = y1;

    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        SDL_RenderDrawPoint(display->renderer, px, py);
        if (dxabs >= dyabs) /* the line is more horizontal than vertical */
        {
            for (int i = 0; i < dxabs; i++) {
                y += dyabs;
                if (y >= dxabs) {
                    y -= dxabs;
                    py += sdy;
                }
                px += sdx;
                SDL_RenderDrawPoint(display->renderer, px, py);
            }
        } else /* the line is more vertical than horizontal */
        {
            for (int i = 0; i < dyabs; i++) {
                x += dxabs;
                if (x >= dyabs) {
                    x -= dyabs;
                    px += sdx;
                }
                py += sdy;
                SDL_RenderDrawPoint(display->renderer, px, py);
            }
        }
    });
}

static inline int put_char(char c, int x, int y) {
    if (c == ' ') {
        return current_font->space_width;
    }
    if (c == '\t') {
        return current_font->space_width * 4;
    }
    fs_glyph_t *glyph = &current_font->glyphs[0];
    for (int i = 0; current_font->glyphs[i].character != 0; i++, glyph = &current_font->glyphs[i]) {
        if (glyph->character == c) {
            fs_glyph_pixel_t *pixel = &glyph->points[0];
            for (int j = 0; pixel->x != -1; j++, pixel = &glyph->points[j]) {
                SDL_RenderDrawPoint(display->renderer, x + pixel->x, y + pixel->y);
            }
            return glyph->width;
        }
    }
    return 0;
}

static inline int char_width(char c) {
    if (c == ' ') {
        return current_font->space_width;
    }
    if (c == '\t') {
        return current_font->space_width * 4;
    }
    fs_glyph_t *glyph = &current_font->glyphs[0];
    for (int i = 0; current_font->glyphs[i].character != 0; i++, glyph = &current_font->glyphs[i]) {
        if (glyph->character == c) {
            return glyph->width;
        }
    }
    return 0;
}

static inline int put_char_mono(char c, int x, int y) {
    if (c == ' ') {
        return current_font->max_width;
    }
    if (c == '\t') {
        return current_font->max_width;
    }
    fs_glyph_t *glyph = &current_font->glyphs[0];
    for (int i = 0; current_font->glyphs[i].character != 0; i++, glyph = &current_font->glyphs[i]) {
        if (glyph->character == c) {
            fs_glyph_pixel_t *pixel = &glyph->points[0];
            for (int j = 0; pixel->x != -1; j++, pixel = &glyph->points[j]) {
                int offset = current_font->max_width / 2 - glyph->width / 2;
                SDL_RenderDrawPoint(display->renderer, x + pixel->x + offset, y + pixel->y);
            }
            return current_font->max_width;
        }
    }
    return current_font->max_width;
}

static void text(CPU_State *state) {
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        const char *str = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -1));
        int x = STACK_VALUE(state, -3)->int_value - display->camera.x;
        int y = STACK_VALUE(state, -2)->int_value - display->camera.y;

        int len = strlen(str);
        for (int i = 0; i < len; i++) {
            x += put_char(str[i], x, y) + 1;
        }
    });
}

static void textWidth(CPU_State *state) {
    const char *str = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));

    size_t len = strlen(str);
    unsigned int x = 0;
    for (int i = 0; i < len; i++) {
        x += char_width(str[i]) + 1;
    }
    if (x > 0) x--;

    state->rr = (vm_value_t) { .uint_value = x, .type = VM_TYPE_UINT };
}

static void textMono(CPU_State *state) {
    SET_COLOR_FOR_CALLS(STACK_VALUE(state, 0), {
        const char *str = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -1));
        int x = STACK_VALUE(state, -3)->int_value - display->camera.x;
        int y = STACK_VALUE(state, -2)->int_value - display->camera.y;

        int len = strlen(str);
        for (int i = 0; i < len; i++) {
            x += put_char_mono(str[i], x, y) + 1;
        }
    });
}

static void setFont(CPU_State *state) {
    const char *name = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));
    if (strcmp(name, "7px") == 0) current_font = &Funky7;
    else if (strcmp(name, "5px") == 0) current_font = &Funky5;
    else if (strcmp(name, "9px") == 0) current_font = &Funky9;
    else {
        fprintf(stderr, "Unknown font: %s\n", name);
    }
}

static void clip(CPU_State *state) {
    SDL_Rect rect = {
            /* x */ STACK_VALUE(state, -3)->int_value - display->camera.x,
            /* y */ STACK_VALUE(state, -2)->int_value - display->camera.y,
            /* w */ STACK_VALUE(state, -1)->int_value,
            /* h */ STACK_VALUE(state, -0)->int_value
    };

    if (rect.w * rect.h == 0) {
        SDL_RenderSetClipRect(display->renderer, NULL);
    } else {
        SDL_RenderSetClipRect(display->renderer, &rect);
    }
}

static SDL_Texture** textures = NULL;
static vm_type_t numTextures = 0;

static unsigned int _createTexture() {
    if (textures == NULL) {
        textures = malloc(sizeof(SDL_Texture*) * numTextures);
    }

    textures = realloc(textures, sizeof(SDL_Texture*) * (numTextures + 1));

    textures[numTextures] = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 64);
    SDL_SetTextureBlendMode(textures[numTextures], SDL_BLENDMODE_BLEND);

    return numTextures++;
}

static void createTexture(CPU_State *state) {
    _createTexture();
    VM_RETURN_UINT(state, numTextures - 1);
}

static void destroyTexture(CPU_State *state) {
    SDL_DestroyTexture(textures[STACK_VALUE(state, 0)->uint_value]);
}

static void renderToTexture(CPU_State *state) {
    if (STACK_VALUE(state, 0)->type == VM_TYPE_EMPTY) {
        SDL_SetRenderTarget(display->renderer, NULL);
    } else {
        SDL_SetRenderTarget(display->renderer, textures[STACK_VALUE(state, 0)->uint_value]);
    }
}

static inline double getDoubleFromStack(CPU_State *state, vm_type_signed_t n) {
    return STACK_VALUE(state, n)->type == VM_TYPE_FLOAT ? STACK_VALUE(state, n)->float_value :
           STACK_VALUE(state, n)->type == VM_TYPE_INT ? STACK_VALUE(state, n)->int_value : STACK_VALUE(state, n)->uint_value;
}

static inline int getIntFromStack(CPU_State *state, vm_type_signed_t n) {
    return STACK_VALUE(state, n)->type == VM_TYPE_FLOAT ? (int)STACK_VALUE(state, n)->float_value :
           STACK_VALUE(state, n)->type == VM_TYPE_INT ? STACK_VALUE(state, n)->int_value : (int)STACK_VALUE(state, n)->uint_value;
}

static void spriteEx(CPU_State *state) {
    // n, x, y, src_x, src_y, src_w, src_h, scale_x, scale_y, flip_x, flip_y, angle, rot_x, rot_y

    SDL_Texture* sheet = textures[STACK_VALUE(state, -13)->uint_value];
    int src_x = getIntFromStack(state, -12);
    int src_y = getIntFromStack(state, -11);
    int x = getIntFromStack(state, -10);
    int y = getIntFromStack(state, -9);
    int src_w = getIntFromStack(state, -8);
    int src_h = getIntFromStack(state, -7);
    double scale_x = getDoubleFromStack(state, -6);
    double scale_y = getDoubleFromStack(state, -5);
    int flip_x = getIntFromStack(state, -4);
    int flip_y = getIntFromStack(state, -3);
    double angle = getDoubleFromStack(state, -2);
    int rot_c_x = getIntFromStack(state, -1);
    int rot_c_y = getIntFromStack(state, 0);

    SDL_Rect srcrect, dstrect;
    SDL_Point center;
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (flip_x) flip |= SDL_FLIP_VERTICAL;
    if (flip_y) flip |= SDL_FLIP_HORIZONTAL;

    center.x = rot_c_x;
    center.y = rot_c_y;

    srcrect.x = src_x;
    srcrect.y = src_y;
    srcrect.w = src_w;
    srcrect.h = src_h;

    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = (int) (src_w * scale_x);
    dstrect.h = (int) (src_h * scale_y);

    if (STACK_VALUE(state, 1)->type == VM_TYPE_EMPTY || STACK_VALUE(state, 0)->type == VM_TYPE_EMPTY) {
        SDL_RenderCopyEx(display->renderer, sheet, &srcrect, &dstrect, angle, NULL, flip);
    } else {
        SDL_RenderCopyEx(display->renderer, sheet, &srcrect, &dstrect, angle, &center, flip);
    }
}

static void sprite(CPU_State *state) {
    // n, x, y, src_x, src_y, src_w, src_h, scale_x, scale_y

    SDL_Texture* sheet = textures[STACK_VALUE(state, -8)->uint_value];
    int src_x = getIntFromStack(state, -7);
    int src_y = getIntFromStack(state, -6);
    int x = getIntFromStack(state, -5);
    int y = getIntFromStack(state, -4);
    int src_w = getIntFromStack(state, -3);
    int src_h = getIntFromStack(state, -2);
    double scale_x = getDoubleFromStack(state, -1);
    double scale_y = getDoubleFromStack(state, -0);

    SDL_Rect srcrect, dstrect;

    srcrect.x = src_x;
    srcrect.y = src_y;
    srcrect.w = src_w;
    srcrect.h = src_h;

    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = (int) (src_w * scale_x);
    dstrect.h = (int) (src_h * scale_y);

    SDL_RenderCopy(display->renderer, sheet, &srcrect, &dstrect);
}

Uint32 get_pixel32(SDL_Surface *surface, int x, int y) {
    //Convert pixels to 32 bits
    Uint32 *pixels = (Uint32*)surface->pixels;

    return pixels[(y*surface->w) + x];
}

static void serializeTexture(CPU_State *state) {
    SDL_Texture* sheet = textures[STACK_VALUE(state, 0)->uint_value];
    char *serialize = malloc(64 * 64 + 1);

    SDL_Surface *surface;
    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    surface = SDL_CreateRGBSurface(0, 64, 64, 32, rmask, gmask, bmask, amask);
    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }
    SDL_SetRenderTarget(display->renderer, sheet);
    SDL_RenderReadPixels(display->renderer, NULL, SDL_PIXELFORMAT_BGRA8888, surface->pixels, surface->pitch);
    SDL_SetRenderTarget(display->renderer, NULL);

    for (int x = 0; x < 64; x++) for (int y = 0; y < 64; y++) {
        Uint8 pixel[4];
        Uint32 pixelval = get_pixel32(surface, x, y);
        memcpy(pixel, &pixelval, 4);
        for (char i = 0; i < 33; i++) {
            if (colors[i].r == pixel[1] && colors[i].g == pixel[2] && colors[i].b == pixel[3] && colors[i].a == pixel[0]) {
                serialize[y * 64 + x] = (char)(56 + i);
            }
        }
    }

    vm_value_t val = vm_create_string(state, serialize);
    free(serialize);

    VM_RETURN(state, val);
}

static void deserializeTexture(CPU_State *state) {
    const char *contents = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));

    vm_type_t index = _createTexture();
    size_t len = strlen(contents);

    SDL_SetRenderTarget(display->renderer, textures[index]);

    SDL_SetRenderDrawBlendMode(display->renderer, SDL_BLENDMODE_NONE);
    for (int i = 0; i < len; i++) {
        struct color_t *color = &colors[contents[i] - 56];
        SDL_SetRenderDrawColor(display->renderer, color->r, color->g, color->b, color->a);
        SDL_RenderDrawPoint(display->renderer, i % 64, i / 64);
    }
    SDL_SetRenderDrawBlendMode(display->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(display->renderer, NULL);

    VM_RETURN_UINT(state, index);
}

void register_bindings_draw(CPU_State *state) {
    display = get_display();
    register_syscall(state, "camera", camera);
    register_syscall(state, "rect", rect);
    register_syscall(state, "fillRect", fillRect);
    register_syscall(state, "setColor", setColor);
    register_syscall(state, "cls", cls);
    register_syscall(state, "circle", circle);
    register_syscall(state, "fillCircle", fillCircle);
    register_syscall(state, "line", line);
    register_syscall(state, "pixel", pixel);
    register_syscall(state, "getPixel", getPixel);
    register_syscall(state, "text", text);
    register_syscall(state, "textWidth", textWidth);
    register_syscall(state, "textMono", textMono);
    register_syscall(state, "setFont", setFont);
    register_syscall(state, "clip", clip);

    register_syscall(state, "createTexture", createTexture);
    register_syscall(state, "destroyTexture", destroyTexture);
    register_syscall(state, "renderToTexture", renderToTexture);
    register_syscall(state, "spriteEx", spriteEx);
    register_syscall(state, "sprite", sprite);
    register_syscall(state, "serializeTexture", serializeTexture);
    register_syscall(state, "deserializeTexture", deserializeTexture);
}
