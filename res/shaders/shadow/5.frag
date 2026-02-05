#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
}fs_in;

uniform samplerCube depthMap;
uniform float far_plane = 25.0;


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

float CalculateShadow(vec3 FragPos)
{
    vec3 fragToLight = FragPos - plt.position;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.05;
    float samples = 4.0;
    float offset = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r;
                closestDepth *= far_plane;   // Undo mapping [0;1]
                if(currentDepth - bias > closestDepth)
                shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

vec3 CalculateHalfVectorLight(Material mtr, PointLight plt)
{
    vec3 ambient = plt.ambient * vec3(texture(mtr.diffuse0, fs_in.TexCoord));
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(plt.position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = plt.diffuse * diff * vec3(texture(mtr.diffuse0, fs_in.TexCoord));
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfVector = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfVector), 0.0), mtr.shininess);
    vec3 specular = plt.specular * spec * vec3(texture(mtr.specular0, fs_in.TexCoord));
    float shadow = CalculateShadow(fs_in.FragPos);
    return ambient + (1.0 - shadow) * (diffuse + specular);
}


void main()
{
    FragColor = vec4(CalculateHalfVectorLight(material, plt), 1.0);
}