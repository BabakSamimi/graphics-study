#include "camera.h"

void init_camera(Camera* cam, vec3* pos, vec3* dir, vec3* up, float fov, float speed)
{
    copy_v3(&cam->position, pos);
    copy_v3(&cam->direction, dir);
    copy_v3(&cam->world_up, up);
    
    cam->fov = fov;
    cam->speed = speed;

    cam->pitch = 0.0f;
    cam->yaw = -90.0f;

    update_camera_transform(cam, 0.0f, 0.0f);
    
}

void move_camera(Camera* cam, WalkingDirection dir, float speed)
{

    vec3 scaled_dir;
        
    if(dir == FORWARD || dir == BACKWARD)        
        copy_v3(&scaled_dir, &cam->direction);    
    else
        copy_v3(&scaled_dir, &cam->right);
    
    scale_v3(&scaled_dir, speed);
    add_v3(&cam->position, &cam->position, &scaled_dir);

}

void update_camera_transform(Camera* cam, float x_offset, float y_offset)
{
    if (x_offset == 0.0f && y_offset == 0.0f) return;

    cam->yaw -= x_offset;
    cam->pitch += y_offset;
    if (cam->pitch > 89.0f) cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;

    cam->direction.x = cos(RADIANS(cam->yaw)) * cos(RADIANS(cam->pitch));
    cam->direction.y = sin(RADIANS(cam->pitch));
    cam->direction.z = sin(RADIANS(cam->yaw)) * cos(RADIANS(cam->pitch));    
    normalize_v3(&cam->direction);

    cross_v3(&cam->right, &cam->direction, &cam->world_up);
    normalize_v3(&cam->right);

    cross_v3(&cam->up, &cam->right, &cam->direction);
    normalize_v3(&cam->up);

}
void get_camera_view_matrix(mat4 view, Camera* cam)
{
    vec3 cam_look_at;
    add_v3(&cam_look_at, &cam->position, &cam->direction);
            
    look_at(view, &cam->position, &cam_look_at, &cam->up);
    
}
