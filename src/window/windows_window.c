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

    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST;
    HWND hwnd = CreateWindowEx(exStyle, clsName, title, WS_POPUP, 
                              100, 100, width, height, NULL, NULL, hInst, NULL);
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

void window_move(window_t* win, int x, int y) {
    SetWindowPos((HWND)win->native_handle, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
}

void window_resize(window_t* win, uint32_t w, uint32_t h) {
    SetWindowPos((HWND)win->native_handle, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE | SWP_NOACTIVATE);
    win->width = w;
    win->height = h;
}

void window_get_mouse_pos(window_t* win, int* x, int* y) {
    if (!win) return;
    POINT p;
    if (GetCursorPos(&p)) {
        *x = p.x;
        *y = p.y;
    }
}

void window_set_mouse_pos(window_t* win, int x, int y) {
    SetCursorPos(x, y);
}

void window_set_input_region(window_t* win, int x, int y, int w, int h) {
    if (!win || !win->native_handle) return;
    HWND hwnd = (HWND)win->native_handle;
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (w <= 0 || h <= 0) {
        exStyle |= WS_EX_TRANSPARENT;
    } else {
        exStyle &= ~WS_EX_TRANSPARENT;
    }
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
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
        if (win->native_handle) DestroyWindow((HWND)win->native_handle);
        free(win);
    }
}

#endif
