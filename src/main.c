#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "window/window.h"
#include "renderer/renderer.h"
#include "physics/physics.h"
#include "tray/tray.h"
#include "logger/logger.h"

#ifdef PLATFORM_LINUX
#include <X11/Xlib.h>
#include <gtk/gtk.h>
#endif

static void on_quit(void* user_data) {
    window_t* win = (window_t*)user_data;
    win->should_close = 1;
}

int main(int argc, char** argv) {
    srand(time(NULL));
#ifdef PLATFORM_LINUX
    g_setenv("GDK_BACKEND", "x11", FALSE);
    gtk_init(&argc, &argv);
#endif
    log_init("desktop_pet.log", true, 1024 * 1024); // 1MB limit for rotation

    const char* asset_path = "res/sprites/rabbits/Rabbit7.png";
    if (access(asset_path, F_OK) == -1) asset_path = "../res/sprites/rabbits/Rabbit7.png";

    // 1. Create window first with a default size
    window_t* win = window_create(100, 100, "Desktop Pet");
    if (!win) return EXIT_FAILURE;

    // 2. Get screen boundaries (Global)
    int screen_x = 0, screen_y = 0;
    int screen_w = 1920, screen_h = 1080;
#ifdef PLATFORM_LINUX
    Display* dpy = (Display*)win->display_server;
    if (dpy) {
        XWindowAttributes root_attrs;
        XGetWindowAttributes(dpy, RootWindow(dpy, DefaultScreen(dpy)), &root_attrs);
        screen_w = root_attrs.width;
        screen_h = root_attrs.height;
    }
#elif defined(PLATFORM_WINDOWS)
    screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    screen_y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    screen_w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    screen_h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#endif

    // 3. Load spritesheet using the window's display connection
    int scale = 2;
    image_t* spritesheet = image_load(win, asset_path, scale);
    if (!spritesheet) {
        window_destroy(win);
        return EXIT_FAILURE;
    }

    // 4. Calculate animal frame size and resize window to match exactly
    int cols = 3, rows = 4;
    int frame_w = spritesheet->width / cols;
    int frame_h = spritesheet->height / rows;

    window_resize(win, frame_w, frame_h);
    
    // 5. Make the window click-through so it doesn't block the mouse
    window_set_input_region(win, 0, 0, 0, 0);

    tray_t* tray = tray_create(asset_path, "Desktop Pet");
    if (tray) tray_add_item(tray, "Quit", on_quit, win);

    body_t pet_body = {
        .position = {(float)screen_x + 100.0f, (float)screen_y + 100.0f},
        .velocity = {0, 0},
        .size = {(float)frame_w, (float)frame_h},
        .speed = 300.0f
    };

    bool is_snatching = false;
    vec2_t snatch_target = {0, 0};
    float snatch_timer = 0;
    float dt = 0.016f;
    int current_frame = 0;
    int mx = screen_x + screen_w / 2;
    int my = screen_y + screen_h / 2;
    int last_mx = -1, last_my = -1;
    vec2_t last_pos = {-1, -1};

    while (!win->should_close) {
        window_poll_events(win);
        if (tray) tray_update(tray);

        // 6. Global Mouse Tracking
        window_get_mouse_pos(win, &mx, &my);
        
        if (!is_snatching) {
            physics_follow_target(&pet_body, (float)mx, (float)my, dt);
            float dx = (float)mx - (pet_body.position.x + pet_body.size.x / 2.0f);
            float dy = (float)my - (pet_body.position.y + pet_body.size.y / 2.0f);
            if (sqrtf(dx * dx + dy * dy) < 60.0f) {
                is_snatching = true;
                snatch_target.x = (float)screen_x + (float)(rand() % (screen_w - (int)pet_body.size.x));
                snatch_target.y = (float)screen_y + (float)(rand() % (screen_h - (int)pet_body.size.y));
                snatch_timer = 5.0f;
                pet_body.speed = 800.0f;
            }
        } else {
            physics_follow_target(&pet_body, snatch_target.x, snatch_target.y, dt);
            int jx = (rand() % 10) - 5, jy = (rand() % 10) - 5;
            window_set_mouse_pos(win, (int)(pet_body.position.x + pet_body.size.x / 2.0f) + jx, 
                                     (int)(pet_body.position.y + pet_body.size.y / 2.0f) + jy);
            
            snatch_timer -= dt;
            int nx, ny;
            window_get_mouse_pos(win, &nx, &ny);
            float rdx = (float)nx - (pet_body.position.x + pet_body.size.x / 2.0f);
            float rdy = (float)ny - (pet_body.position.y + pet_body.size.y / 2.0f);
            if (sqrtf(rdx * rdx + rdy * rdy) > 200.0f || snatch_timer <= 0) {
                is_snatching = false;
                pet_body.speed = 300.0f;
            }
        }
        
        physics_update(&pet_body, dt, (float)screen_x, (float)screen_y, (float)screen_x + (float)screen_w, (float)screen_y + (float)screen_h);

        // 7. Animal movement
        window_move(win, (int)pet_body.position.x, (int)pet_body.position.y);
        
        // Performance: Only log when there's significant movement/change
        if (abs(mx - last_mx) > 2 || abs(my - last_my) > 2 || 
            abs((int)pet_body.position.x - (int)last_pos.x) > 1 || 
            abs((int)pet_body.position.y - (int)last_pos.y) > 1) {
            log_info("Window: (%d, %d) | Cursor: (%d, %d)", 
                     (int)pet_body.position.x, (int)pet_body.position.y, mx, my);
            last_mx = mx; last_my = my;
            last_pos.x = pet_body.position.x; last_pos.y = pet_body.position.y;
        }

        static float anim_timer = 0;
        anim_timer += dt;
        if (anim_timer > (is_snatching ? 0.03f : 0.1f)) {
            current_frame = (current_frame + 1) % (cols * rows);
            anim_timer = 0;
        }

        int sx = (current_frame % cols) * frame_w;
        int sy = (current_frame / cols) * frame_h;

        // 8. Draw pre-scaled frame
        renderer_draw_image(win, spritesheet, sx, sy, frame_w, frame_h);
        renderer_present(win);

        // Optimization: if both velocities are near zero, we could sleep longer, 
        // but we still need to poll global mouse position.
        usleep(16000);
    }

    if (tray) tray_destroy(tray);
    renderer_cleanup(win);
    image_free(spritesheet);
    window_destroy(win);
    log_cleanup();
    return EXIT_SUCCESS;
}
