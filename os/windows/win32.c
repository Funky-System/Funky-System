#include "../../funky_system.h"

#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "SDL_syswm.h"

//#define ID_LOADROM 1
#define ID_ABOUT 2
#define ID_EXIT 3
#define ID_CONTROLS 4
#define ID_SCALE_1X 5
#define ID_SCALE_2X 6
#define ID_SCALE_3X 7
#define ID_SCALE_4X 8

HMENU hMenuBar, hFile, hScale, hHelp;

void reset_scale_checkboxes() {
    int scale = get_display()->scale;
    CheckMenuItem(hScale, ID_SCALE_1X, MF_BYCOMMAND | (scale == 1 ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hScale, ID_SCALE_2X, MF_BYCOMMAND | (scale == 2 ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hScale, ID_SCALE_3X, MF_BYCOMMAND | (scale == 3 ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hScale, ID_SCALE_4X, MF_BYCOMMAND | (scale == 4 ? MF_CHECKED : MF_UNCHECKED));
}

void set_scale(int scale) {
    display_set_scale(get_display(), scale);
    reset_scale_checkboxes();
}

HWND getSDLWinHandle(SDL_Window* win) {
    SDL_SysWMinfo infoWindow;
    SDL_VERSION(&infoWindow.version);
    if (!SDL_GetWindowWMInfo(win, &infoWindow))
    {
        printf("Failed: %s\n", SDL_GetError());
        return NULL;
    }
    return (infoWindow.info.win.window);
}

void initNativeWindow() {
    hMenuBar = CreateMenu();
    hFile = CreateMenu();
    hScale = CreateMenu();
    hHelp = CreateMenu();

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, "File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hScale, "Scale");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelp, "Help");

    //AppendMenu(hFile, MF_STRING, ID_LOADROM, "Load ROM");
    AppendMenu(hFile, MF_STRING, ID_EXIT, "Exit");

    AppendMenu(hScale, MF_STRING, ID_SCALE_1X, "Scale 1x");
    AppendMenu(hScale, MF_STRING, ID_SCALE_2X, "Scale 2x");
    AppendMenu(hScale, MF_STRING, ID_SCALE_3X, "Scale 3x");
    AppendMenu(hScale, MF_STRING, ID_SCALE_4X, "Scale 4x");

    AppendMenu(hHelp, MF_STRING, ID_ABOUT, "About");

    SetMenu(getSDLWinHandle(get_display()->window), hMenuBar);

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    // reset scale to make room for menu
    display_set_scale(get_display(), get_display()->scale);
    reset_scale_checkboxes();
}

int platform_handle_event(SDL_Event *event, CPU_State* state) {
    if (event->syswm.msg->msg.win.msg == WM_COMMAND)
    {
        if (LOWORD(event->syswm.msg->msg.win.wParam) == ID_EXIT)
        {
            state->running = 0;
        }

        else if (LOWORD(event->syswm.msg->msg.win.wParam) == ID_SCALE_1X)
        {
            set_scale(1);
        }

        else if (LOWORD(event->syswm.msg->msg.win.wParam) == ID_SCALE_2X)
        {
            set_scale(2);
        }

        else if (LOWORD(event->syswm.msg->msg.win.wParam) == ID_SCALE_3X)
        {
            set_scale(3);
        }

        else if (LOWORD(event->syswm.msg->msg.win.wParam) == ID_SCALE_4X)
        {
            set_scale(4);
        }

        else if (LOWORD(event->syswm.msg->msg.win.wParam) == ID_ABOUT)
        {
            MessageBox(NULL,
                   _T("Funky System\nVersion 2018.1\n\nBy Bas du Pré"),
                   _T("About Funky System"),
                   MB_OK|MB_SYSTEMMODAL);
        }
    }
}

char* get_executable_filepath() {
    DWORD last_error;
    DWORD result;
    DWORD path_size = 1024;
    char* path      = malloc(1024);

    for (;;)
    {
        memset(path, 0, path_size);
        result     = GetModuleFileName(0, path, path_size - 1);
        last_error = GetLastError();

        if (0 == result)
        {
            free(path);
            path = 0;
            return NULL;
        }
        else if (result == path_size - 1)
        {
            free(path);
            /* May need to also check for ERROR_SUCCESS here if XP/2K */
            if (ERROR_INSUFFICIENT_BUFFER != last_error)
            {
                path = 0;
                free(path);
                return NULL;
            }
            path_size = path_size * 2;
            path = malloc(path_size);
        }
        else
        {
            return path;
        }
    }

    /*if (!path)
    {
        fprintf(stderr, "Failure: %d\n", last_error);
    }
    else
    {
        printf("path=%s\n", path);
    }*/

}

char* get_executable_path(const char* append) {
    char* path = get_executable_filepath();
    char* ret = malloc(strlen(path) + strlen(append) + 1);
    strcpy(ret, path);
    strcpy(strrchr(ret, '\\') + 1, append);
    free(path);
    return ret;
}

