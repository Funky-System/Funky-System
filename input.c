//
// Created by Bas du Pré on 18-02-18.
//

#include <funkyvm/cpu.h>
#include "input.h"

extern void resetKeyPress(Uint8 key);
void registerKeyDown(Uint8 key, Uint32 timestamp);

fs_input_t input_init() {
    fs_input_t input = {0};

    return input;
}

void input_destroy(fs_input_t *input) {

}

Uint32 mousewheel_y = 0;
Uint32 mousewheel_x = 0;

void input_frame(fs_input_t *input, CPU_State* state) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        //User requests quit
        if (e.type == SDL_QUIT) {
            state->running = 0;
        }
        else if (e.type == SDL_MOUSEWHEEL) {
            mousewheel_y += e.wheel.y * (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1);
            mousewheel_x += e.wheel.x * (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1);
        }
        else if (e.type == SDL_KEYDOWN) {
            registerKeyDown(e.key.keysym.scancode, e.common.timestamp);
        }
        else if (e.type == SDL_KEYUP) {
            resetKeyPress(e.key.keysym.scancode);
        }

        else if (e.type == SDL_SYSWMEVENT) {
            platform_handle_event(&e, state);
        }
    }
}

