#include <stdio.h>
#include <SDL.h>

#include <funkyvm/funkyvm.h>

#include "funky_system.h"
#include "display.h"
#include "input.h"

static fs_display_t display;
static fs_input_t input;

void debug_print(CPU_State *state) {
    const char* str = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));
    printf("%s", str);
}

Uint32 prev_ticks = 0;
Uint32 tmr = 0;
Uint32 num_frames = 0;
Uint32 overhead = 0;
void pump_events(CPU_State *state) {
    Uint32 start_ticks = SDL_GetTicks();
    tmr += start_ticks - prev_ticks;
    prev_ticks = start_ticks;
    if (tmr >= 5000) {
        printf("FPS: %.2f, load: %.2f%%\n", num_frames / 5.0, ((5000.0 - overhead) / 5000.0) * 100.0);
        tmr = 0;
        overhead = 0;
        num_frames = 0;
    }
    num_frames++;
    input_frame(&input);
    display_frame(&display);

    Uint32 frameTicks = SDL_GetTicks() - start_ticks;
    if (frameTicks < (1000.0 / 60.0))
    {
        //Wait remaining time
        overhead += (1000.0/60.0) - frameTicks;
        SDL_Delay((1000.0/60.0) - frameTicks);
    }
}

fs_display_t *get_display() { return &display; }
fs_input_t *get_input() { return &input; }

int main() {
    unsigned char *main_memory = malloc(VM_MEMORY_LIMIT);
    Memory memory;
    memory_init(&memory, main_memory);

    CPU_State cpu_state = cpu_init(&memory);

    Module module = module_load_name(&cpu_state, "kernel");
    module_register(&cpu_state, module);

    cpu_set_entry_to_module(&cpu_state, &module);

    register_syscall(&cpu_state, "debug_print", debug_print);
    register_syscall(&cpu_state, "pump_events", pump_events);
    register_bindings_draw(&cpu_state);

    display = display_init();
    input = input_init();

    vm_type_t ret = cpu_run(&cpu_state);

    input_destroy(&input);
    display_destroy(&display);

    free(main_memory);
    memory_destroy(&memory);
    cpu_destroy(&cpu_state);

    return 0;
}
