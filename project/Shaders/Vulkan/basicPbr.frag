#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // MVP
    vec3 cameraPos;
    mat4 model;
    mat4 view;
    mat4 projection;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;
} pc;

struct PointLight
{
    vec4 lightPos;
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;
    float attConst;
    float attLin;
    float attQuad;
    bool isInitialized;
};

layout (set = 0, binding = 0) uniform sampler smplr;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (std140, set = 1, binding = 0) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

const float PI = 3.14159265359;

vec3 MaterialColor(int textureIndex)
{
   return vec3(texture(sampler2D(textures[textureIndex], smplr), inUV));
}

float NormalDistribution(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/(PI * denom*denom);
}

float GeometricShadowing(float dotNL, float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

vec3 Fresnel(float cosTheta, float metallic)
{
    vec3 F0 = mix(vec3(0.04), MaterialColor(pc.roughnessIndex), metallic);
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
}

vec3 SpecularBRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
{
    vec3 H = normalize(V + L);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotLH = clamp(dot(L, H), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);

    vec3 lightColor = vec3(1.0);
    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        float roughness = max(0.05, roughness);
        float D = NormalDistribution(dotNH, roughness);
        float G = GeometricShadowing(dotNL, dotNV, roughness);
        vec3 F = Fresnel(dotNV, metallic);

        vec3 spec = D * F * G / (4.0 * dotNL * dotNV);
        color += spec * dotNL * lightColor;
    }

    return color;
}

void main() 
{
    vec3 N = normalize(inNormal);
    vec3 V = normalize(pc.cameraPos - inWorldPos);

    float roughness = MaterialColor(pc.roughnessIndex).r;

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (!pointLights.lights[i].isInitialized)
        {
            break;
        }

        vec3 vToL = pointLights.lights[i].lightPos.xyz - pc.cameraPos;
        float distToL = length(vToL);
        vec3 dirToL = vToL / distToL;

        float att = 1.0 / (pointLights.lights[i].attConst + pointLights.lights[i].attLin * distToL + pointLights.lights[i].attQuad * (distToL * distToL));

        vec3 L = normalize(pointLights.lights[i].diffuseColor.xyz - inWorldPos);
        Lo += SpecularBRDF(L, V, N, 0.5, roughness);
        Lo += pointLights.lights[i].ambientColor.xyz;
        Lo *= att;
    }

    vec3 color = MaterialColor(pc.baseColorIndex) * 0.02;
    color += Lo;

    // Gamma correct
    color = pow(color, vec3(0.4545));

    outFragColor = vec4(color, 1.0);
}