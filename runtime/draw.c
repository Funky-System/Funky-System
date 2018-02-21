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
    Uint8 r, g, b;
};

static struct color_t colors[] = {
        { .r = 252, .g = 92, .b = 101 }, { .r = 253, .g = 150, .b = 68 }, { .r = 254, .g = 211, .b = 48 },
        { .r = 38, .g = 222, .b = 129 }, { .r = 43, .g = 203, .b = 186 }, { .r = 69, .g = 170, .b = 242 },
        { .r = 75, .g = 123, .b = 236 }, { .r = 165, .g = 94, .b = 234 }, { .r = 235, .g = 59, .b = 90 },
        { .r = 250, .g = 130, .b = 49 }, { .r = 247, .g = 183, .b = 49 }, { .r = 32, .g = 191, .b = 107 },
        { .r = 15, .g = 185, .b = 177 }, { .r = 45, .g = 152, .b = 218 }, { .r = 56, .g = 103, .b = 214 },
        { .r = 136, .g = 84, .b = 208 }, { .r = 186, .g = 36, .b = 62 }, { .r = 255, .g = 204, .b = 204 },
        { .r = 190, .g = 125, .b = 80 }, { .r = 29, .g = 150, .b = 86 }, { .r = 34, .g = 108, .b = 161 },
        { .r = 209, .g = 216, .b = 224 }, { .r = 119, .g = 140, .b = 163 }, { .r = 50, .g = 61, .b = 74 },
        { .r = 129, .g = 26, .b = 44 }, { .r = 255, .g = 184, .b = 184 }, { .r = 143, .g = 90, .b = 54 },
        { .r = 27, .g = 113, .b = 68 }, { .r = 9, .g = 74, .b = 120 }, { .r = 165, .g = 177, .b = 194 },
        { .r = 75, .g = 101, .b = 132 }, { .r = 0, .g = 0, .b = 0 }
};

fs_font_t *current_font = &Funky5;

//   clip([x, y, w, h]) -- set screen clipping region
//   cursor(x, y) -- set cursor and CR/LF margin position
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
       Uint8 __a; \
       SDL_GetRenderDrawColor(display->renderer, &__oldColor.r, &__oldColor.g, &__oldColor.b, &__a); \
       struct color_t *__color = &colors[(vm_val)->int_value]; \
       SDL_SetRenderDrawColor(display->renderer, __color->r, __color->g, __color->b, 255); \
    } \
    {calls} \
    if ((vm_val)->type != VM_TYPE_EMPTY) \
        SDL_SetRenderDrawColor(display->renderer, __oldColor.r, __oldColor.g, __oldColor.b, 255); \
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
    Uint8 pixel[3];
    SDL_RenderReadPixels(display->renderer, &rect, SDL_PIXELFORMAT_BGR888, pixel, sizeof(pixel));
    for (int i = 0; i < 32; i++) {
        if (colors[i].r == pixel[0] && colors[i].g == pixel[1] && colors[i].b == pixel[2]) {
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
        SDL_RenderDrawRect(display->renderer, &rect);
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
    SDL_SetRenderDrawColor(display->renderer, color->r, color->g, color->b, 255);
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
    int x1 = STACK_VALUE(state, -4)->int_value - 1 - display->camera.x;
    int y1 = STACK_VALUE(state, -3)->int_value - display->camera.y;
    int x2 = STACK_VALUE(state, -2)->int_value - 1 - display->camera.x;
    int y2 = STACK_VALUE(state, -1)->int_value- display->camera.y;

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
    register_syscall(state, "textMono", textMono);
    register_syscall(state, "setFont", setFont);
}
