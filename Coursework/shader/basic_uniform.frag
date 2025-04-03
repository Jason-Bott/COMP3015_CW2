#version 460

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

uniform sampler2DShadow ShadowMap;
uniform sampler3D OffsetTex;

layout (location = 0) out vec4 FragColor;

layout (binding = 1) uniform sampler2D TexColor;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
    float Intensity;
} lights[2];

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
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }
    return ambient + (diffuse + spec) * lights[light].L;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine(RenderPassType)
void shadeWithShadow() {
    vec3 ambient = lights[0].Intensity * Material.Ka;
    vec3 diffAndSpec = lights[0].Intensity * blinnPhong(0, Position, normalize(Normal));

    float sum = 0;
    float shadow = 1.0;

    if(ShadowCoord.z >= 0) {
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1, -1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1, 1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1, 1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1, -1));
        shadow = sum * 0.25;
    }

    FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);
    FragColor += lights[1].Intensity * vec4(blinnPhong(1, Position, normalize(Normal)), 1.0);
    FragColor = pow(FragColor, vec4(1.0 / 2.2));
}

subroutine(RenderPassType)
void recordDepth() {
    //do nothing, depth written automatically
}

void main() {
    RenderPass();
}
