Texture2D tex;
SamplerState splr;

cbuffer LightBuffer
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
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


float4 main(VSOut vertexOutput) : SV_Target
{
    const float3 vToL = lightPos - vertexOutput.positionFromCamera;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    const float3 materialColor = {1.0f, 1.0f, 1.0f};

    //attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL) );
    
    //diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, vertexOutput.normal));

    const float3 w = vertexOutput.normal * dot(vToL, vertexOutput.normal);
    const float3 r = w * 2.0f - vToL;
    const float3 specular = att * (diffuseColor * diffuseIntensity) * 1.0f * pow( max(0.0f, dot(normalize(-r), normalize(vertexOutput.positionFromCamera) ) ), 1.0f);

    return float4(saturate(diffuse + ambient + specular) * materialColor * tex.Sample(splr, vertexOutput.uv), 1.0f);

}
