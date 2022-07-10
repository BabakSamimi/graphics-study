#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_pos;

uniform mat4 model;
uniform mat4 projection;

uniform vec2 p0;
uniform vec2 p1;
uniform vec3 color;

out vec3 in_color;

void main()
{
    gl_Position = projection * model * vec4(a_pos.xy, 0.0, 1.0);
    in_color = color;
}

#endif

#ifdef FRAGMENT_SHADER

in vec3 in_color;
out vec4 frag_color;

vec3 normalize_rgb(vec3 c)
{
    return vec3(c.x/255, c.y/255, c.z/255);
}

void main()
{
    // 56, 140, 120
    frag_color = vec4(normalize_rgb(in_color), 1.0);
}

#endif
