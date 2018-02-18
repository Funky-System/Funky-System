//
// Created by Bas du Pré on 18-02-18.
//

#ifndef FUNKY_SYSTEM_FUNKY_SYSTEM_H
#define FUNKY_SYSTEM_FUNKY_SYSTEM_H

#include <funkyvm/funkyvm.h>
#include "display.h"
#include "input.h"

void register_bindings_draw(CPU_State *state);

fs_display_t *get_display();
fs_input_t *get_input();

#endif //FUNKY_SYSTEM_FUNKY_SYSTEM_H
