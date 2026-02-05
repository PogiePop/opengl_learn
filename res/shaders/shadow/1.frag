#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
}fs_in;

struct Material 
{
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};

struct ParallelLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D shadowMap;

uniform Material material;
uniform ParallelLight pt;
uniform vec3 lightPos;
uniform vec3 viewPos;

float CalculateShadow(vec4 fragPosLightSpace, float diff)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = max(0.05 * (1.0 - diff), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float currentDepth = projCoords.z;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

vec3 CalculateHalfVectorLight(Material mtr, ParallelLight plt)
{
    vec3 ambient = plt.ambient * vec3(texture(mtr.diffuse0, fs_in.TexCoord));
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-plt.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = plt.diffuse * diff * vec3(texture(mtr.diffuse0, fs_in.TexCoord));
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfVector = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfVector), 0.0), mtr.shininess);
    vec3 specular = plt.specular * spec * vec3(texture(mtr.specular0, fs_in.TexCoord));
    float shadow = CalculateShadow(fs_in.FragPosLightSpace, diff);
    return ambient + (1.0 - shadow) * (diffuse + specular);
}



void main()
{
    FragColor = vec4(CalculateHalfVectorLight(material, pt), 1.0);
}