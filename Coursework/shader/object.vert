#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out mat3 toObjectLocal;
out vec4 ShadowCoord;

uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ShadowMatrix;
uniform mat4 MVP;

void main() {
    TexCoord = VertexTexCoord;
    FragPos = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
    Normal = normalize(NormalMatrix * VertexNormal);

    vec3 Tangent = normalize(NormalMatrix * vec3(VertexTangent));
    vec3 Binormal = normalize(cross(Normal, Tangent)) * VertexTangent.w;
    toObjectLocal = mat3(Tangent, Binormal, Normal);

    ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
