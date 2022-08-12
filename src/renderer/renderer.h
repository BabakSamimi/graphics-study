#ifndef RENDERER_H
#define RENDERER_H

#include "glad/glad.h"

// temporary
typedef struct {
    GLuint diffuse_map;
    GLuint specular_map;
    GLuint emission_map;
} RenderAssets;

void render_init();
void render(float dt);

#endif
