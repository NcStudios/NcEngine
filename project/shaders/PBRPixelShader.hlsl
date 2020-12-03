Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D roughnessTex : register(t2);
Texture2D metallicTex : register(t3);
SamplerState splr;

struct LightBuffer
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer LightBuffers : register(b0)
{
    LightBuffer pointLights[4];
}

cbuffer MaterialProperties : register(b1)
{
    float3 color;
    float specularIntensity;
    float specularPower;
};

cbuffer CBuf : register(b2)
{
    matrix modelView;
    matrix modelViewProj;
    float2 tiling;
};

// From Vertex Shader
struct VSOut
{
    float4 position : SV_POSITION;
    float3 viewPosition : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

float3 CalculatePointLight(LightBuffer light, VSOut vertexOutput, float4 albedo, float4 metallic, float4 roughness, float4 normal)
{
    const float3 vToL = light.lightPos - vertexOutput.viewPosition;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    // Attenuation
    const float att = 1.0f / (light.attConst + light.attLin * distToL + light.attQuad * (distToL * distToL) );
    
    // Diffuse intensity
    const float3 diffuse = light.diffuseColor * att * max(0.0f, dot(dirToL, vertexOutput.normal));
    const float3 w = vertexOutput.normal * dot(vToL, vertexOutput.normal);
    const float3 r = normalize(w * 2.0f - vToL);

    // Specular
    const float3 viewCamToFrag = normalize(vertexOutput.viewPosition);
    const float3 specular = att * light.diffuseColor * roughness.rrr * light.diffuseIntensity * pow( max(0.0f, dot(-r, viewCamToFrag) ), specularPower);

    return float4(saturate((diffuse + (light.ambient / 1.5)) * albedo.rgb * color + specular), 1.0f);
}

float4 main(VSOut vertexOutput) : SV_Target
{
    // Get sampled value per pixel for each map
    float4 albedo = albedoTex.Sample(splr, vertexOutput.uv);
    float4 metallic = metallicTex.Sample(splr, vertexOutput.uv);
    float4 roughness = roughnessTex.Sample(splr, vertexOutput.uv);
    float4 normal = normalTex.Sample(splr, vertexOutput.uv);

    // Build the transform rotation into tangent space
    const float3x3 tanToView =  float3x3 ( normalize(vertexOutput.tangent),
                                           normalize(vertexOutput.bitangent),
                                           normalize(vertexOutput.normal) );

    // Normal mapping
    vertexOutput.normal.x = normal.x * 2.0f - 1.0f;
    vertexOutput.normal.y = -normal.y * 2.0f + 1.0f;
    vertexOutput.normal.z = normal.z;
    vertexOutput.normal = mul(vertexOutput.normal, tanToView);

    float3 result;

    for (int i = 0; i < 4; i++)
    {
        result += CalculatePointLight(pointLights[i], vertexOutput, albedo, metallic, roughness, normal);
    }

    return float4(result, albedo.a);
}
