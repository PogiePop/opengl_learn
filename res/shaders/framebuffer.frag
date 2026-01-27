#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D material_diffuse0;


const int DEFAULT = 0;
const int INVERSION = 1;
const int NUCLEAR = 2;
const int BLUR = 3;
const int EDGE_CHECK =4;

uniform int state = DEFAULT;

const float offset = 1.0 / 300.0; 


vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
);

float kernel_nuclear[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float kernel_blur[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float kernel_edge_check[9] = float[](
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
);

vec3 CalculateCore(float kernel[9])
{
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(material_diffuse0, TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}

void main()
{
    vec4 texColor = texture(material_diffuse0, TexCoord);
    if(state == INVERSION)
    {
        FragColor = texColor;
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    else if(state == NUCLEAR)
        FragColor = vec4(CalculateCore(kernel_nuclear), 1.0);
    else if(state == BLUR)
        FragColor = vec4(CalculateCore(kernel_blur), 1.0);
    else if(state == EDGE_CHECK)
        FragColor = vec4(CalculateCore(kernel_edge_check), 1.0);
    else
        FragColor = texColor;
}