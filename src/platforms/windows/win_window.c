
#ifdef PLATFORM_WINDOWS

#include <windows.h>

#include <stdlib.h>
#include <stdint.h>

#include "src/core/features/window.h"

static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    window_t* win = (window_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE) {
        if (win) { win->should_close = 1; }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int window_create(window_t* win, uint32_t width, uint32_t height, const char* title) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    const char* clsName = "AppContextCls";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WinProc;
    wc.hInstance = hInst;
    wc.lpszClassName = clsName;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT;

    HWND hwnd = CreateWindowEx(exStyle, clsName, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInst, NULL);

    if (!hwnd) { return EXIT_FAILURE; }

    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);

    win->native_handle = (void*)hwnd;
    win->display_server = NULL;
    win->width = width;
    win->height = height;
    win->should_close = 0;

    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);
    ShowWindow(hwnd, SW_SHOW);

    return EXIT_SUCCESS;
}

void window_poll_events(window_t* win) {
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void window_destroy(window_t* win) {
    if (win->native_handle) {
        DestroyWindow((HWND)win->native_handle);
    }
}

#endif
