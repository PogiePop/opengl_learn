#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D material_diffuse0;

void main()
{
    vec4 texColor = texture(material_diffuse0, TexCoord);
    // if(texColor.a < 0.1)
    //     discard;
    FragColor = texColor;
}