#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D material_diffuse0;
in vec3 colors;

void main()
{
    FragColor = texture(material_diffuse0, TexCoord);
}