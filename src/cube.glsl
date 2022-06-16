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

in vec3 normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform vec3 object_color;
uniform vec3 view_pos;

uniform vec3 light_color;
uniform vec3 light_pos;

void main()
{

    // Ambient
    float ambient_coeff = 0.2;
    vec3 ambient_color = ambient_coeff * light_color;

    // Diffuse
    vec3 unit_normal = normalize(normal);
    vec3 light_dir = normalize(light_pos - frag_pos);

    float diffuse_coeff = max(dot(unit_normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_coeff * light_color;

    // Specular
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, unit_normal);
    float specular_strength = 0.5;
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), 64) * specular_strength * light_color;

    frag_color = vec4((ambient_color + diffuse_color + specular) * object_color, 1.0);
}

#endif
