Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D roughnessTex : register(t2);
Texture2D metallicTex : register(t3);
SamplerState splr;

cbuffer LightBuffer : register(b0)
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer MaterialProperties : register(b1)
{
    float3 color;
    float specularIntensity;
    float specularPower;
    float xTiling;
    float yTiling;
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

float4 main(VSOut vertexOutput) : SV_Target
{
    const float3 vToL = lightPos - vertexOutput.viewPosition;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    // Build the transform rotation into tangent space
    const float3x3 tanToView =  float3x3 ( normalize(vertexOutput.tangent),
                                           normalize(vertexOutput.bitangent),
                                           normalize(vertexOutput.normal) );

    // normal mapping
    const float3 normalSample = normalTex.Sample(splr, vertexOutput.uv).xyz;
    vertexOutput.normal.x = normalSample.x * 2.0f - 1.0f;
    vertexOutput.normal.y = -normalSample.y * 2.0f + 1.0f;
    vertexOutput.normal.z = normalSample.z;
    vertexOutput.normal = mul(vertexOutput.normal, tanToView);

    //attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL) );
    
    //diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, vertexOutput.normal));
    const float3 w = vertexOutput.normal * dot(vToL, vertexOutput.normal);
    const float3 r = normalize(w * 2.0f - vToL);

    // Specular
    const float3 viewCamToFrag = normalize(vertexOutput.viewPosition);
    const float4 specularSample = roughnessTex.Sample(splr, vertexOutput.uv);
    const float3 specularReflectionColor = specularSample.rgb;
    const float3 specular = att * specularReflectionColor * specularIntensity * pow( max(0.0f, dot(-r, viewCamToFrag) ), specularPower);

    return float4(saturate((diffuse + ambient) * albedoTex.Sample(splr, vertexOutput.uv).rgb * color + specular), 1.0f);
}
