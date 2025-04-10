#version 460

layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;

out vec2 TexCoords;

uniform vec2 position;
uniform vec2 scale;

void main()
{
    gl_Position = vec4(VertexPosition * scale + position, 0.0, 1.0);
    TexCoords = VertexTexCoord;
}