#include "camera.h"

void init_camera(Camera* c, vec3* pos, vec3* dir, vec3* up, float fov, float sensitivity, float speed)
{
    copy_v3(&c->position, pos);
    copy_v3(&c->direction, dir);
    copy_v3(&c->up, up);
    
    c->fov = fov;
    c->sensitivity = sensitivity;
    c->speed = speed;

    c->pitch = 0.0f;
    c->yaw = -90.0f;
    
}
