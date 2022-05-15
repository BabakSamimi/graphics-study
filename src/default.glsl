#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex;

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    tex_coord = a_tex;
}

#endif

#ifdef FRAGMENT_SHADER

uniform float u_time;
uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 tex_coord;

out vec4 final_color;

void main()
{
    float modulating_value = sin(u_time)*sin(u_time);
    
    vec4 texel0 = texture(texture0, tex_coord);
    vec4 texel1 = texture(texture1, tex_coord);
    
    final_color = mix(texel0, texel1, texel1.a*modulating_value); //texture(texture1, tex_coord) * vec4(vertex_color*modulating_value, 1.0);
    
    
}

#endif
