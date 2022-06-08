//
// Created by Bas du Pré on 18-02-18.
//

#include <SDL.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"

const int INITIAL_SCALE = 3;
const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 192;

static SDL_Cursor *init_system_cursor(const char *image[]);
static const char *arrow[32];

extern void initNativeWindow();

void display_set_scale(fs_display_t* display, int scale) {
    display->scale = scale;
    SDL_SetWindowSize(display->window, SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale);
    SDL_RenderSetScale(display->renderer, scale, scale);
}

fs_display_t display_init() {
    fs_display_t display = { 0 };

    display.scale = INITIAL_SCALE;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //Create window
    display.window = SDL_CreateWindow("Funky System",
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      SCREEN_WIDTH * INITIAL_SCALE, SCREEN_HEIGHT * INITIAL_SCALE, SDL_WINDOW_SHOWN);
    if (display.window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    display.renderer = SDL_CreateRenderer(display.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(display.renderer, INITIAL_SCALE, INITIAL_SCALE);
    SDL_SetRenderDrawBlendMode(display.renderer, SDL_BLENDMODE_BLEND);
    //SDL_RenderSetLogicalSize(display.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    //SDL_RenderSetIntegerScale(display.renderer, SDL_TRUE);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, 0);
    if(display.renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    display.camera = (SDL_Point) { .x = 0, .y = 0 };

    SDL_SetCursor(init_system_cursor(arrow));

    return display;
}

void display_destroy(fs_display_t *display) {
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);

    SDL_Quit();
}

void display_frame(fs_display_t *display) {
    SDL_RenderPresent(display->renderer);
}

/* XPM */
static const char *arrow[32] = {
        /* width height num_colors chars_per_pixel */
        "    26    26        3            1",
        /* colors */
        ". c #000000",
        "X c #ffffff",
        "o c None",
        /* pixels */
        "..oooooooooooooooooooooooo",
        "..oooooooooooooooooooooooo",
        "....oooooooooooooooooooooo",
        "....oooooooooooooooooooooo",
        "..XX..oooooooooooooooooooo",
        "..XX..oooooooooooooooooooo",
        "..XXXX..oooooooooooooooooo",
        "..XXXX..oooooooooooooooooo",
        "..XXXXXX..oooooooooooooooo",
        "..XXXXXX..oooooooooooooooo",
        "..XXXXXXXX..oooooooooooooo",
        "..XXXXXXXX..oooooooooooooo",
        "..XXXXXXXXXX..oooooooooooo",
        "..XXXXXXXXXX..oooooooooooo",
        "..XXXXXXXXXXXX..oooooooooo",
        "..XXXXXXXXXXXX..oooooooooo",
        "..XXXXXXXXXXXXXX..oooooooo",
        "..XXXXXXXXXXXXXX..oooooooo",
        "..XXXXXX..........oooooooo",
        "..XXXXXX..........oooooooo",
        "..XXXX..oooooooooooooooooo",
        "..XXXX..oooooooooooooooooo",
        "..XX..oooooooooooooooooooo",
        "..XX..oooooooooooooooooooo",
        "....oooooooooooooooooooooo",
        "....oooooooooooooooooooooo",
        "0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
    int i, row, col;
    Uint8 data[4*26];
    Uint8 mask[4*26];
    int hot_x, hot_y;

    i = -1;
    for (row=0; row<26; ++row) {
        for (col=0; col<26; ++col) {
            if (col % 8) {
                data[i] <<= 1;
                mask[i] <<= 1;
            } else {
                ++i;
                data[i] = mask[i] = 0;
            }
            switch (image[4+row][col]) {
                case '.':
                    data[i] |= 0x01;
                    mask[i] |= 0x01;
                    break;
                case 'X':
                    mask[i] |= 0x01;
                    break;
                case ' ':
                    break;
            }
        }
    }
    sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
    return SDL_CreateCursor(data, mask, 26, 26, hot_x, hot_y);
}
