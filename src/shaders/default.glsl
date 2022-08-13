#ifdef VERTEX_SHADER
layout (location = 0) in vec3 pos_attr;
layout (location = 1) in vec3 normal_attr;
layout (location = 2) in vec2 tex_attr;

out vec2 tex_coord;
out vec3 frag_normal;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    
    frag_pos = vec3(model * vec4(pos_attr, 1.0)); // world-space
    frag_normal = mat3(transpose(inverse(model))) * normal_attr; // normal of the primitive in world-space
    tex_coord = tex_attr;
    
    gl_Position = projection * view * model * vec4(pos_attr, 1.0);
    

}

#endif

#ifdef FRAGMENT_SHADER

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;    
};

in vec3 frag_normal;
in vec3 frag_pos;
in vec2 tex_coord;
out vec4 final_color;

uniform Material material;
uniform DirectionalLight dir_light;
uniform vec3 view_pos;
//uniform sampler2D diffuse_map0;
//uniform sampler2D specular_map0;

void main()
{

    vec3 dir_light_norm = normalize(-dir_light.direction);
    vec3 view_dir = normalize(view_pos - frag_pos);
    
    float diffuse_strength = max(dot(frag_normal, dir_light_norm), 0.0);

    vec3 reflect_dir = reflect(-dir_light_norm, frag_normal);
    float specular_strength = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    vec3 ambient = dir_light.ambient * material.ambient;
    vec3 diffuse = dir_light.diffuse * diffuse_strength * material.diffuse;
    vec3 specular = dir_light.specular * specular_strength * material.specular;

    vec3 color = ambient + diffuse + specular;
    color = pow(color, vec3(1.0 / 2.2));    
    final_color = vec4(color, 1.0);
    
        
}

#endif
