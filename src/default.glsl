#ifdef VERTEX_SHADER
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.3f, 0.7f, 0.4f, 1.0f);
} 

#endif
