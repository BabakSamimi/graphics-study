#include "camera.h"

void init_camera(Camera* cam, vec3 pos, vec3 dir, vec3 up, float fov, float speed)
{
    cam->position = pos;
    cam->direction = dir;
    cam->world_up = up;
    
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
        scaled_dir = cam->direction;
    else
        scaled_dir = cam->right;
    
    scaled_dir = scale_vec3(scaled_dir, speed);
    cam->position = add_vec3(cam->position, scaled_dir);

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
    cam->direction = normalize_vec3(cam->direction);

    cam->right = cross_vec3(cam->direction, cam->world_up);
    cam->right = normalize_vec3(cam->right);

    cam->up = cross_vec3(cam->right, cam->direction);
    cam->up = normalize_vec3(cam->up);

}
mat4x4 get_camera_view_matrix(Camera* cam)
{
    vec3 cam_look_at = cam_look_at = add_vec3(cam->position, cam->direction);
            
    return look_at(cam->position, cam_look_at, cam->up);
    
}
