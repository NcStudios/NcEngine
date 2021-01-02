cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VSOut
{
    float4 position : SV_POSITION;
};

VSOut main(VSIn input)
{
    VSOut output;
    output.position = mul(float4(input.position, 1.0f), modelViewProj); 
    return output;
}
