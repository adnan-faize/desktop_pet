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

static void on_quit(void* user_data) {
    window_t* win = (window_t*)user_data;
    win->should_close = 1;
}

int main(int argc, char** argv) {
    srand(time(NULL));

    window_t* win = window_create(0, 0, "Desktop Pet");
    if (!win) {
        fprintf(stderr, "Fatal Error: Failed to create window\n");
        return EXIT_FAILURE;
    }

    uint32_t win_width = win->width;
    uint32_t win_height = win->height;

    const char* asset_path = "res/sprites/rabbits/Rabbit7.png";
    if (access(asset_path, F_OK) == -1) {
        asset_path = "../res/sprites/rabbits/Rabbit7.png";
    }

    tray_t* tray = tray_create(asset_path, "Desktop Pet");
    if (tray) {
        tray_add_item(tray, "Quit", on_quit, win);
    }

    image_t* spritesheet = image_load(win, asset_path);
    if (!spritesheet) {
        window_destroy(win);
        return EXIT_FAILURE;
    }

    int cols = 3;
    int rows = 4;
    
    // Scale size by 2
    int frame_w = (spritesheet->width / cols) * 2;
    int frame_h = (spritesheet->height / rows) * 2;
    
    int current_frame = 0;
    int total_frames = cols * rows;

    body_t pet_body = {
        .position = {100, 100},
        .velocity = {0, 0},
        .size = {(float)frame_w, (float)frame_h},
        .speed = 300.0f
    };

    bool is_snatching = false;
    vec2_t snatch_target = {0, 0};
    float snatch_timer = 0;

    float dt = 0.016f;
    
    while (!win->should_close) {
        window_poll_events(win);
        if (tray) tray_update(tray);

        int mx, my;
        window_get_mouse_pos(win, &mx, &my);
        
        if (!is_snatching) {
            physics_follow_target(&pet_body, (float)mx, (float)my, dt);
            
            float dx = (float)mx - (pet_body.position.x + pet_body.size.x / 2.0f);
            float dy = (float)my - (pet_body.position.y + pet_body.size.y / 2.0f);
            float dist = sqrtf(dx * dx + dy * dy);
            
            // Scaled detection radius
            if (dist < 60.0f) {
                is_snatching = true;
                snatch_target.x = (float)(rand() % (win_width - (int)pet_body.size.x));
                snatch_target.y = (float)(rand() % (win_height - (int)pet_body.size.y));
                snatch_timer = 5.0f;
                pet_body.speed = 800.0f;
            }
        } else {
            physics_follow_target(&pet_body, snatch_target.x, snatch_target.y, dt);
            
            int jitter_x = (rand() % 10) - 5;
            int jitter_y = (rand() % 10) - 5;
            window_set_mouse_pos(win, (int)(pet_body.position.x + pet_body.size.x / 2.0f) + jitter_x, 
                                     (int)(pet_body.position.y + pet_body.size.y / 2.0f) + jitter_y);
            
            snatch_timer -= dt;
            
            float dx = snatch_target.x - (pet_body.position.x + pet_body.size.x / 2.0f);
            float dy = snatch_target.y - (pet_body.position.y + pet_body.size.y / 2.0f);
            float dist = sqrtf(dx * dx + dy * dy);
            
            int nx, ny;
            window_get_mouse_pos(win, &nx, &ny);
            float release_dx = (float)nx - (pet_body.position.x + pet_body.size.x / 2.0f);
            float release_dy = (float)ny - (pet_body.position.y + pet_body.size.y / 2.0f);
            if (sqrtf(release_dx * release_dx + release_dy * release_dy) > 200.0f) {
                is_snatching = false;
            }

            if (dist < 30.0f || snatch_timer <= 0) {
                is_snatching = false;
                pet_body.speed = 300.0f;
            }
        }
        
        physics_update(&pet_body, dt, (float)win_width, (float)win_height);

        window_set_input_region(win, (int)pet_body.position.x, (int)pet_body.position.y, frame_w, frame_h);

        static float anim_timer = 0;
        anim_timer += dt;
        if (anim_timer > (is_snatching ? 0.03f : 0.1f)) {
            current_frame = (current_frame + 1) % total_frames;
            anim_timer = 0;
        }

        int sx = (current_frame % cols) * (spritesheet->width / cols);
        int sy = (current_frame / cols) * (spritesheet->height / rows);

        renderer_draw_image(win, spritesheet, (int)pet_body.position.x, (int)pet_body.position.y, frame_w, frame_h, sx, sy);
        renderer_present(win);

        usleep(16000);
    }

    if (tray) tray_destroy(tray);
    image_free(spritesheet);
    window_destroy(win);

    return EXIT_SUCCESS;
}
