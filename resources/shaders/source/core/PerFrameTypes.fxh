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
    float time;
};

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint shapeKeyMetadataIndex;
    uint vertexOffset;
};

struct SkinnedMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint boneIndex;
    uint shapeKeyMetadataIndex;
    uint vertexOffset;
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

struct ShapeKeyMetadata
{
    int shapeKeyAnimationIndex;
    float durationInSeconds;
    uint numShapeKeys;
    uint padding;
};

cbuffer WireframeProperties
{
    float4x4 wireframeModelMatrix;
    float4 wireframeColor;
};

static const float3 positionsPlane0[] = {
    float3(-1.00000000, 0.00000000, -1.01383543),
    float3( 1.00000000, 0.00000000, -1.01383543),
    float3( 1.00000000, 0.00000000,  1.00000000),
    float3(-1.00000000, 0.184691206,  1.00000000)
};

static const float3 positionsPlane1[] = {
    float3(-1.00000000, 0.0596593395, -1.00000000),
    float3( 1.00000000, 0.00000000, -1.00000000),
    float3( 1.00000000, 0.00000000,  3.00000000),
    float3(-1.00000000, 0.00000000,  1.00000000)
};

static const float3 positionsPlane2[] = {
    float3(-1.00999999, 0.00000000, -1.00999999),
    float3( 1.00999999, -0.106265679, -1.00999999),
    float3( 1.00999999, 0.00000000,  1.04837060),
    float3(-1.00999999, 0.00000000,  1.04837060)
};


static const float3 positions1[] = {
    float3(-1.0, -1.0, -1.0),
    float3(-1.0,  1.0, -1.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0, -1.0,  1.0),
    float3(-1.0, -1.0,  1.0),
    float3(-1.0,  1.0,  1.0),
    float3( 1.0,  1.0,  2.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0,  1.0,  2.0),
    float3( 1.0,  1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0,  1.0, -1.0),
    float3(-1.0,  1.0, -1.0),
    float3(-1.0, -1.0, -1.0),
    float3(-1.0, -1.0,  1.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0, -1.0, -1.0),
    float3(-1.0, -1.0, -1.0),
    float3( 1.0,  1.0,  2.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0,  1.0, -1.0),
    float3( 1.0,  1.0, -1.0),
};

static const float3 positions2[] = {
    float3(-1.0, -1.0, -1.0),
    float3(-1.0,  2.0, -1.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0, -1.0,  1.0),
    float3(-1.0, -1.0,  1.0),
    float3(-1.0,  1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0,  1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0,  1.0, -1.0),
    float3(-1.0,  2.0, -1.0),
    float3(-1.0, -1.0, -1.0),
    float3(-1.0, -1.0,  1.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0, -1.0, -1.0),
    float3(-1.0, -1.0, -1.0),
    float3( 1.0,  1.0,  1.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0,  2.0, -1.0),
    float3( 1.0,  1.0, -1.0)
};
