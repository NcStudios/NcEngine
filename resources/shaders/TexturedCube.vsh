cbuffer Constants
{
	float4x4 g_WorldViewProj;
};

struct VSInput
{
	float3 Pos : ATTRIB0;
	float2 UV  : ATTRIB1;
};

struct PSInput
{
	float4 Pos : SV_POSITION;
	float2 UV  : TEX_COORD;
};

void main(in  VSInput VSIn,
		  out PSInput PSIn)
{
	PSIn.Pos = mul(float4(VSIn.Pos, 1.0f), g_WorldViewProj);
	PSIn.UV  = VSIn.UV;
}