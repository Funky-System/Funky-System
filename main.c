#include <stdio.h>
#include <SDL2/SDL.h>

#include <funkyvm/funkyvm.h>
#include <funkyvm/memory.h>

#ifdef FUNKY_VM_OS_EMSCRIPTEN
#include <emscripten/emscripten.h>
#pragma pack(1)
#endif

#include "funky_system.h"
#include "display.h"
#include "input.h"

static fs_display_t display;
static fs_input_t input;

void debug_print(CPU_State *state) {
    const char* str = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));
    printf("%s", str);
}

Uint32 fps_cap = 60;

Uint32 prev_ticks = 0;
Uint32 tmr = 0;
Uint32 num_frames = 0;
float overhead = 0;
float target_fps = 60;

extern int vm_emscripten_running;

void pump_events(CPU_State *state) {
    num_frames++;
    input_frame(&input);
    display_frame(&display);

    Uint32 frameTicks = SDL_GetTicks() - prev_ticks;

    // FPS cap
    overhead += (1000.0/target_fps) - (float)frameTicks;
    if (frameTicks < (1000.0 / target_fps))
    {
        //Wait remaining time
        SDL_Delay((Uint32)((1000.0/target_fps) - (double)frameTicks));
    }

    tmr += SDL_GetTicks() - prev_ticks;
    prev_ticks = SDL_GetTicks();

    // FPS calculation
    if (tmr >= 5000) {
        printf("FPS: %.2f, load: %.2f%%\n", num_frames / (tmr / 1000.0), (((float)tmr - overhead) / (float)tmr) * 100.0);
        tmr = 0;
        overhead = 0;
        num_frames = 0;
    }

    #ifdef FUNKY_VM_OS_EMSCRIPTEN
        cpu_emscripten_yield(state);
    #endif
}

fs_display_t *get_display() { return &display; }
fs_input_t *get_input() { return &input; }


static void run(void *arg) {
    #ifdef FUNKY_VM_OS_EMSCRIPTEN
    emscripten_cancel_main_loop();
    #endif
    display = display_init();
    input = input_init();

    vm_type_t ret = cpu_run((CPU_State*)arg);
}

static Memory memory;
static CPU_State cpu_state;

int main() {
    #if defined(VM_NATIVE_MALLOC) && VM_NATIVE_MALLOC
        unsigned char *main_memory = 0;
    #else
        unsigned char *main_memory = malloc(VM_MEMORY_LIMIT);
    #endif
    memory_init(&memory, main_memory);

    cpu_state = cpu_init(&memory);

    Module module = module_load_name(&cpu_state, "kernel");
    module_register(&cpu_state, module);

    cpu_set_entry_to_module(&cpu_state, &module);

    register_syscall(&cpu_state, "debug_print", debug_print);
    register_syscall(&cpu_state, "pump_events", pump_events);
    register_bindings_draw(&cpu_state);
    register_bindings_input(&cpu_state);
    register_bindings_timer(&cpu_state);

    #ifdef FUNKY_VM_OS_EMSCRIPTEN
        emscripten_set_main_loop_arg(&run, &cpu_state, 0, 0);
    #else
        run(&cpu_state);

        input_destroy(&input);
        display_destroy(&display);

        free(main_memory);
        memory_destroy(&memory);
        cpu_destroy(&cpu_state);

        return 0;
    #endif

}
