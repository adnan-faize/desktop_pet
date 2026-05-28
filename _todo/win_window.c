#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const wchar_t CLASS_NAME[] = L"CompileTimeGhostWindow";

    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = DefWindowProcW; // Simplified for example
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClassW(&wc);

    // 1. Set styles based entirely on compile-time targets
#ifdef TARGET_RETRO_WIN98
    DWORD exStyle = WS_EX_TOOLWINDOW;
#else
    // Modern default (XP through 11)
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT;
#endif

    HWND hwnd = CreateWindowExW(
        exStyle, CLASS_NAME, L"Ghost Window", WS_POPUP,
        100, 100, 800, 600, NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    // 2. Execute the specific architecture branch at compile time
#ifdef TARGET_RETRO_WIN98
    // --- Windows 98 Only Compilation ---
    HRGN empty_region = CreateRectRgn(0, 0, 0, 0);
    SetWindowRgn(hwnd, empty_region, TRUE);
#else
    // --- Modern Windows (XP-11) Only Compilation ---
    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
#endif

    ShowWindow(hwnd, nShowCmd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
