#version 460

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 SkyBoxVec;
in mat3 toObjectLocal;
in vec4 ShadowCoord;

uniform sampler2DShadow ShadowMap;
uniform sampler3D OffsetTex;
uniform float Radius;
uniform vec3 OffsetTexSize;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform samplerCube SkyBoxTex;
layout (binding = 1) uniform sampler2D TexColor;
layout (binding = 2) uniform sampler2D NormalMap;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
    float Intensity;
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

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine(RenderPassType)
void shadeWithShadow() {
    vec3 ambient = lights[0].Intensity * Material.Ka;
    vec3 diffAndSpec = blinnPhong(0, Position, normalize(Normal));

    float sum = 0;
    float shadow = 1.0;

    ivec3 offsetCoord;
    offsetCoord.xy = ivec2(mod(gl_FragCoord.xy, OffsetTexSize.xy));
    int samplesDiv2 = int(OffsetTexSize.z);
    vec4 sc = ShadowCoord;

    if(sc.z >= 0) {
        for(int i = 0; i < 4; i++) {
            offsetCoord.z = i;
            vec4 offsets = texelFetch(OffsetTex, offsetCoord, 0) * Radius * ShadowCoord.w;
            sc.xy = ShadowCoord.xy + offsets.xy;
            sum += textureProj(ShadowMap, sc);
            sc.xy = ShadowCoord.xy + offsets.zw;
            sum += textureProj(ShadowMap, sc);
        }
        shadow = sum / 8.0;

        if(shadow != 1.0 && shadow != 0.0) {
            for(int i = 0; i < samplesDiv2; i++) {
                offsetCoord.z = i;
                vec4 offsets = texelFetch(OffsetTex, offsetCoord, 0) * Radius * ShadowCoord.w;
                sc.xy = ShadowCoord.xy + offsets.xy;
                sum += textureProj(ShadowMap, sc);
                sc.xy = ShadowCoord.xy + offsets.zw;
                sum += textureProj(ShadowMap, sc);
            }
            shadow = sum / float(samplesDiv2 * 2.0);
        }
    }
    FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);
    FragColor = pow(FragColor, vec4(1.0 / 2.2));
}

subroutine(RenderPassType)
void recordDepth() {
    //do nothing, depth written automatically
}

void main() {
    RenderPass();
    
    //vec3 texColor = texture(SkyBoxTex, normalize(SkyBoxVec)).rgb;
    //vec3 Color = vec3(0.0);

    //for (int i = 0; i < 4; i++) {
        //Color += blinnPhong(i, Position, Normal) * lights[i].Brightness;
    //}

    //float Gamma = 2.2f;
    //bool isSky = length(Position) > 30.0;
    //vec3 finalColor = isSky ? texColor : pow(Color, vec3(1.0/Gamma));
    //FragColor = vec4(finalColor, 1.0);
}
