#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // N MVP matrices
    mat4 normal; // Transforms the vertex data normals into world space
    mat4 model;
    mat4 viewProjection;

    // Camera world position
    vec3 cameraPos;

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
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    int isInitialized;
};

layout (set = 0, binding = 0) uniform sampler smplr;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (std140, set = 1, binding = 0) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (location = 0) in vec3 inFragWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in mat3 inTBN;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(int textureIndex)
{
   return vec3(texture(sampler2D(textures[textureIndex], smplr), inUV));
}

const float specularIntensity = 0.75;
const float ambientIntensity = 1.0;

vec3 CalculatePointLight(PointLight light, vec3 normal)
{
    // Diffuse
    vec3 lightDir = normalize(light.lightPos.xyz - inFragWorldPos); // LightPos is also in world space.
    float diffuseImpact = max(dot(normal, lightDir), 0.0) * light.diffuseIntensity; // The smaller the angle of light dir, the greater the diffuse impact.
    vec3 diffuseColor = light.diffuseColor.rgb * diffuseImpact;

    // Attenuation
    float lightDistance = length(light.lightPos.xyz - inFragWorldPos);
    float attenuation = 1.0 / (light.attConst + light.attLin * lightDistance + light.attQuad * (lightDistance * lightDistance));

    // Ambient
    vec3 ambientColor = light.ambientColor.rgb * ambientIntensity;

    // Specular/Roughness
    vec3 viewDir = normalize(pc.cameraPos - inFragWorldPos);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float specularImpact = pow(max(dot(viewDir, reflectionDir), 0.0), 100/(500 * MaterialColor(pc.roughnessIndex).r + 0.01));
    vec3 specularColor = specularIntensity * specularImpact * light.specularColor.rgb;

    return max((diffuseColor + ambientColor) * attenuation + specularColor, 0.0);
}

void main() 
{
    vec3 result = vec3(0.025);

    // The normal map texture is in tangent space. The inTBN matrix converts it to world space.
    vec3 normal = MaterialColor(pc.normalIndex);
    normal = normal * 2.0 - 1.0;
    normal = normalize(inTBN * normal);

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        result += CalculatePointLight(pointLights.lights[i], inNormal);
    }

    result *= MaterialColor(pc.baseColorIndex);

    outFragColor = vec4(result, 1.0);
}