#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;
out vec4 ShadowCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ShadowMatrix;
uniform mat4 MVP;

void main()
{
    TexCoord = VertexTexCoord;
    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
    Normal = normalize(NormalMatrix * VertexNormal);
    ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}