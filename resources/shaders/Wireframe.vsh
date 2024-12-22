struct VSInput
{
    float3 Pos : ATTRIB0;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
};

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float3 cameraPosition;
    uint lightCount;
};

cbuffer WireframeBufferData
{
    float4x4 wireframeModelMatrix;
    float4 wireframeColor;
};

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSInput PSIn)
{
    float4 transformedPos = mul(float4(VSIn.Pos, 1.0), wireframeModelMatrix);
    PSIn.Pos = mul(transformedPos, cameraViewProjection);
}
