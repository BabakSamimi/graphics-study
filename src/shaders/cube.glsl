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

struct Phong {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;  
};

struct DirectionalLight {
    vec3 direction;
    Phong phong;    
};

struct PointLight {
    vec3 position;
    Phong phong;
    float power;        
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    Phong phong;
    float power;
    float cutoff;
    float outer_cutoff;
};

in vec3 normal; // normal of the fragment
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 frag_color;

uniform vec3 cam_pos;

uniform DirectionalLight dir_light;
uniform PointLight pointLights[4];
uniform SpotLight spotlight;

uniform Material material;

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 view_dir, Phong texels, float shininess)
{
    vec3 light_dir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);

    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    // resulting light
    vec3 ambient = light.phong.ambient * texels.diffuse;
    vec3 diffuse = light.phong.diffuse * diff * texels.diffuse;
    vec3 specular = light.phong.specular * spec * texels.specular;

    return (ambient + diffuse + specular);
    
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, Phong texels, float shininess)
{
    
    vec3 light_dir = normalize(light.position - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);

    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    // Intensity calculation - inverse square law
    float distance = length(frag_pos - light.position);
    float light_intensity = light.power / (4*PI*distance);

    // resulting light
    vec3 ambient = light.phong.ambient * texels.diffuse;
    vec3 diffuse = light_intensity * light.phong.diffuse * diff * texels.diffuse;
    vec3 specular = light_intensity * light.phong.specular * spec * texels.specular;

    return (ambient + diffuse + specular);

}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, Phong texels, float shininess)
{
    
    vec3 light_dir = normalize(light.position - frag_pos);
    
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outer_cutoff;
    
    // light calculation
    float light_to_frag_dist = length(frag_pos - light.position);
    float light_intensity = light.power / (4*PI*light_to_frag_dist);

    light_intensity *= smoothstep(0.0, 1.0, (theta - light.outer_cutoff) / epsilon);        
                        
    // Diffuse
    vec3 unit_normal = normalize(normal);
    float diff = max(dot(unit_normal, light_dir), 0.0);

    // Specular
    vec3 reflect_dir = reflect(-light_dir, unit_normal);
    
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    vec3 ambient = light.phong.ambient * texels.diffuse;
    vec3 diffuse = light_intensity * light.phong.diffuse * diff * texels.diffuse;    
    vec3 specular = light_intensity * light.phong.specular * spec * texels.specular;

    return (ambient + diffuse + specular);
}    

void main()
{
    // get pixel colour of the surface
    Phong phong_texels;
    phong_texels.diffuse = vec3(texture(material.diffuse, tex_coords)).rgb; 
    phong_texels.specular = vec3(texture(material.specular, tex_coords)).rgb;
    
    vec3 view_dir = normalize(cam_pos - frag_pos);

    vec3 result = CalculateDirLight(dir_light, normal, view_dir, phong_texels, material.shininess);

    for(int pointlight_index = 0; pointlight_index < 4; pointlight_index++)
    {
        result += CalculatePointLight(pointLights[pointlight_index], normal, frag_pos, view_dir, phong_texels, material.shininess);        
    }

    result += CalculateSpotLight(spotlight, normal, frag_pos, view_dir, phong_texels, material.shininess);

        
    frag_color = vec4(result, 1.0);
}

#endif
