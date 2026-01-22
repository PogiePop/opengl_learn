#version 330 core

out vec4 FragColor;
uniform sampler2D texture0;
uniform sampler2D texture1;
in vec2 texCoords;

void main()
{
    vec4 color0 = texture(texture0, texCoords);
    vec4 color1 = texture(texture1, texCoords);
    FragColor = mix(color0, color1, 0.5);
}