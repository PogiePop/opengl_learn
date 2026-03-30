#version 330 core
layout (location = 0) out vec4 FragColor;
uniform vec3 color = vec3(0.3, 0.4, 0.5);

void main()
{
    FragColor = vec4(color, 1.0);
}