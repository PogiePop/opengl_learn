#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
struct Material
{
  sampler2D diffuse0;
};

uniform Material material;

void main()
{
    float depth = texture(material.diffuse0, TexCoord).r;
    FragColor = vec4(vec3(depth), 1.0);
}