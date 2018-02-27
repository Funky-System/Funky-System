#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"

static void fs_rename(CPU_State *state) {
    const char* from = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -1));
    const char* to = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    rename(from, to);
}

static void fs_exists(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    struct stat b;
    VM_RETURN_INT(state, stat(path, &b));
}

static void fs_mkdir(CPU_State *state) {
    mode_t mode = 0733; // UNIX style permissions
    int error = 0;
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    #if defined(_WIN32)
        error = _mkdir(path); // can be used on Windows
    #else
        error = mkdir(path, mode); // can be used on non-Windows
    #endif
    VM_RETURN_INT(state, error);
}

static void fs_rmdir(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    VM_RETURN_INT(state, rmdir(path));
}

static void fs_writeString(CPU_State *state) {
    FILE * pFile;
    const char *filename = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -1));
    const char *contents = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    pFile = fopen(filename, "w");
    fwrite(contents, sizeof(char), strlen(contents), pFile);
    fclose(pFile);
}

static void fs_readString(CPU_State *state) {
    const char *filename = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -1));

    FILE * pFile;
    size_t lSize;
    char * buffer;
    size_t result;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    // obtain file size:
    fseek(pFile, 0, SEEK_END);
    lSize = (size_t)ftell(pFile);
    rewind(pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc(sizeof(char)*lSize);
    if (buffer == NULL) {
        fputs ("Memory error", stderr);
        exit (2);
    }

    // copy the file into the buffer:
    result = fread(buffer, 1, lSize, pFile);
    if (result != lSize) {
        fputs("Reading error", stderr);
        exit(3);
    }

    /* the whole file is now loaded in the memory buffer. */
    state->rr = vm_create_string(state, buffer);

    // terminate
    fclose (pFile);
    free (buffer);
}

void register_bindings_fs(CPU_State *state) {
    register_syscall(state, "rename", fs_rename);
    register_syscall(state, "exists", fs_exists);
    register_syscall(state, "mkdir", fs_mkdir);
    register_syscall(state, "rmdir", fs_rmdir);
}
