cbuffer EnvironmentProperties
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float3 cameraPosition;
    uint lightCount;
    float nearClip;
    float farClip;
    uint skyboxIndex;
    uint useSkybox;
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
    float4 gradientStart;
    float4 gradientEnd;
    float4 primaryColor;
    float4 secondaryColor;
    float4 tertiaryColor;
    float normalIntensity;
    float hatchTiling;
    float gradientAmount;
    float reflectivity;
    uint diffuseTexIndex;
    uint normalTexIndex;
    uint hatchTexIndex;
    bool useTextureNormals;
    bool useFlatShading;
    bool useColorOverride;
    bool useHatchTexture;
    float padding;
};

struct BoneData
{
    float4x4 animatedBoneMatrix;
};

struct ParticleData
{
    float4x4 model;
    float4 color;
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
