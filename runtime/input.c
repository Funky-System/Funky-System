#include <string.h>

#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"
#include "../font.h"

static void getKeyState(CPU_State *state) {
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    state->rr.int_value = keyboard_state[STACK_VALUE(state, 0)->int_value];
    state->rr.type = VM_TYPE_INT;
}


void register_bindings_input(CPU_State *state) {
    register_syscall(state, "getButtonState", getKeyState);
}
