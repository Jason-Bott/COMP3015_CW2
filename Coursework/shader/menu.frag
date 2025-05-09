#version 460

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 5) uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoords);
}
