#ifdef VERTEX_SHADER

vec3 quad[4] = {
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 1.0, 0.0)
};

void main()
{
    mat4 scale()
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER

out vec4 frag_color;

uniform vec3 light_color;

void main()
{
    
    frag_color = vec4(light_color, 1.0);
}

#endif
