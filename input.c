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

        else if(e.type == SDL_KEYDOWN)
        {
            //Select surfaces based on key press
            /*switch(e.key.keysym.sym)
            {
                case SDLK_UP:
                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                    break;

                case SDLK_DOWN:
                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                    break;

                case SDLK_LEFT:
                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
                    break;

                case SDLK_RIGHT:
                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
                    break;

                default:
                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                    break;
            }*/
        }
    }
}

