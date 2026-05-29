#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>
#include "tray.h"

#define WM_TRAY_ICON (WM_USER + 1)
#define ID_TRAY_ICON 1
#define ID_TRAY_EXIT 1001

struct tray_item {
    char* text;
    tray_callback_t callback;
    void* user_data;
};

struct tray_t {
    HWND hwnd;
    NOTIFYICONDATA nid;
    HMENU hmenu;
    struct tray_item* items;
    int num_items;
    int capacity;
};

static LRESULT CALLBACK tray_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    tray_t* tray = (tray_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (msg == WM_TRAY_ICON) {
        if (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP) {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(tray->hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);
        }
    } else if (msg == WM_COMMAND) {
        int id = LOWORD(wparam);
        if (id >= 2000 && id < 2000 + tray->num_items) {
            int idx = id - 2000;
            if (tray->items[idx].callback) {
                tray->items[idx].callback(tray->items[idx].user_data);
            }
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

tray_t* tray_create(const char* icon_path, const char* tooltip) {
    tray_t* tray = calloc(1, sizeof(tray_t));
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = tray_wnd_proc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "TrayIconClass";
    RegisterClass(&wc);

    tray->hwnd = CreateWindow("TrayIconClass", "", 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    SetWindowLongPtr(tray->hwnd, GWLP_USERDATA, (LONG_PTR)tray);

    memset(&tray->nid, 0, sizeof(tray->nid));
    tray->nid.cbSize = sizeof(tray->nid);
    tray->nid.hWnd = tray->hwnd;
    tray->nid.uID = ID_TRAY_ICON;
    tray->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    tray->nid.uCallbackMessage = WM_TRAY_ICON;
    tray->nid.hIcon = (HICON)LoadImage(NULL, icon_path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (!tray->nid.hIcon) {
        tray->nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    strncpy(tray->nid.szTip, tooltip, sizeof(tray->nid.szTip));
    
    Shell_NotifyIcon(NIM_ADD, &tray->nid);
    
    tray->hmenu = CreatePopupMenu();
    tray->capacity = 10;
    tray->items = malloc(sizeof(struct tray_item) * tray->capacity);
    
    return tray;
}

void tray_add_item(tray_t* tray, const char* text, tray_callback_t callback, void* user_data) {
    if (tray->num_items >= tray->capacity) {
        tray->capacity *= 2;
        tray->items = realloc(tray->items, sizeof(struct tray_item) * tray->capacity);
    }
    tray->items[tray->num_items].text = strdup(text);
    tray->items[tray->num_items].callback = callback;
    tray->items[tray->num_items].user_data = user_data;
    
    AppendMenu(tray->hmenu, MF_STRING, 2000 + tray->num_items, text);
    tray->num_items++;
}

void tray_update(tray_t* tray) {
    MSG msg;
    while (PeekMessage(&msg, tray->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void tray_destroy(tray_t* tray) {
    Shell_NotifyIcon(NIM_DELETE, &tray->nid);
    DestroyWindow(tray->hwnd);
    for (int i = 0; i < tray->num_items; i++) free(tray->items[i].text);
    free(tray->items);
    free(tray);
}
#endif
