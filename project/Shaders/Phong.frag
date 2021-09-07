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
    vec3 lightPos;
    vec3 ambientColor;
    vec3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    int isInitialized;
    float padding[15];
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
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in mat3 inTBN;
layout (location = 6) in flat int inObjectInstance;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(int textureIndex)
{
   return vec3(texture(sampler2D(textures[textureIndex], smplr), inUV));
}

const float specularPower = 32.0;
const float specularIntensity = 0.6;

vec3 CalculatePointLight(PointLight light, vec3 calculatedNormal, vec3 baseColor, vec3 roughnessColor)
{
    const vec3 vToL = light.lightPos - inViewPosition;
    const float distToL = length(vToL);
    const vec3 dirToL = vToL / distToL;

    // Attenuation
    const float att = 1.0 / (light.attConst + light.attLin * distToL + light.attQuad * (distToL * distToL));

    // Diffuse
    const vec3 diffuse = light.diffuseColor.rgb * att * max(0.0, dot(dirToL, calculatedNormal));
    const vec3 w = calculatedNormal * dot(vToL, calculatedNormal);
    const vec3 r = normalize(w * 2.0 - vToL);

    // Specular
    const vec3 viewCamToFrag = normalize(inViewPosition);
    const vec3 specular = att * light.diffuseColor * roughnessColor.rrr * light.diffuseIntensity * pow(max(0.0, dot(-r, viewCamToFrag)), specularPower);

    return clamp(((diffuse + light.ambientColor) * baseColor + specular), 0.0, 1.0);
}

void main() 
{
    vec3 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex);
    vec3 normalColor = MaterialColor(objectBuffer.objects[inObjectInstance].normalIndex);
    vec3 roughnessColor = MaterialColor(objectBuffer.objects[inObjectInstance].roughnessIndex);

    vec3 calculatedNormal =  inNormal;
    calculatedNormal.x = normalColor.x * 2.0 - 1.0;
    calculatedNormal.y = -normalColor.y * 2.0 + 1.0;
    calculatedNormal.z = normalColor.z;

    calculatedNormal = (calculatedNormal + inNormal) / 2;

    vec3 result = vec3(0.0);

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        result += CalculatePointLight(pointLights.lights[i], calculatedNormal, baseColor, roughnessColor);
    }

    outFragColor = vec4(result, 1.0);
}