#include <string.h>

#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"

static void getClipboard(CPU_State *state) {
    state->rr = vm_create_string(state, SDL_GetClipboardText());
}

static void setClipboard(CPU_State *state) {
    const char* str = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));
    SDL_SetClipboardText(str);
}

void register_bindings_clipboard(CPU_State *state) {
    register_syscall(state, "getClipboard", getClipboard);
    register_syscall(state, "setClipboard", setClipboard);
}
