cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
    float2 tiling;
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
    float3 viewPosition : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

VSOut main(VSIn input)
{
    VSOut output;
    output.viewPosition = (float3)mul(float4(input.position, 1.0f), modelView); // transformation on homogeneous coords
    output.normal = mul(input.normal, (float3x3)modelView); // apply rotation to normals, but not translation
    output.tangent = mul(input.tangent, (float3x3)modelView);
    output.bitangent = mul(input.bitangent, (float3x3)modelView);
    output.position = mul(float4(input.position, 1.0f), modelViewProj); 
    output.uv = float2(input.uv.x * tiling.x, input.uv.y * tiling.y);
    return output;
}
