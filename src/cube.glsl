#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out vec3 normal;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    frag_pos = vec3(model * vec4(a_pos, 1.0)); // world-space coord
    normal = mat3(transpose(inverse(model))) * a_normal;
}

#endif

#ifdef FRAGMENT_SHADER

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;    
};

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform vec3 object_color;
uniform vec3 view_pos;

uniform vec3 light_color;
uniform Light light;

uniform Material material;

void main()
{

    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    vec3 unit_normal = normalize(normal);
    vec3 light_dir = normalize(light.position - frag_pos);

    float diffuse_coeff = max(dot(unit_normal, light_dir), 0.0);
    vec3 diffuse = (diffuse_coeff * material.diffuse) * light.diffuse;

    // Specular
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, unit_normal);
    
    float specular_strength = 0.5;
    float specular_coeff = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular =  (specular_coeff * material.specular) * light.specular;

    frag_color = vec4((ambient + diffuse + specular), 1.0);
}

#endif
