#ifndef RENDERER_H
#define RENDERER_H

#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "camera.h"

#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))

void render_init();
void render(float dt, Camera *cam);

#endif
