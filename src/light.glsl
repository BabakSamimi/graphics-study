#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER

out vec4 frag_color;

void main()
{
    
    frag_color = vec4(1.0);
}

#endif
