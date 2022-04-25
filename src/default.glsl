#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 vertex_color;

void main()
{
    gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
    vertex_color = a_color;
}

#endif

#ifdef FRAGMENT_SHADER

uniform vec4 u_color;

in vec3 vertex_color;
out vec4 final_color;

void main()
{
    final_color = vec4(vertex_color, 1.0); //vec4(0.3f, 0.7f, 0.4f, 1.0f);
} 

#endif
