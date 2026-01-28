#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

struct Material {
    sampler2D diffuse0;  // 对应第一个diffuse纹理
    sampler2D diffuse1;  // 对应第二个diffuse纹理
};
uniform Material material;  // 声明结构体uniform

void main()
{
    if(gl_FrontFacing)
        FragColor = texture(material.diffuse0, TexCoord);
    else
        FragColor = texture(material.diffuse1, TexCoord);
    if(FragColor.a < 0.1)
        discard;
}