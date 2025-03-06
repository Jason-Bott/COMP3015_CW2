#version 460

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec3 Binormal;
in vec3 SkyBoxVec;
in mat3 toObjectLocal;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform samplerCube SkyBoxTex;
layout (binding = 1) uniform sampler2D TexColor;
layout (binding = 2) uniform sampler2D NormalMap;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
    float Brightness;
} lights[4];

uniform struct MaterialInfo {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
} Material;

vec3 blinnPhong(int light, vec3 position, vec3 n) {
    vec3 diffuse = vec3(0), spec = vec3(0);

    vec3 texColor = texture(TexColor, TexCoord).rgb;

    vec3 ambient = lights[light].La * texColor;
    vec3 s = normalize(vec3(lights[light].Position.xyz) - position);
    float sDotN = max(dot(s, n), 0.0);
    diffuse = texColor * sDotN;
    if(sDotN > 0.0) {
        vec3 v = toObjectLocal * normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }
    return ambient + (diffuse + spec) * lights[light].L;
}

void main() {
    vec3 texColor = texture(SkyBoxTex, normalize(SkyBoxVec)).rgb;
    vec3 Color = vec3(0.0);

    for (int i = 0; i < 4; i++) {
        Color += blinnPhong(i, Position, Normal) * lights[i].Brightness;
    }

    float Gamma = 2.2f;
    bool isSky = length(Position) > 30.0;
    vec3 finalColor = isSky ? texColor : pow(Color, vec3(1.0/Gamma));
    FragColor = vec4(finalColor, 1.0);
}
