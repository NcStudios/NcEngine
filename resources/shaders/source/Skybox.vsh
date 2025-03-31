#include "core/PerFrameTypes.fxh"

struct VSInput
{
    // Vertex attributes
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
};

struct PSInput 
{
    float4 Pos           : SV_POSITION;
    float3 UVW;
};


static float4x4 modelMatrix = float4x4(
    100.0f, 0.0f,   0.0f,   0.0f,
    0.0f,   100.0f, 0.0f,   0.0f,
    0.0f,   0.0f,   100.0f, 0.0f,
    0.0f,   0.0f,   0.0f,   1.0f
);

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), modelMatrix);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UVW  = VSIn.Pos;
}
