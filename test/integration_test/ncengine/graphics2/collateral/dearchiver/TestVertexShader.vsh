struct VSInput
{
    float3 Pos : ATTRIB0;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
};

void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.Pos = float4(VSIn.Pos, 1);
}
