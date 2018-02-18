//
// Created by Bas du Pré on 18-02-18.
//

#include <SDL2/SDL.h>

#include "display.h"

const int scale = 3;
const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 192;

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

    display.renderer = SDL_CreateRenderer(display.window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(display.renderer, scale, scale);
    SDL_RenderSetLogicalSize(display.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(display.renderer, SDL_TRUE);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, 0);
    if(display.renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return display;
}

void display_destroy(fs_display_t *display) {
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);

    SDL_Quit();
}

void display_frame(fs_display_t *display) {
    /*
    // Clear screen
    SDL_SetRenderDrawColor(display->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(display->renderer);
    //Render red filled quad
    SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    SDL_SetRenderDrawColor( display->renderer, 0xFF, 0x00, 0x00, 0xFF );
    SDL_RenderFillRect( display->renderer, &fillRect );

    //Render green outlined quad
    SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
    SDL_SetRenderDrawColor( display->renderer, 0x00, 0xFF, 0x00, 0xFF );
    SDL_RenderDrawRect( display->renderer, &outlineRect );

    //Draw blue horizontal line
    SDL_SetRenderDrawColor( display->renderer, 0x00, 0x00, 0xFF, 0xFF );
    SDL_RenderDrawLine( display->renderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

    //Draw vertical line of yellow dots
    SDL_SetRenderDrawColor( display->renderer, 0xFF, 0xFF, 0x00, 0xFF );
    for( int i = 0; i < SCREEN_HEIGHT; i += 4 )
    {
        SDL_RenderDrawPoint( display->renderer, SCREEN_WIDTH / 2, i );
    }
    */

    //Update screen
    SDL_RenderPresent(display->renderer);
}