#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <stdlib.h>
#include <stdint.h>
#include "window.h"

static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    window_t* win = (window_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE) {
        if (win) { win->should_close = 1; }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

window_t* window_create(uint32_t width, uint32_t height, const char* title) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    const char* clsName = "DesktopPetCls";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WinProc;
    wc.hInstance = hInst;
    wc.lpszClassName = clsName;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    if (width == 0) width = GetSystemMetrics(SM_CXSCREEN);
    if (height == 0) height = GetSystemMetrics(SM_CYSCREEN);

    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT;

    HWND hwnd = CreateWindowEx(exStyle, clsName, title, WS_POPUP, 
                              0, 0, width, height, NULL, NULL, hInst, NULL);

    if (!hwnd) { return NULL; }

    window_t* win = malloc(sizeof(window_t));
    win->native_handle = (void*)hwnd;
    win->display_server = NULL;
    win->width = width;
    win->height = height;
    win->should_close = 0;

    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    
    ShowWindow(hwnd, SW_SHOW);

    return win;
}

void window_get_mouse_pos(window_t* win, int* x, int* y) {
    POINT p;
    GetCursorPos(&p);
    *x = p.x;
    *y = p.y;
}

void window_set_mouse_pos(window_t* win, int x, int y) {
    SetCursorPos(x, y);
}

void window_set_input_region(window_t* win, int x, int y, int w, int h) {
}

void window_poll_events(window_t* win) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void window_destroy(window_t* win) {
    if (win) {
        if (win->native_handle) {
            DestroyWindow((HWND)win->native_handle);
        }
        free(win);
    }
}

#endif
