#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D material_diffuse0;

void main()
{
    FragColor = texture(material_diffuse0, TexCoord);
}