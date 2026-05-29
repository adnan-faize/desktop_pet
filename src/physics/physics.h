#ifndef PHYSICS_H_
#define PHYSICS_H_

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    vec2_t position;
    vec2_t velocity;
    vec2_t size;
    float speed;
} body_t;

void physics_update(body_t* body, float dt, float boundary_width, float boundary_height);
void physics_follow_target(body_t* body, float target_x, float target_y, float dt);

#endif // PHYSICS_H_
