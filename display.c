//
// Created by Bas du Pré on 18-02-18.
//

#include <SDL.h>

#include "display.h"
#include "font.h"

const int scale = 3;
const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 192;

fs_font_t f = (fs_font_t) {
        .name = "hoi",
        .max_width = 5,
        .height = 5,
        .glyphs = (fs_glyph_t[]) {
                {
                        .width = 2,
                        .height = 5,
                        .character = 'a',
                        .points = (fs_glyph_pixel_t[]) {
                                { .x = 1, .y = 4 },
                                { .x = 1, .y = 2 },
                        }
                }
        }
};

fs_display_t display_init() {
    fs_display_t display = { 0 };

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //Create window
    display.window = SDL_CreateWindow("Funky System",
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale, SDL_WINDOW_SHOWN);
    if (display.window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    display.renderer = SDL_CreateRenderer(display.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(display.renderer, scale, scale);
    //SDL_RenderSetLogicalSize(display.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    //SDL_RenderSetIntegerScale(display.renderer, SDL_TRUE);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, 0);
    if(display.renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    display.camera = (SDL_Point) { .x = 0, .y = 0 };

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