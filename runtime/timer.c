#include <string.h>

#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"

static void getTicks(CPU_State *state) {
    state->rr.int_value = SDL_GetTicks();
    state->rr.type = VM_TYPE_INT;
}

void register_bindings_timer(CPU_State *state) {
    register_syscall(state, "getTicks", getTicks);
}
