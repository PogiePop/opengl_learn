#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;    // 片段世界坐标
    vec2 TexCoord;   // 纹理坐标
    mat3 TBN;        // 切线空间→世界空间转换矩阵
}fs_in;

// 保留所有实际使用的纹理（diffuse/normal/specular）
struct Material
{
    sampler2D diffuse0;    // 漫反射纹理
    sampler2D specular0;   // 高光纹理（你已添加t3）
    sampler2D normal0;     // 法线纹理
    float shininess;       // 高光指数
};

// 点光源结构体（完整包含衰减参数）
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

// 全局Uniform（仅保留实际使用的）
uniform PointLight plt;       // 点光源（与代码中SetUniform("plt", sd1)一致）
uniform Material material;    // 材质
uniform vec3 viewPos;         // 相机位置

// 修复后的点光源计算函数（添加衰减+完整逻辑）
vec3 CalculatePointLight(Material mtr, PointLight pl)
{
    // 1. 法线贴图转换（切线空间→世界空间）
    vec3 normal = texture(mtr.normal0, fs_in.TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); // [0,1] → [-1,1]
    normal = normalize(fs_in.TBN * normal); // 转换到世界空间

    // 2. 基础向量计算
    vec3 lightDir = normalize(pl.position - fs_in.FragPos); // 光源→片段方向
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);       // 相机→片段方向

    // 3. 核心：点光源距离衰减计算（没有这个，移动光源完全没反应）
    float distance = length(pl.position - fs_in.FragPos); // 光源到片段的距离
    float attenuation = 1.0 / (pl.constant + pl.linear * distance + pl.quadratic * distance * distance);

    // 4. 环境光（基础亮度）
    vec3 ambient = pl.ambient * texture(mtr.diffuse0, fs_in.TexCoord).rgb;

    // 5. 漫反射计算
    float diffFactor = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pl.diffuse * diffFactor * texture(mtr.diffuse0, fs_in.TexCoord).rgb;

    // 6. 镜面反射（使用你添加的specular0纹理，不再用固定值）
    vec3 halfVector = normalize(lightDir + viewDir); // Blinn-Phong半程向量
    float specFactor = pow(max(dot(normal, halfVector), 0.0), mtr.shininess);
    vec3 specular = pl.specular * specFactor * texture(mtr.specular0, fs_in.TexCoord).rgb;

    // 7. 应用衰减（关键：距离越远，光照越弱，移动光源能看到明暗变化）
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 finalColor = CalculatePointLight(material, plt);
    FragColor = vec4(finalColor, 1.0);
}