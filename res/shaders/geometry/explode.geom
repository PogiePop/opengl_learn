#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time;
uniform float magnitude = 2.0;

in VS_OUT
{
    vec2 TexCoords;
}gs_in[];

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[1].gl_Position - gl_in[2].gl_Position);
    return normalize(cross(a, b));
}

vec4 Explode(vec4 pos, vec3 normal)
{
    vec3 offset = (sin(time) + 1.0) * 0.5 * normal * magnitude;
    return pos + vec4(offset, 0.0); 
}

out vec2 TexCoord;

void main()
{
    vec3 normal = GetNormal();
    gl_Position = Explode(gl_in[0].gl_Position, normal);
    TexCoord = gs_in[0].TexCoords;
    EmitVertex();
    gl_Position = Explode(gl_in[1].gl_Position, normal);
    TexCoord = gs_in[1].TexCoords;
    EmitVertex();
    gl_Position = Explode(gl_in[2].gl_Position, normal);
    TexCoord = gs_in[2].TexCoords;
    EmitVertex();
    EndPrimitive();
}