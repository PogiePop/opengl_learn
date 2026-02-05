#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Material
{
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};
uniform Material material;

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
uniform PointLight plt;

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float innerCutOff;
    float outerCutOff;
};
uniform SpotLight splt;


struct ParallelLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform ParallelLight prlt;


vec3 CalculatePointLight(PointLight pt, Material mt, vec3 lightDir, vec3 viewDir, vec3 normal, vec2 texC, float dis)
{
    vec3 ambient = pt.ambient * vec3(texture(mt.diffuse0, texC));
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = pt.diffuse * diff * vec3(texture(mt.diffuse0, texC));
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), mt.shininess);
    vec3 specular = pt.specular * spec * vec3(texture(mt.specular0, texC));
    float attenuation = 1.0 / (pt.constant + pt.linear * dis + pt.quadratic * dis * dis);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight spt, Material mt, vec3 lightDir, vec3 viewDir, vec3 normal, vec2 texC, float dis)
{
    vec3 ambient = spt.ambient * vec3(texture(mt.diffuse0, texC));
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = spt.diffuse * diff * vec3(texture(mt.diffuse0, texC));
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), mt.shininess);
    vec3 specular = spt.specular * spec * vec3(texture(mt.specular0, texC));
    float attenuation = 1.0 / (spt.constant + spt.linear * dis + spt.quadratic * dis * dis);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float theta = max(dot(-lightDir, normalize(spt.direction)), 0.0);
    float intensity = clamp((theta - spt.outerCutOff) / (spt.innerCutOff - spt.outerCutOff), 0.0, 1.0);
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;
    return ambient + diffuse + specular;
}

vec3 CalculateParallelLight(ParallelLight prt, Material mt, vec3 viewDir, vec3 normal, vec2 texC)
{
    vec3 ambient = prt.ambient * vec3(texture(mt.diffuse0, texC));
    vec3 lightDir = -normalize(prt.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = prt.diffuse * diff * vec3(texture(mt.diffuse0, texC));
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), mt.shininess);
    vec3 specular = prt.specular * spec * vec3(texture(mt.specular0, texC));
    return ambient + diffuse + specular;
}

void main()
{
    vec3 lightDir_splt = normalize(splt.position - FragPos);
    vec3 lightDir_plt = normalize(plt.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 normal = normalize(Normal);
   float dis_plt = length(plt.position - FragPos);
   float dis_splt = length(splt.position - FragPos);
    vec4 res = vec4(CalculatePointLight(plt, material, lightDir_plt, viewDir, normal, TexCoords, dis_plt), 1.0);
    res += vec4(CalculateSpotLight(splt, material, lightDir_splt, viewDir, normal, TexCoords, dis_splt), 1.0);
    res += vec4(CalculateParallelLight(prlt, material, viewDir, normal, TexCoords), 1.0);
    FragColor = res;
}