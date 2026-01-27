#version 330 core
in vec3 TexCoord;
out vec4 FragColor;
uniform samplerCube material_diffuse0;

uniform vec3 viewPos;
in vec3 Normal;
in vec3 FragPos;

uniform float ratio = 1.00 / 1.52;

uniform int isRefract = 1;

void main()
{
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = vec3(1.0);
    if(isRefract == 1)
        R = refract(I, normalize(Normal), ratio);
    else
        R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(material_diffuse0, R).rgb, 1.0);
}