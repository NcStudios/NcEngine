#include "core/PerFrameTypes.fxh"

struct VSInput
{
    float3 Pos : ATTRIB0;
};

struct PSInput 
{
    float4 Pos           : SV_POSITION;
    float3 UVW;
};

static float4x4 localModelMatrix = float4x4(
    100.0f, 0.0f,   0.0f,   0.0f,
    0.0f,   100.0f, 0.0f,   0.0f,
    0.0f,   0.0f,   100.0f, 0.0f,
    0.0f,   0.0f,   0.0f,   1.0f
);

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), localModelMatrix);
    TransformedPos += float4(cameraPosition, 0.0);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.Pos.z = PSIn.Pos.w; // Set z (depth) to equal w so depth becomes 1.0 when doing perspective divide (z/w). (Maximum possible depth)
    PSIn.UVW  = VSIn.Pos;
}
