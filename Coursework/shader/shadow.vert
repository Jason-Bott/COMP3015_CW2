#version 460

layout (location = 0) in vec3 VertexPosition;

uniform mat4 LightSpaceMatrix;
uniform mat4 ModelMatrix;

void main() {
    gl_Position = LightSpaceMatrix * ModelMatrix * vec4(VertexPosition, 1.0);
}