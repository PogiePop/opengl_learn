#version 330 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D albedSpec;

uniform vec3 viewPos;

uniform float shininess = 32.0;



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

vec3 CalculatePointLight(PointLight pt, vec3 Normal, vec3 FragPos)
{
    vec3 ambient, diffuse, specular;
    ambient = pt.ambient * texture(albedSpec, texCoords).rgb;
    vec3 lightDir = normalize(pt.position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    diffuse = pt.diffuse * diff * texture(albedSpec, texCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), shininess);
    specular = pt.specular * spec * texture(albedSpec, texCoords).a;
    float dis = length(pt.position - FragPos);
    float attenuation = 1.0 / (pt.constant + pt.linear * dis + pt.quadratic * dis * dis);
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    if(texture(albedSpec, texCoords).a != 0.5)discard;
    vec3 normal = texture(normals, texCoords).rgb;
    vec3 FragPos = texture(positions, texCoords).rgb;
    FragColor = vec4(CalculatePointLight(point, normal, FragPos), 1.0);
}