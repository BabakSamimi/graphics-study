#ifdef VERTEX_SHADER
layout (location = 0) in vec3 pos_attr;
layout (location = 1) in vec3 normal_attr;
layout (location = 2) in vec2 tex_attr;

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos_attr, 1.0);
    tex_coord = tex_attr;
}

#endif

#ifdef FRAGMENT_SHADER

in vec2 tex_coord;
out vec4 final_color;

uniform vec4 diffuse;
//uniform sampler2D diffuse_map0;
//uniform sampler2D specular_map0;

void main()
{

    /*
    vec4 diffuse = texture(diffuse_map0, tex_coord);
    vec4 specular = texture(specular_map0, tex_coord);

    final_color = mix(diffuse, specular, 1.0); //texture(texture1, tex_coord) * vec4(vertex_color*modulating_value, 1.0);
    */

    final_color = diffuse;
        
}

#endif
