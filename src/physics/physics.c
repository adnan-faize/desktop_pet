#include "physics.h"
#include <math.h>

void physics_update(body_t* body, float dt, float boundary_width, float boundary_height) {
    body->position.x += body->velocity.x * dt;
    body->position.y += body->velocity.y * dt;

    // Bounce off walls
    if (body->position.x < 0) {
        body->position.x = 0;
        body->velocity.x = -body->velocity.x;
    } else if (body->position.x + body->size.x > boundary_width) {
        body->position.x = boundary_width - body->size.x;
        body->velocity.x = -body->velocity.x;
    }

    if (body->position.y < 0) {
        body->position.y = 0;
        body->velocity.y = -body->velocity.y;
    } else if (body->position.y + body->size.y > boundary_height) {
        body->position.y = boundary_height - body->size.y;
        body->velocity.y = -body->velocity.y;
    }
}

void physics_follow_target(body_t* body, float target_x, float target_y, float dt) {
    // Calculate vector to target (center of body to target)
    float dx = target_x - (body->position.x + body->size.x / 2.0f);
    float dy = target_y - (body->position.y + body->size.y / 2.0f);
    
    float distance = sqrtf(dx * dx + dy * dy);
    
    if (distance > 5.0f) { // Stop when close enough
        body->velocity.x = (dx / distance) * body->speed;
        body->velocity.y = (dy / distance) * body->speed;
    } else {
        body->velocity.x = 0;
        body->velocity.y = 0;
    }
}
