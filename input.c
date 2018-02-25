//
// Created by Bas du Pré on 18-02-18.
//

#include "input.h"

extern void resetKeyPress(Uint8 key);

fs_input_t input_init() {
    fs_input_t input;

    return input;
}

void input_destroy(fs_input_t *input) {

}

Uint32 mousewheel_y = 0;
Uint32 mousewheel_x = 0;

void input_frame(fs_input_t *input) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        //User requests quit
        if (e.type == SDL_QUIT) {
            //state->quit = 1;
        }
        else if (e.type == SDL_MOUSEWHEEL) {
            mousewheel_y += e.wheel.y * (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1);
            mousewheel_x += e.wheel.x * (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1);
        }
        else if (e.type == SDL_KEYDOWN) {

        }
        else if (e.type == SDL_KEYUP) {
            resetKeyPress(e.key.keysym.scancode);
        }
    }
}

