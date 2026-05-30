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

#define NUM_PETS 10

typedef struct {
    window_t* win;
    body_t body;
    image_t* spritesheet;
    bool is_snatching;
    vec2_t snatch_target;
    float snatch_timer;
    int current_frame;
    int last_rendered_frame;
    float anim_timer;
    vec2_t last_pos;
} pet_t;

static bool should_exit = false;

static void on_quit(void* user_data) {
    (void)user_data;
    should_exit = true;
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

    pet_t pets[NUM_PETS];
    int screen_x = 0, screen_y = 0;
    int screen_w = 1920, screen_h = 1080;

    for (int i = 0; i < NUM_PETS; i++) {
        pets[i].win = window_create(100, 100, "Desktop Pet");
        if (!pets[i].win) return EXIT_FAILURE;

        if (i == 0) {
#ifdef PLATFORM_LINUX
            Display* dpy = (Display*)pets[i].win->display_server;
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
        }

        int cols = 3, rows = 4;
        int scale = 2;
        pets[i].spritesheet = image_load(pets[i].win, asset_path, cols, rows, scale);
        if (!pets[i].spritesheet) return EXIT_FAILURE;

        int frame_w = pets[i].spritesheet->target_w;
        int frame_h = pets[i].spritesheet->target_h;

        window_resize(pets[i].win, frame_w, frame_h);
        window_set_input_region(pets[i].win, 0, 0, 0, 0);

        pets[i].body.position.x = (float)screen_x + (float)(rand() % (screen_w - frame_w));
        pets[i].body.position.y = (float)screen_y + (float)(rand() % (screen_h - frame_h));
        pets[i].body.velocity.x = 0;
        pets[i].body.velocity.y = 0;
        pets[i].body.size.x = (float)frame_w;
        pets[i].body.size.y = (float)frame_h;
        pets[i].body.speed = 300.0f + (rand() % 100);

        pets[i].is_snatching = false;
        pets[i].snatch_target = (vec2_t){0, 0};
        pets[i].snatch_timer = 0;
        pets[i].current_frame = 0;
        pets[i].last_rendered_frame = -1;
        pets[i].anim_timer = (float)(rand() % 100) / 1000.0f;
        pets[i].last_pos = (vec2_t){-1, -1};
    }

    tray_t* tray = tray_create(asset_path, "Desktop Pet");
    if (tray) tray_add_item(tray, "Quit", on_quit, NULL);

    float dt = 0.016f;
    int mx, my;
    
    while (!should_exit) {
        if (tray) tray_update(tray);

        for (int i = 0; i < NUM_PETS; i++) {
            window_poll_events(pets[i].win);
            if (pets[i].win->should_close) should_exit = true;

            window_get_mouse_pos(pets[i].win, &mx, &my);

            if (!pets[i].is_snatching) {
                physics_follow_target(&pets[i].body, (float)mx, (float)my, dt);
                float dx = (float)mx - (pets[i].body.position.x + pets[i].body.size.x / 2.0f);
                float dy = (float)my - (pets[i].body.position.y + pets[i].body.size.y / 2.0f);
                if (sqrtf(dx * dx + dy * dy) < 60.0f) {
                    pets[i].is_snatching = true;
                    pets[i].snatch_target.x = (float)screen_x + (float)(rand() % (screen_w - (int)pets[i].body.size.x));
                    pets[i].snatch_target.y = (float)screen_y + (float)(rand() % (screen_h - (int)pets[i].body.size.y));
                    pets[i].snatch_timer = 5.0f;
                    pets[i].body.speed = 800.0f;
                }
            } else {
                physics_follow_target(&pets[i].body, pets[i].snatch_target.x, pets[i].snatch_target.y, dt);
                int jx = (rand() % 10) - 5, jy = (rand() % 10) - 5;
                window_set_mouse_pos(pets[i].win, (int)(pets[i].body.position.x + pets[i].body.size.x / 2.0f) + jx, 
                                         (int)(pets[i].body.position.y + pets[i].body.size.y / 2.0f) + jy);
                
                pets[i].snatch_timer -= dt;
                int nx, ny;
                window_get_mouse_pos(pets[i].win, &nx, &ny);
                float rdx = (float)nx - (pets[i].body.position.x + pets[i].body.size.x / 2.0f);
                float rdy = (float)ny - (pets[i].body.position.y + pets[i].body.size.y / 2.0f);
                if (sqrtf(rdx * rdx + rdy * rdy) > 200.0f || pets[i].snatch_timer <= 0) {
                    pets[i].is_snatching = false;
                    pets[i].body.speed = 300.0f + (rand() % 100);
                }
            }

            physics_update(&pets[i].body, dt, (float)screen_x, (float)screen_y, (float)screen_x + (float)screen_w, (float)screen_y + (float)screen_h);

            bool pos_changed = (abs((int)pets[i].body.position.x - (int)pets[i].last_pos.x) > 0 || 
                                abs((int)pets[i].body.position.y - (int)pets[i].last_pos.y) > 0);
            if (pos_changed) {
                window_move(pets[i].win, (int)pets[i].body.position.x, (int)pets[i].body.position.y);
                pets[i].last_pos = pets[i].body.position;
            }

            pets[i].anim_timer += dt;
            if (pets[i].anim_timer >= 0.125f) {
                pets[i].current_frame = (pets[i].current_frame + 1) % (pets[i].spritesheet->cols * pets[i].spritesheet->rows);
                pets[i].anim_timer = 0;
            }

            if (pets[i].current_frame != pets[i].last_rendered_frame) {
                renderer_draw_frame(pets[i].win, pets[i].spritesheet, pets[i].current_frame);
                renderer_present(pets[i].win);
                pets[i].last_rendered_frame = pets[i].current_frame;
            }
        }

        usleep(16000);
    }

    if (tray) tray_destroy(tray);
    for (int i = 0; i < NUM_PETS; i++) {
        renderer_cleanup(pets[i].win);
        image_free(pets[i].spritesheet);
        window_destroy(pets[i].win);
    }
    log_cleanup();
    return EXIT_SUCCESS;
}
