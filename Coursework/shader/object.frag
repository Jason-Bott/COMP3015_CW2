#version 460

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 toObjectLocal;
in vec4 ShadowCoord;

out vec4 FragColor;

uniform sampler2DShadow ShadowMap;
uniform sampler2D TextureMap;
uniform sampler2D NormalMap;

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

vec3 blinnPhong(int light, vec3 position, vec3 normal) {
    vec3 texColor = texture(TextureMap, TexCoord).rgb;
    vec3 ambient = lights[light].La * texColor;
    
    vec3 lightDir = normalize(vec3(lights[light].Position) - position);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = texColor * diff;

    vec3 viewDir = normalize(-position);
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), Material.Shininess);
    vec3 specular = Material.Ks * spec;

    return ambient + (diffuse + specular) * lights[light].L;
}

float computeShadow() {
    if (ShadowCoord.z < 0.0) return 1.0;
    return textureProj(ShadowMap, ShadowCoord);
}

void main() {
    vec3 normal = normalize(Normal);
    vec3 color = vec3(0.0);
    
    for (int i = 0; i < 4; i++) {
        color += blinnPhong(i, FragPos, normal) * lights[i].Brightness;
    }
    
    float shadow = computeShadow();
    color *= shadow;

    FragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
