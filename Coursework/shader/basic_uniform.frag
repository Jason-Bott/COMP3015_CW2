#version 460

const float PI = 3.14159265358979323846;

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
    vec3 L;
} lights[5];

uniform struct MaterialInfo {
    float Rough;
    bool Metal;
    vec3 Color;
} Material;

uniform float Whiteness;

float ggxDistribution(float nDotH) {
    float alpha2 = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float geomSmith(float dotProd) {
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    if(Material.Metal) {
        f0 = Material.Color;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

vec3 microfacetModel(int lightIdx, vec3 position, vec3 n) {
    vec3 diffuseBrdf = vec3(0.0);
    if(!Material.Metal) {
        diffuseBrdf = Material.Color;
    }

    vec3 l = vec3(0.0);
    vec3 lightI = lights[lightIdx].L;
    if(lights[lightIdx].Position.w == 0.0) {
        l = normalize(lights[lightIdx].Position.xyz);
    } else {
        l = lights[lightIdx].Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * schlickFresnel(lDotH) * geomSmith(nDotL) * geomSmith(nDotV);

    vec3 texColor = texture(TexColor, TexCoord).rgb;

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL * texColor;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine(RenderPassType)
void shadeWithShadow() {
    float sum = 0;
    float shadow = 1.0;

    if(ShadowCoord.z >= 0) {
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1, -1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1, 1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1, 1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1, -1));
        shadow = sum * 0.25;
    }
    
    FragColor = vec4(microfacetModel(0, Position, normalize(Normal)) * shadow, 1.0);
    for(int i = 1; i < 5; i++) {
        FragColor += vec4(microfacetModel(i, Position, normalize(Normal)), 1.0);
    }
    FragColor = pow(FragColor, vec4(1.0 / 2.2));

    if(Whiteness > 0.0) {
        FragColor += vec4(Whiteness, Whiteness, Whiteness, Whiteness);
    }
}

subroutine(RenderPassType)
void recordDepth() {
    //do nothing, depth written automatically
}

void main() {
    RenderPass();
}