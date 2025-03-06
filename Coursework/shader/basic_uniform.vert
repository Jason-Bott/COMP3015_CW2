#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;
out vec3 Tangent;
out vec3 Binormal;
out vec3 SkyBoxVec;
out mat3 toObjectLocal;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    TexCoord = VertexTexCoord;
    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
    Normal = normalize(NormalMatrix * VertexNormal);
    Tangent = normalize(NormalMatrix * vec3(VertexTangent));
    Binormal = normalize(cross(Normal, Tangent)) * VertexTangent.w;
    SkyBoxVec = VertexPosition;

    toObjectLocal = mat3(
      Tangent.x, Binormal.x, Normal.x,
      Tangent.y, Binormal.y, Normal.y,
      Tangent.z, Binormal.z, Normal.z
    );

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
