#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out vec3 normal;
out vec3 frag_pos;
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    frag_pos = vec3(model * vec4(a_pos, 1.0)); // world-space coord
    normal = mat3(transpose(inverse(model))) * a_normal; // normal residing in world-space
    tex_coords = a_tex_coords;
}

#endif

#ifdef FRAGMENT_SHADER
#define PI 3.14159265

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    
    float shininess;    
};

struct Light {
    
    vec3 position;
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float power;
    float cutoff; // Angle of inner cone
    float outer_cutoff; // Angle of outer cone
    
};

in vec3 normal; // normal of the fragment
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 frag_color;

uniform vec3 cam_pos;
uniform Light light;
uniform Material material;

void main()
{

    vec3 light_dir = normalize(light.position - frag_pos);
    
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outer_cutoff;
    
    // light calculation
    float light_to_frag_dist = length(frag_pos - light.position);
    float light_intensity = light.power / (4*PI*light_to_frag_dist);

    light_intensity *= smoothstep(0.0, 1.0, (theta - light.outer_cutoff) / epsilon);
        
    // get pixel colour of the surface
    vec3 texel_diffuse = vec3(texture(material.diffuse, tex_coords)).rgb; 
    vec3 texel_specular = vec3(texture(material.specular, tex_coords)).rgb;
    vec3 texel_emission = vec3(texture(material.emission, tex_coords)).rgb;
                        
    // Diffuse
    vec3 unit_normal = normalize(normal);
    float diffuse_coeff = max(dot(unit_normal, light_dir), 0.0);

    // Specular
    vec3 view_dir = normalize(cam_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, unit_normal);
    
    float specular_strength = 0.5;
    float specular_coeff = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * texel_diffuse;
    vec3 diffuse = light_intensity * light.diffuse   * diffuse_coeff * texel_diffuse;    
    vec3 specular = light_intensity * light.specular * specular_coeff * texel_specular;
        
    frag_color = vec4((ambient + diffuse + specular), 1.0);
}

#endif
