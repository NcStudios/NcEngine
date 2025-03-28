cbuffer EnvironmentProperties
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float3 cameraPosition;
    uint lightCount;
    float nearClip;
    float farClip;
};

cbuffer SinkIndices
{
    int colorRT1;
    int colorRT2;
    int colorRT3;
    int colorRT4;
    int depthRT1;
    int depthRT2;
    int depthRT3;
    uint hasPostProcess;
    uint lightIndex;
    uint lightFaceIndex;
};

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

struct SkinnedMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint boneIndex;
};

struct TransformData
{
    float4x4 model;
};

struct MaterialData
{
    float3 gradientStart;
    uint diffuseTexIndex;
    float3 gradientEnd;
    uint normalTexIndex;
    uint hatchTexIndex;
    float normalIntensity;
    float hatchTiling;
    float gradientAmount;
    float reflectivity;
    uint useTextureNormals;
    uint useFlatShading;
    float padding1;
};

struct BoneData
{
    float4x4 animatedBoneMatrix;
};

struct ParticleData
{
    float4x4 model;
    uint textureIndex;
};

struct LightData
{
    float3 diffuseColor;
    int type; // 0: Directional, 1: Point, 2: Spot
    float3 specularColor;
    float radius;
    float3 position;
    float innerAngle;
    float3 direction;
    float outerAngle;
    float intensity;
    int castsShadows;
    uint lightMatrixIndex;
    uint padding;
};

struct LightMatrix
{
    float4x4 viewProjection;
};

cbuffer WireframeProperties
{
    float4x4 wireframeModelMatrix;
    float4 wireframeColor;
};
