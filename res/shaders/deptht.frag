#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
const float far = 100.0;
const float near = 0.1;
uniform bool isLinear = false;

float TranslateToLinear(float depth)
{
    float ndc = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
    return linearDepth;
}

void main()
{
    if(isLinear)
    FragColor = vec4(vec3(TranslateToLinear(gl_FragCoord.z)), 1.0);
    else
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}