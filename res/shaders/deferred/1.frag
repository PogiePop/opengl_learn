#version 330 core
layout (location = 0) out vec3 positions;
layout (location = 1) out vec3 normals;
layout (location = 2) out vec4 albedSpecs;


in vec3 FragPos;
in vec2 texCoords;
in vec3 Normal;

uniform sampler2D material_diffuse0;

void main()
{
    normals = normalize(Normal);
    positions = FragPos;
    albedSpecs.rgb = texture(material_diffuse0, texCoords).rgb;
    albedSpecs.a = 0.5;
}