#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;

struct Material
{
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normal0;
    sampler2D depth0;
    float shininess;       // 高光指数
};

uniform Material material;

uniform float height_scale = 0.15;

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
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

// 全局Uniform（仅保留实际使用的）
uniform PointLight plt;       // 点光源（与代码中SetUniform("plt", sd1)一致）


vec3 CalculatePointLight(Material mtr, PointLight pl)
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoord, viewDir);
    // 1. 法线贴图转换（切线空间→世界空间）
    vec3 normal = texture(mtr.normal0, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); // [0,1] → [-1,1]
    //normal = normalize(fs_in.TBN * normal); // 转换到世界空间

    // 2. 基础向量计算
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos); // 光源→片段方向
    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);       // 相机→片段方向

    // 3. 核心：点光源距离衰减计算（没有这个，移动光源完全没反应）
    float distance = length(pl.position - fs_in.TangentFragPos); // 光源到片段的距离
    float attenuation = 1.0 / (pl.constant + pl.linear * distance + pl.quadratic * distance * distance);

    // 4. 环境光（基础亮度）
    vec3 ambient = pl.ambient * texture(mtr.diffuse0, texCoords).rgb;

    // 5. 漫反射计算
    float diffFactor = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pl.diffuse * diffFactor * texture(mtr.diffuse0, texCoords).rgb;

    // 6. 镜面反射（使用你添加的specular0纹理，不再用固定值）
    vec3 halfVector = normalize(lightDir + viewDir); // Blinn-Phong半程向量
    float specFactor = pow(max(dot(normal, halfVector), 0.0), mtr.shininess);
    vec3 specular = pl.specular * specFactor * texture(mtr.specular0, texCoords).rgb;

    // 7. 应用衰减（关键：距离越远，光照越弱，移动光源能看到明暗变化）
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float h = texture(material.depth0, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (h * height_scale);
    return texCoords - p;
}

void main()
{
    FragColor = vec4(CalculatePointLight(material, plt), 1.0);
}