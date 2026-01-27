#version 330 core
in vec3 TexCoord;
out vec4 FragColor;
uniform samplerCube material_diffuse0;

void main()
{
    vec4 texColor = texture(material_diffuse0, TexCoord);
    FragColor = texColor;
}