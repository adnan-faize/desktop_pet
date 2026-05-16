#include <windows.h>
#include <shellapi.h>

#include "../../core/features/tray.h"

#define WM_TRAY_CALLBACK_MESSAGE (WM_USER + 1)
#define WC_TRAY_CLASS_NAME "TRAY"
#define ID_TRAY_FIRST 1000

static WNDCLASSEX wc;
static NOTIFYICONDATA nid;
static HWND hwnd;
static HMENU hmenu = NULL;

static LRESULT CALLBACK _tray_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_TRAY_CALLBACK_MESSAGE:
            if (lparam == WM_LBUTTONUP || lparam == WM_RBUTTONUP) {
                POINT p;
                GetCursorPos(&p);
                SetForegroundWindow(hwnd);
                WORD cmd = TrackPopupMenu(
                    hmenu, 
                    TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
                    p.x,
                    p.y,
                    0,
                    hwnd,
                    NULL
                );
                SendMessage(hwnd, WM_COMMAND, cmd, 0);
                return 0;
            }
            break;
        case WM_COMMAND:
            if (wparam >= ID_TRAY_FIRST) {
                MENUITEMINFO item = {
                    .cbSize = sizeof(MENUITEMINFO), 
                    .fMask = MIIM_ID | MIIM_DATA,
                };
                if (GetMenuItemInfo(hmenu, wparam, FALSE, &item)) {
                    struct tray_menu *menu = (struct tray_menu *)item.dwItemData;
                    if (menu != NULL && menu->cb != NULL) { menu->cb(menu); }
                }
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

static HMENU _tray_menu(struct tray_menu *m, UINT *id) {
    HMENU hmenu = CreatePopupMenu();

    while (m != NULL && m->text != NULL) {
        if (strcmp(m->text, "-") == 0) 
        
        m++;
        (*id)++;
    }
}