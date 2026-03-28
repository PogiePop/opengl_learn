#version 330 core
layout (location = 0)out vec4 FragColor;
layout (location = 1)out vec4 BrightColor;

in vec2 texCoords;
in vec3 FragPos;
in vec3 Normal;

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

uniform PointLight point;

vec3 CalculatePointLight(PointLight pt, Material mt)
{
    vec3 ambient, diffuse, specular;
    ambient = pt.ambient * texture(mt.diffuse0, texCoords).rgb;
    vec3 lightDir = normalize(pt.position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    diffuse = pt.diffuse * diff * texture(mt.diffuse0, texCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), mt.shininess);
    specular = pt.specular * spec * texture(mt.specular0, texCoords).rgb;
    float dis = length(lightDir);
    float attenuation = 1.0 / (pt.constant + pt.linear * dis + pt.quadratic * dis * dis);
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    FragColor = vec4(CalculatePointLight(point, material), 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
