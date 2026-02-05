#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
uniform vec3 viewPos;
in vec3 colors;
in vec2 TexCoord;
uniform int state = 0;

struct Material
{
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};

uniform Material mt;

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pt;

vec3 CalculateNormalLight(Material mtr, PointLight plt)
{
    vec3 ambient = plt.ambient * vec3(texture(mtr.diffuse0, TexCoord));
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(plt.position - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = plt.diffuse * diff * vec3(texture(mtr.diffuse0, TexCoord));
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtr.shininess);
    vec3 specular = plt.specular * spec * vec3(texture(mtr.specular0, TexCoord));
    float dis = length(lightDir - FragPos);
    float attenuation = 1.0 / (plt.constant + plt.linear * dis + plt.quadratic * dis * dis);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

vec3 CalculateHalfVectorLight(Material mtr, PointLight plt)
{
    vec3 ambient = plt.ambient * vec3(texture(mtr.diffuse0, TexCoord));
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(plt.position - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = plt.diffuse * diff * vec3(texture(mtr.diffuse0, TexCoord));
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfVector = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfVector), 0.0), mtr.shininess);
    vec3 specular = plt.specular * spec * vec3(texture(mtr.specular0, TexCoord));
    float dis = length(lightDir - FragPos);
    float attenuation = 1.0 / (plt.constant + plt.linear * dis + plt.quadratic * dis * dis);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

void main()
{
    if(state == 0)
        FragColor = vec4(CalculateNormalLight(mt, pt), 1.0);
    else
        FragColor = vec4(CalculateHalfVectorLight(mt, pt), 1.0);
}