#include "../../funky_system.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

void initNativeWindow() {

}

int platform_handle_event(SDL_Event *event, CPU_State* state) {
    return 0;
}

char* get_executable_filepath() {
    char *buf = malloc(PATH_MAX);
    readlink("/proc/self/exe", buf, PATH_MAX);
    return buf;
}

char* get_executable_path(const char* append) {
    char* path = get_executable_filepath();
    char* ret = malloc(strlen(path) + strlen(append) + 1);
    strcpy(ret, path);
    strcpy(strrchr(ret, '\\') + 1, append);
    free(path);
    return ret;
}

