#ifndef CAMERA_H
#define CAMERA_H
#include "../gfx_math.h"

typedef enum
{
    FORWARD, BACKWARD, LEFT, RIGHT
} WalkingDirection;

typedef struct
{
    /*
      world_up is the up vector of the camera in the world-coordinate system
      This is because the other 'up' vector is in the coordinate system of the camera, which means if we're looking slighty up or down, then this up vector will have to be recalculated in order to be correct.
    */

    mat4x4 view_matrix;
    vec3 position, direction, world_up, up, right;
    float pitch, yaw, fov, sensitivity, speed;
    
} Camera;

void create_camera(Camera* cam, vec3 pos, vec3 dir, vec3 up, float fov, float speed);

void update_camera_transform(Camera* cam, float x_offset, float y_offset);
void move_camera(Camera* cam, WalkingDirection dir_sign, float speed);
mat4x4 get_camera_view_matrix(Camera* cam);

#endif
