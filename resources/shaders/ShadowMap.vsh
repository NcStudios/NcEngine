struct VSInput
{
    // Vertex attributes
    float3 Pos    : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV     : ATTRIB2;
};

struct PSInput 
{
    float4 Pos : SV_POSITION;
};

struct TransformData
{
    float4x4 model;
};

struct LightData {
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
    float4x4 viewProj;
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<LightData> Lights;

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<StaticMeshInstanceData> StaticInstances;

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
    int lightIndex;
};

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    PSIn.Pos = mul(TransformedPos, Lights[lightIndex].viewProj);
}
