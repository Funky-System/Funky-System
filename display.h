//
// Created by Bas du Pré on 18-02-18.
//

#ifndef FUNKY_SYSTEM_DISPLAY_H
#define FUNKY_SYSTEM_DISPLAY_H

#include <SDL.h>

void initNativeWindow();

typedef struct fs_display_t {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Point camera;

    int scale;
} fs_display_t;

fs_display_t display_init();
void display_destroy(fs_display_t *display);
void display_frame(fs_display_t *display);
void display_set_scale(fs_display_t* display, int scale);

#endif //FUNKY_SYSTEM_DISPLAY_H
