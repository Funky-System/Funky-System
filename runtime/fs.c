#define _XOPEN_SOURCE 500
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#include "../compat/dirent.h"
#include <Windows.h>
#include <shellapi.h>
#else
#include <dirent.h>
#include <ftw.h>
#endif

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
    VM_RETURN_INT(state, stat(path, &b) == 0);
}

static void fs_fileExists(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    struct stat b;
    VM_RETURN_INT(state, stat(path, &b) == 0 && !S_ISDIR(b.st_mode));
}

static void fs_dirExists(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    struct stat b;
    VM_RETURN_INT(state, stat(path, &b) == 0 && S_ISDIR(b.st_mode));
}

static void fs_mkdir(CPU_State *state) {
    int error = 0;
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -0));
    #if defined(_WIN32)
        error = _mkdir(path); // can be used on Windows
    #else
        mode_t mode = 0733; // UNIX style permissions
        error = mkdir(path, mode); // can be used on non-Windows
    #endif
    VM_RETURN_INT(state, error);
}

#ifdef _MSC_VER
LONG DeleteDirectoryAndAllSubfolders(LPCWSTR wzDirectory)
{
    WCHAR szDir[MAX_PATH+1];  // +1 for the double null terminate
    SHFILEOPSTRUCTW fos = {0};

    wcscpy(szDir, wzDirectory);
    int len = lstrlenW(szDir);
    szDir[len+1] = 0; // double null terminate for SHFileOperation

    // delete the folder and everything inside
    fos.wFunc = FO_DELETE;
    fos.pFrom = szDir;
    fos.fFlags = FOF_NO_UI;
    return SHFileOperation( &fos );
}

static int rmrfdir(const char* path) {
    wchar_t wpath[MAX_PATH+1];
    mbstowcs(wpath, path, strlen(path)+1);//Plus null
    DeleteDirectoryAndAllSubfolders(wpath);
    return 0;
}
#else
static int rmFiles(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if(remove(pathname) < 0)
    {
        perror("ERROR: remove");
        return -1;
    }

    return 0;
}

static int rmrfdir(const char* path) {
    return nftw(path, rmFiles, 10, FTW_DEPTH|FTW_MOUNT|FTW_PHYS);
}
#endif


static void fs_rmdir(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, -1));
    const int force = STACK_VALUE(state, 0)->int_value;
    if (force) {
        VM_RETURN_INT(state, rmrfdir(path));
    } else {
        VM_RETURN_INT(state, rmdir(path));
    }
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
    const char *filename = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));

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
    buffer = (char*) malloc(sizeof(char)*lSize + 1);
    if (buffer == NULL) {
        fputs("Memory error", stderr);
        exit(2);
    }

    // copy the file into the buffer:
    result = fread(buffer, 1, lSize, pFile);
    if (result != lSize) {
        fputs("Reading error", stderr);
        exit(3);
    }
    buffer[lSize] = '\0';

    /* the whole file is now loaded in the memory buffer. */
    state->rr = vm_create_string(state, buffer);

    // terminate
    fclose (pFile);
    free (buffer);
}

static void fs_delete(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));
    VM_RETURN_INT(state, unlink(path));
}

static void fs_dir(CPU_State *state) {
    const char *path = cstr_pointer_from_vm_value(state, STACK_VALUE(state, 0));

    vm_value_t array = vm_create_array(state);

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            //printf ("%s\n", ent->d_name);
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            vm_array_append(state, array, vm_create_string(state, ent->d_name));
        }
        closedir(dir);
    } else {
        /* could not open directory */
        //perror("");
        //exit(EXIT_FAILURE);
        VM_RETURN_EMPTY(state);
    }

    VM_RETURN(state, array);
}

void register_bindings_fs(CPU_State *state) {
    register_syscall(state, "rename", fs_rename);
    register_syscall(state, "exists", fs_exists);
    register_syscall(state, "fileExists", fs_fileExists);
    register_syscall(state, "dirExists", fs_dirExists);
    register_syscall(state, "mkdir", fs_mkdir);
    register_syscall(state, "rmdir", fs_rmdir);
    register_syscall(state, "readString", fs_readString);
    register_syscall(state, "writeString", fs_writeString);
    register_syscall(state, "delete", fs_delete);
    register_syscall(state, "dir", fs_dir);
}
