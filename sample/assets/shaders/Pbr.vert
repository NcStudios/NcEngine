#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
 
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

struct PointLight
{
    mat4 lightViewProj;
    vec3 lightPos;
    int castShadows;
    vec3 ambientColor;
    float attLin;
    vec3 diffuseColor;
    float attQuad;
    float diffuseIntensity;
    int isInitialized;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

layout (location = 0) out vec3 outViewPosition;
layout (location = 1) out vec3 outFragPosition;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec2 outUV;
layout (location = 4) out mat3 outTBN;
layout (location = 7) out int  outObjectInstance;
layout (location = 8) out vec3 outUVW;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
    ObjectData object = objectBuffer.objects[gl_BaseInstance];

    outFragPosition = vec3(object.model * vec4(inPos, 1.0));
    outNormal = mat3(object.model) * inNormal;
    vec3 N = normalize(vec3(object.model * vec4(inNormal,    0.0)));
    vec3 T = normalize(vec3(object.model * vec4(inTangent,   0.0)));
    vec3 B = normalize(cross(N, T));
    outTBN = mat3(T, B, N);
    outUV = inUV;
    outObjectInstance = gl_BaseInstance;
    outUVW = inPos;
    gl_Position = object.viewProjection * object.model * vec4(inPos, 1.0);
}