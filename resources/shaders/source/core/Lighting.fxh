SamplerState  UniShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
SamplerComparisonState  PointShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
Texture2D     UniShadowMapSinks[];
TextureCube   PointShadowMapSinks[];

StructuredBuffer<LightData> Lights : register(t2);
StructuredBuffer<LightMatrix> LightMatrices;

struct LightInfluence
{
    float3 diffuseColor;
    float3 specularColor;
    float specularAmt;
    float diffuseAmt;
    float intensity;
};

float CalculateDiffuse(float3 L, float3 N)
{
    return max(dot(N, L), 0.0f);
}

float CalculateSpecular(float3 L, float3 V, float3 N)
{
    float3 H = normalize(L + V);
    float NDotH = max(0, dot(N, H));
    return max(pow(NDotH, 32), 0.0f);
}

float CalculateAttenuation(float D, float R)
{
    float distance = D / R;
    return exp(-8.0f * distance); // Steep and smooth
}

LightInfluence DirectionalLightRadiance(LightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(-light.direction); // Light's direction vector (leading to light)
    float diffuseTotal = CalculateDiffuse(lightVec, normal);

    // Specular
    float3 viewVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float specularTotal = CalculateSpecular(lightVec, viewVec, normal);

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor, specularTotal, diffuseTotal, light.intensity * .1f};
    return lightInfluence;
}

LightInfluence PointLightRadiance(LightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(light.position - fragWorldPos); // Vector from light to fragment
    float diffuseTotal = CalculateDiffuse(lightVec, normal);

    // Specular
    float3 viewVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float specularTotal = CalculateSpecular(lightVec, viewVec, normal);

    // Attenuation
    float distance = length(light.position - fragWorldPos);
    diffuseTotal *= CalculateAttenuation(distance, light.radius);
    specularTotal *= CalculateAttenuation(distance, light.radius);

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor, specularTotal, diffuseTotal, light.intensity};
    return lightInfluence;
}

LightInfluence SpotLightRadiance(LightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(light.position - fragWorldPos); // Vector from light to fragment
    float diffuseTotal = CalculateDiffuse(lightVec, normal);

    // Specular
    float3 viewVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float specularTotal = CalculateSpecular(lightVec, viewVec, normal);

    // Spot Light Cutoff
    float theta = dot(lightVec, normalize(-light.direction));
    float epsilon = max(light.innerAngle - light.outerAngle, 0.0001f);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.0f, 1.0f);

    // Attenuation
    float distance = length(light.position - fragWorldPos);
    diffuseTotal *= CalculateAttenuation(distance, light.radius);
    specularTotal *= CalculateAttenuation(distance, light.radius);

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor,  specularTotal, diffuseTotal, intensity * light.intensity * 5.0f};
    return lightInfluence;
}

LightInfluence LightRadiance(LightData light, float3 fragWorldPos, float3 cameraPos, float3 normal)
{
    if (light.type == 0)
    {
        return DirectionalLightRadiance(light, fragWorldPos, cameraPos, normal);
    }
    else if (light.type == 1)
    {
        return PointLightRadiance(light, fragWorldPos, cameraPos, normal);
    }
    else
    {
        return SpotLightRadiance(light, fragWorldPos, cameraPos, normal);
    }
}

static const float4x4 biasMat = float4x4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0
);

float UniShadowCalculation(bool isDirectional, float4 fragPosLightSpace, Texture2D depthTex)
{
    // Perform perspective divide
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if (projCoords.z > 1.0 || projCoords.z < 0)
    {
        return 0.0f;
    }

    // Flip Y
    projCoords.y = 1-projCoords.y;

    // Get depth of current fragment from light's perspective
    float distance = projCoords.z;
    float shadow = 0.0;

   // Get shadow map sampling radius for soft edges
    uint2 shadowMapSize;
    depthTex.GetDimensions(shadowMapSize.x, shadowMapSize.y);
    float2 texelSize = float2(1.0f / shadowMapSize);

    [unroll]
    for(int x = -2; x <= 2; x++)
    {
        [unroll]
        for(int y = -2; y <= 2; y++)
        {
            // Get closest depth value from light's perspective
            float closestDepth = depthTex.Sample(UniShadowMapSinks_sampler, projCoords.xy + float2(x, y) * texelSize).r;
            if (distance > closestDepth)
            {
                shadow += 1.0;
            }
        }
    }

    shadow /= 36.0f;

    // Falloff as edge of UV Shadow Map is reached
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(projCoords.xy - center);
    float falloff = 1.0 - smoothstep(0.1, 0.5, distFromCenter);
    shadow *= falloff;

    return shadow;
}

float PointShadowCalculation(float4 fragPosWorldSpace, float3 lightPosWorldSpace, TextureCube depthTex)
{
    // Get the direction to sample the cubemap
    float3 lightToFrag = lightPosWorldSpace - fragPosWorldSpace.xyz;
    float distance = length(lightToFrag) / 150.0f;
    float3 sampleDir = -normalize(lightToFrag);

    uint2 shadowMapSize;
    depthTex.GetDimensions(shadowMapSize.x, shadowMapSize.y);
    float2 texelSize = float2(1.0f / shadowMapSize);

    float shadow = 0.0;
    float closestDepth = depthTex.SampleCmpLevelZero(PointShadowMapSinks_sampler, sampleDir, distance - 0.001f);
    return (1-(closestDepth));
}