#ifndef WIN64_MAIN_H
#define WIN64_MAIN_h

#include "defines.h"

typedef struct
{
    s32 window_width, window_height;
    
    f32 target_frame_ms;
    f32 delta_time;
    f32 last_time;
    f32 fov;
    
    f64 mouse_x;
    f64 mouse_last_x;
    f64 mouse_y;
    f64 mouse_last_y;

    f32 sensitivity;

    s32 camera_control;

    s32 reloading_shaders;
    s32 wireframe_on;
    
} AppState;

#endif
