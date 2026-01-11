SamplerComparisonState  UniShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
SamplerState  PointShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
Texture2D     UniShadowMapSinks[];
TextureCube   PointShadowMapSinks[];

StructuredBuffer<LightData> Lights : register(t2);
StructuredBuffer<LightMatrix> LightMatrices;
StructuredBuffer<CascadeData> Cascades;

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

float UniShadowCalculation(bool isDirectional, float4 fragPosLightSpace, Texture2D depthTex, float3 normal, float3 lightDir)
{
    // Perform perspective divide
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if (projCoords.z > 1.0 || projCoords.z < 0)
    {
        return 0.0f;
    }

    // Flip Y
    projCoords.y = 1-projCoords.y;

    // Calculate adaptive bias based on surface orientation to light
    // Surfaces at grazing angles need more bias to avoid shadow acne
    float cosTheta = saturate(dot(normal, lightDir));
    float bias = 0.005 * sqrt(1.0 - cosTheta * cosTheta) / max(cosTheta, 0.01);
    bias = clamp(bias, 0.001, 0.02);

    // Get depth of current fragment from light's perspective
    float distance = projCoords.z - bias;
    float shadow = 1-depthTex.SampleCmpLevelZero(UniShadowMapSinks_sampler, projCoords.xy, distance);

    // Falloff as edge of UV Shadow Map is reached
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(projCoords.xy - center);
    float falloff = 1.0 - smoothstep(0.1, 0.5, distFromCenter);
    shadow *= falloff;

    return shadow;
}

// CSM helper: Select cascade based on view-space depth
uint SelectCascade(float viewDepth, uint cascadeStartIndex, uint cascadeCount)
{
    for (uint i = 0; i < cascadeCount; ++i)
    {
        if (viewDepth < Cascades[cascadeStartIndex + i].splitDepth)
        {
            return i;
        }
    }
    return cascadeCount - 1;
}

// Helper: Sample shadow from a specific cascade
float SampleCascadeShadow(
    float3 worldPos,
    float3 normal,
    float3 lightDir,
    uint cascadeDataIndex,
    uint shadowMapIndex)
{
    CascadeData cascade = Cascades[cascadeDataIndex];

    // Normal offset: Move sampling position along the normal to handle grazing angles
    // This prevents peter panning at extreme light angles better than depth bias alone
    float cosTheta = saturate(dot(normal, lightDir));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Scale normal offset by texel size (stored in cascade data) for consistent results
    // At grazing angles (sinTheta -> 1), we need more offset
    float normalOffsetScale = cascade.texelSize * sinTheta * 2.5f;
    float3 offsetPos = worldPos + normal * normalOffsetScale;

    // Transform world position to light space for selected cascade
    float4 lightSpacePos = mul(float4(offsetPos, 1.0), cascade.viewProjection);
    lightSpacePos = mul(lightSpacePos, biasMat);

    // Perspective divide
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;

    // Early out if outside shadow map
    if (projCoords.z > 1.0 || projCoords.z < 0.0)
    {
        return 0.0;
    }

    // Flip Y for DirectX texture coordinates
    projCoords.y = 1.0 - projCoords.y;

    // Small constant depth bias (normal offset handles most of the work now)
    float constBias = 0.003;

    // Minimal slope bias for remaining acne at moderate angles
    float slopeBias = 0.0005 * sinTheta / max(cosTheta, 0.1);
    slopeBias = clamp(slopeBias, 0.0, 0.001);

    float bias = constBias + slopeBias;

    float distance = projCoords.z - bias;

    float shadow = 0.0;
    // PCF 3x3 sampling for far cascades
    float2 texelSize = float2(1.0 / shadowMapResolution, 1.0 / shadowMapResolution);
    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += UniShadowMapSinks[shadowMapIndex].SampleCmpLevelZero(
                UniShadowMapSinks_sampler,
                projCoords.xy + offset,
                distance
            );
        }
    }
    shadow /= 9.0;

    // Invert: SampleCmp returns 1 when pass (lit), we want shadow amount
    shadow = 1.0 - shadow;

    // // Edge falloff
    // float2 center = float2(0.5, 0.5);
    // float distFromCenter = length(projCoords.xy - center);
    // float falloff = 1.0 - smoothstep(0.4, 0.5, distFromCenter);
    // shadow *= falloff;

    return shadow;
}

// CSM shadow calculation for directional lights with cascade blending
float CascadedShadowCalculation(
    float3 worldPos,
    float3 normal,
    float3 lightDir,
    float viewDepth,
    uint cascadeStartIndex,
    uint cascadeCount,
    uint shadowMapBaseIndex)
{
    // Select appropriate cascade
    uint cascadeIndex = SelectCascade(viewDepth, cascadeStartIndex, cascadeCount);

    // Sample shadow from current cascade
    float shadow = SampleCascadeShadow(
        worldPos, normal, lightDir,
        cascadeStartIndex + cascadeIndex,
        shadowMapBaseIndex + cascadeIndex
    );

    // Blend with next cascade near the boundary
    if (cascadeIndex < cascadeCount - 1)
    {
        float currentSplitDepth = Cascades[cascadeStartIndex + cascadeIndex].splitDepth;
        float blendRegion = currentSplitDepth * 0.3; // blend region

        if (viewDepth > currentSplitDepth - blendRegion)
        {
            // Sample from next cascade
            float nextShadow = SampleCascadeShadow(
                worldPos, normal, lightDir,
                cascadeStartIndex + cascadeIndex + 1,
                shadowMapBaseIndex + cascadeIndex + 1
            );

            // Blend factor: 0 at start of blend region, 1 at cascade boundary
            float blendFactor = (viewDepth - (currentSplitDepth - blendRegion)) / blendRegion;
            blendFactor = smoothstep(0.0, 1.0, blendFactor);

            shadow = lerp(shadow, nextShadow, blendFactor);
        }
    }

    return shadow;
}

float PointShadowCalculation(float4 fragPosWorldSpace, float3 lightPosWorldSpace, TextureCube depthTex, float3 normal)
{
    // Get sample vector (light to frag dir)
    float3 lightToFrag = lightPosWorldSpace - fragPosWorldSpace.xyz;
    float distance = length(lightToFrag);

    // Normalize the distance based on the far plane (Keep in sync with LightSubsystem.cpp)
    float farPlane = 150.0f;
    distance = distance / farPlane;
    float3 sampleDir = -normalize(lightToFrag);

    // PCF and bias
    float shadow = 0.0f;
    const float sampleCount = 4.0f;
    const float offset = 0.005f;
    float bias = 0.005f * (1.0f - dot(normal, -sampleDir));
    bias = clamp(bias, 0.001f, 0.01f);

    float totalSamples = 0.0f;
    [unroll]
    for (float x = -offset; x <= offset; x += offset / (sampleCount * 0.5f))
    {
        [unroll]
        for (float y = -offset; y <= offset; y += offset / (sampleCount * 0.5f))
        {
            [unroll]
            for (float z = -offset; z <= offset; z += offset / (sampleCount * 0.5f))
            {
                float closestDepth = depthTex.Sample(PointShadowMapSinks_sampler, sampleDir + float3(x, y, z)).r;
                if (distance - bias > closestDepth)
                {
                    shadow += 1.0f;
                }
                totalSamples += 1.0f;
            }
        }
    }

    // Normalize the shadow value
    shadow /= totalSamples;
    return shadow;
}