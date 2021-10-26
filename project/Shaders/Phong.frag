#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // Camera world position
    vec3 cameraPos;
} pc;

struct PointLight
{
    mat4 lightViewProj;
    vec3 lightPos;
    float attConst;
    vec3 ambientColor;
    float attLin;
    vec3 diffuseColor;
    float attQuad;
    float specularIntensity;
    int isInitialized;
};

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;

    int isInitialized;
};

layout(std140, set=2, binding=0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (set = 0, binding = 0) uniform sampler smplr;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (std140, set=1, binding=0) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (location = 0) in vec3 inViewPosition;
layout (location = 1) in vec3 inFragPosition;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;
layout (location = 4) in mat3 inTBN;
layout (location = 7) in flat int inObjectInstance;
layout (location = 8) in vec4 inLightSpacePosition;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(int textureIndex)
{
   return vec3(texture(sampler2D(textures[textureIndex], smplr), inUV));
}

const float specularPower = 32.0;
const float specularIntensity = 0.6;

layout (set = 3, binding = 0) uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // // perform perspective divide
    // vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    // float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // // get depth of current fragment from light's perspective
    // float currentDepth = projCoords.z;

    // // check whether current frag pos is in shadow
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    float shadow = 0.0;

    if (projCoords.z > 1.0 || projCoords.z < -1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}

float sRGB(float x) {
    if (x <= 0.00031308)
        return 12.92 * x;
    else
        return 1.055*pow(x,(1.0 / 2.4) ) - 0.055;
}
vec3 sRGB_v3(vec3 c) {
    return vec3(sRGB(c.x),sRGB(c.y),sRGB(c.z));
}

vec3 CalculatePointLight(int lightIndex, vec3 normal, vec3 baseColor, vec3 roughnessColor)
{
    PointLight light = pointLights.lights[lightIndex];

    vec3 lightColor = light.diffuseColor;

    // Ambient
    vec3 ambientColor = light.ambientColor;

    // Diffuse
    vec3 lightDir = normalize(light.lightPos - inFragPosition);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(pc.cameraPos - inFragPosition);
    vec3 reflectDir = (-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;

    // Shadow
    float shadow = ShadowCalculation(inLightSpacePosition);
    vec3 lighting = (ambientColor + (1.0 - shadow) * (diffuse + specular)) * baseColor;
    return lighting;
}

void main() 
{
    vec3 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex);
    vec3 normalColor = MaterialColor(objectBuffer.objects[inObjectInstance].normalIndex);
    vec3 roughnessColor = MaterialColor(objectBuffer.objects[inObjectInstance].roughnessIndex);

    vec3 calculatedNormal = normalize(inNormal);
    // vec3 calculatedNormal = sRGB_v3(normalColor);

    vec3 result = vec3(0.0);

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        result += CalculatePointLight(i, calculatedNormal, baseColor, roughnessColor);
    }

    outFragColor = vec4(result, 1.0);
}