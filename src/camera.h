#include "gfx_math.h"

typedef struct
{
    vec3 position, direction, up;
    float pitch, yaw, fov, sensitivity, speed;
    
} Camera;

void init_camera(Camera* c, vec3* pos, vec3* dir, vec3* up, float fov, float sensitivity, float speed);
