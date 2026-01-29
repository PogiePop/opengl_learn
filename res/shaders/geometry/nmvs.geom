#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec3 normal;
}gs_in[];

uniform mat4 projection;
uniform float magnitude = 0.4;

void GenerateNormal(int index)
{   
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * magnitude); 
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateNormal(0);
    GenerateNormal(1);
    GenerateNormal(2);
}