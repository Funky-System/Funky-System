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

static void getLeftButtonState(CPU_State *state) {
    state->rr.int_value = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) ? 1 : 0;
    state->rr.type = VM_TYPE_INT;
}

static void getRightButtonState(CPU_State *state) {
    state->rr.int_value = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT) ? 1 : 0;
    state->rr.type = VM_TYPE_INT;
}

static void getMiddleButtonState(CPU_State *state) {
    state->rr.int_value = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE) ? 1 : 0;
    state->rr.type = VM_TYPE_INT;
}

static void getMousePositionX(CPU_State *state) {
    SDL_GetMouseState(&state->rr.int_value, NULL);
    state->rr.int_value /= get_display()->scale;
    state->rr.type = VM_TYPE_INT;
}

static void getMousePositionY(CPU_State *state) {
    SDL_GetMouseState(NULL, &state->rr.int_value);
    state->rr.int_value /= get_display()->scale;
    state->rr.type = VM_TYPE_INT;
}

void register_bindings_input(CPU_State *state) {
    register_syscall(state, "getKeyState", getKeyState);
    register_syscall(state, "getLeftButtonState", getLeftButtonState);
    register_syscall(state, "getRightButtonState", getRightButtonState);
    register_syscall(state, "getMiddleButtonState", getMiddleButtonState);
    register_syscall(state, "getMousePositionX", getMousePositionX);
    register_syscall(state, "getMousePositionY", getMousePositionY);
}
