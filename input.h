//
// Created by Bas du Pré on 18-02-18.
//

#ifndef FUNKY_SYSTEM_INPUT_H
#define FUNKY_SYSTEM_INPUT_H

#include <SDL2/SDL.h>

typedef struct fs_input_t {

} fs_input_t;

fs_input_t input_init();
void input_destroy(fs_input_t *display);
void input_frame(fs_input_t *display, CPU_State* state);

extern Uint32 mousewheel_y;
extern Uint32 mousewheel_x;

#endif //FUNKY_SYSTEM_INPUT_H
