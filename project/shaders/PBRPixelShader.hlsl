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

// float4 main(VSOut vertexShaderOutputs): SV_Target
// {
//     const float3 vToL = lightPos - vertexShaderOutputs.posFromCam;
//     const float distToL = length(vToL);
//     const float3 dirToL = vToL / distToL;
//     const float3 materialColor = {0.9f, 0.0f, 0.2f};

//     //attenuation
//     const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL) );
    
//     //diffuse intensity
//     const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, vertexShaderOutputs.normal));

//     const float3 w = vertexShaderOutputs.normal * dot(vToL, vertexShaderOutputs.normal);
//     const float3 r = w * 2.0f - vToL;
//     const float3 specular = att * (diffuseColor * diffuseIntensity) * 1.0f * pow( max(0.0f, dot(normalize(-r), normalize(vertexShaderOutputs.posFromCam) ) ), 1.0f);

//     return tex.Sample(samplerState, vertexShaderOutputs.posFromCam);
//     //return float4(saturate(diffuse + ambient + specular) * materialColor * tex.Sample(samplerState, vertexShaderOutputs.tex), 1.0f);
// }