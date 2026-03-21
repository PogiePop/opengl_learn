#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D scene;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoord).rgb;

    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}