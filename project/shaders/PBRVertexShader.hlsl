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
};

struct VSOut
{
    float4 position : SV_POSITION;
    float3 positionFromCamera : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

VSOut main(VSIn input)
{
    VSOut output;
    output.position = mul(float4(input.position, 1.0f), modelViewProj); 
    output.positionFromCamera = (float3)mul(float4(input.position, 1.0f), modelView); // transformation on homogeneous coords
    output.normal = mul(input.normal, (float3x3)modelView); // apply rotation to normals, but not translation
    output.uv = input.uv;
    return output;
}
