#version 330 core
out vec4 FragColor;


in vec2 TexCoord;

// 纹理采样器（和你代码中绑定的变量名完全匹配）
uniform sampler2D material_diffuse0;  // 漫反射纹理（对应diffuse类型）

void main()
{
    vec4 texColor = texture(material_diffuse0, TexCoord);
    
    FragColor = texColor;
}