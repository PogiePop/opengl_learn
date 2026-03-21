#version 330 core

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoord;
    vec3 Normal;
}fs_in;

uniform vec3 viewPos;
uniform vec3 lightColors[4];
uniform vec3 lightPos[4];

struct Material
{
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};

uniform Material material;
out vec4 FragColor;
uniform float constant = 1.0;
uniform float linear = 0.09;
uniform float quadratic = 0.032;

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 ambient = vec3(0.0), diffuse = vec3(0.0), specular = vec3(0.0);
    vec3 tex_diff = texture(material.diffuse0, fs_in.TexCoord).rgb;
    vec3 tex_spec = texture(material.specular0, fs_in.TexCoord).rgb;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    for(int i = 0; i < 4; ++i)
    {
        float dis = length(lightPos[i] - fs_in.FragPos);
        float attenuation = 1.0 / (constant + linear * dis + quadratic * dis * dis);
        ambient += lightColors[i] * tex_diff * attenuation;
        vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse += lightColors[i] * diff * tex_diff * attenuation;
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        specular += lightColors[i] * spec * tex_spec * attenuation;
    }
    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(tex_diff, 1.0);
}