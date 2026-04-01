#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;



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

vec3 CalculatePointLight(PointLight pt, vec3 Normal, vec3 FragPos, vec3 ambient)
{
    vec3 diffuse, specular;
    //ambient = pt.ambient * texture(albedSpec, texCoords).rgb;
    vec3 lightDir = normalize(pt.position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    diffuse = pt.diffuse * diff * texture(gAlbedo, TexCoords).rgb;
    vec3 viewDir = normalize(-FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), shininess);
    specular = pt.specular * spec;
    float dis = length(pt.position - FragPos);
    float attenuation = 1.0 / (pt.constant + pt.linear * dis + pt.quadratic * dis * dis);
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{             
    // Retrieve data from g-buffer
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    // Then calculate lighting as usual
    vec3 ambient = vec3(0.3 * AmbientOcclusion); // <-- this is where we use ambient occlusion
    FragColor = vec4(CalculatePointLight(point, Normal, FragPos, ambient), 1.0);


}