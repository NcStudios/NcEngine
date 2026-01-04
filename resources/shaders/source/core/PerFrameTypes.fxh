cbuffer EnvironmentProperties
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float4 primaryColor;
    float4 secondaryColor;
    float4 tertiaryColor;
    float3 cameraPosition;
    uint lightCount;
    float nearClip;
    float farClip;
    uint skyboxIndex;
    uint useSkybox;
    bool useColorOverride;
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

static const uint MAX_CASCADE_COUNT = 4;

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
    uint cascadeStartIndex;  // Index into CascadeData buffer (CSM)
    uint cascadeCount;       // 0 = legacy single shadow map, >0 = CSM
    uint padding[3];
};

struct LightMatrix
{
    float4x4 viewProjection;
};

struct CascadeData
{
    float4x4 viewProjection;
    float splitDepth;        // View-space far depth for this cascade
    float texelSize;         // World-space texel size for bias calculation
    float2 padding;
};

cbuffer WireframeProperties
{
    float4x4 wireframeModelMatrix;
    float4 wireframeColor;
};