//
// Created by Bas du Pré on 18-02-18.
//

#include "input.h"

fs_input_t input_init() {
    fs_input_t input;

    return input;
}

void input_destroy(fs_input_t *input) {

}

void input_frame(fs_input_t *input) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        //User requests quit
        if (e.type == SDL_QUIT) {
            //state->quit = 1;
        }
    }
}

