#include <string.h>

#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"
#include "../font.h"

enum CustomModifiers {
    CTRL = 400,
    SHIFT = 401,
    ALT = 402,
    GUI = 403
};

static void getKeyState(CPU_State *state) {
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    vm_type_signed_t val;
    if (STACK_VALUE(state, 0)->int_value == CTRL) {
        val = keyboard_state[SDL_SCANCODE_LCTRL] || keyboard_state[SDL_SCANCODE_RCTRL];
    } else if (STACK_VALUE(state, 0)->int_value == SHIFT) {
        val = keyboard_state[SDL_SCANCODE_LSHIFT] || keyboard_state[SDL_SCANCODE_RSHIFT];
    } else if (STACK_VALUE(state, 0)->int_value == ALT) {
        val = keyboard_state[SDL_SCANCODE_LALT] || keyboard_state[SDL_SCANCODE_RALT];
    } else if (STACK_VALUE(state, 0)->int_value == GUI) {
        val = keyboard_state[SDL_SCANCODE_LGUI] || keyboard_state[SDL_SCANCODE_RGUI];
    } else {
        val = keyboard_state[STACK_VALUE(state, 0)->int_value];
    }
    state->rr.int_value = val;
    state->rr.type = VM_TYPE_INT;
}

Uint32 *keyStates;
static void getKeyPress(CPU_State *state) {
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);

    if (keyboard_state[STACK_VALUE(state, 0)->int_value] &&
        (!keyStates[STACK_VALUE(state, 0)->int_value]                              /* it isn't pressed */
         || keyStates[STACK_VALUE(state, 0)->int_value] < GetFrameTicks() - 100    /* it hasnt been reported pressed for 100 ms */
         || keyStates[STACK_VALUE(state, 0)->int_value] == GetFrameTicks() + 400   /* it has been reported this frame */
         || keyStates[STACK_VALUE(state, 0)->int_value] == GetFrameTicks()         /* it has been reported this frame */
        ) ) {
        if (keyStates[STACK_VALUE(state, 0)->int_value] == 0) {
            keyStates[STACK_VALUE(state, 0)->int_value] = GetFrameTicks() + 400;
        } else if (keyStates[STACK_VALUE(state, 0)->int_value] < GetFrameTicks()) {
            keyStates[STACK_VALUE(state, 0)->int_value] = GetFrameTicks();
        }
        state->rr.int_value = 1;
        state->rr.type = VM_TYPE_INT;
        return;
    } else {
        state->rr.int_value = 0;
        state->rr.type = VM_TYPE_INT;
    }
}

void resetKeyPress(Uint8 key) {
    keyStates[key] = 0;
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
    keyStates = calloc(1, sizeof(Uint32) * 512);

    register_syscall(state, "getKeyState", getKeyState);
    register_syscall(state, "getKeyPress", getKeyPress);
    register_syscall(state, "getLeftButtonState", getLeftButtonState);
    register_syscall(state, "getRightButtonState", getRightButtonState);
    register_syscall(state, "getMiddleButtonState", getMiddleButtonState);
    register_syscall(state, "getMousePositionX", getMousePositionX);
    register_syscall(state, "getMousePositionY", getMousePositionY);
}
