#ifndef RENDERER_H
#define RENDERER_H

#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))

void RendererInit(int window_width, int window_height);
void Render(float dt);

#endif
