#ifndef CASCADED_SHADOWS_FXH
#define CASCADED_SHADOWS_FXH

// CSM cascade selection based on view-space depth
uint SelectCascade(float viewDepth, uint cascadeStartIndex, uint cascadeCount, StructuredBuffer<CascadeData> cascades)
{
    for (uint i = 0; i < cascadeCount; ++i)
    {
        if (viewDepth < cascades[cascadeStartIndex + i].splitDepth)
        {
            return i;
        }
    }
    return cascadeCount - 1;
}

// Compute view-space depth from world position
float ComputeViewDepth(float3 worldPos, float3 cameraPos, float4x4 viewProjection)
{
    // Transform to view space and return linear depth
    float4 viewPos = mul(float4(worldPos, 1.0), viewProjection);
    return viewPos.w; // View-space Z (linear depth)
}

// Alternative: compute from camera distance (simpler, works well for most cases)
float ComputeViewDepthSimple(float3 worldPos, float3 cameraPos)
{
    return length(worldPos - cameraPos);
}

// CSM shadow sampling with cascade selection
// Returns shadow factor (0 = fully shadowed, 1 = fully lit)
float CascadedShadowCalculation(
    float3 worldPos,
    float3 normal,
    float3 lightDir,
    float viewDepth,
    uint cascadeStartIndex,
    uint cascadeCount,
    uint shadowMapBaseIndex,
    StructuredBuffer<CascadeData> cascades,
    Texture2D shadowMaps[],
    SamplerComparisonState shadowSampler)
{
    // Select appropriate cascade
    uint cascadeIndex = SelectCascade(viewDepth, cascadeStartIndex, cascadeCount, cascades);
    uint shadowMapIndex = shadowMapBaseIndex + cascadeIndex;

    CascadeData cascade = cascades[cascadeStartIndex + cascadeIndex];

    // Transform world position to light space for selected cascade
    float4 lightSpacePos = mul(float4(worldPos, 1.0), cascade.viewProjection);

    // Apply bias matrix
    static const float4x4 biasMat = float4x4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.0, 1.0
    );
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

    // Adaptive bias based on surface orientation and cascade texel size
    float cosTheta = saturate(dot(normal, lightDir));
    float slopeBias = 0.005 * sqrt(1.0 - cosTheta * cosTheta) / max(cosTheta, 0.01);

    // Scale bias by cascade texel size
    float texelBias = cascade.texelSize * 2.0;
    float bias = clamp(slopeBias + texelBias, 0.001, 0.03);

    float distance = projCoords.z - bias;

    // Single sample (no PCF)
    float shadow = shadowMaps[shadowMapIndex].SampleCmpLevelZero(
        shadowSampler,
        projCoords.xy,
        distance
    );

    // Invert: SampleCmp returns 1 when pass (lit), we want shadow factor
    shadow = 1.0 - shadow;

    // Edge falloff to reduce artifacts at cascade boundaries
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(projCoords.xy - center);
    float falloff = 1.0 - smoothstep(0.4, 0.5, distFromCenter);
    shadow *= falloff;

    return shadow;
}

// Cascade blending version for smoother transitions
float CascadedShadowCalculationBlended(
    float3 worldPos,
    float3 normal,
    float3 lightDir,
    float viewDepth,
    uint cascadeStartIndex,
    uint cascadeCount,
    uint shadowMapBaseIndex,
    float blendRegion,
    StructuredBuffer<CascadeData> cascades,
    Texture2D shadowMaps[],
    SamplerComparisonState shadowSampler)
{
    uint cascadeIndex = SelectCascade(viewDepth, cascadeStartIndex, cascadeCount, cascades);

    CascadeData cascade = cascades[cascadeStartIndex + cascadeIndex];

    // Calculate shadow for current cascade
    float4 lightSpacePos = mul(float4(worldPos, 1.0), cascade.viewProjection);

    static const float4x4 biasMat = float4x4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.0, 1.0
    );
    lightSpacePos = mul(lightSpacePos, biasMat);

    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;

    if (projCoords.z > 1.0 || projCoords.z < 0.0)
    {
        return 0.0;
    }

    projCoords.y = 1.0 - projCoords.y;

    float cosTheta = saturate(dot(normal, lightDir));
    float slopeBias = 0.005 * sqrt(1.0 - cosTheta * cosTheta) / max(cosTheta, 0.01);
    float texelBias = cascade.texelSize * 2.0;
    float bias = clamp(slopeBias + texelBias, 0.001, 0.03);

    float distance = projCoords.z - bias;
    uint shadowMapIndex = shadowMapBaseIndex + cascadeIndex;

    float shadow = 1.0 - shadowMaps[shadowMapIndex].SampleCmpLevelZero(
        shadowSampler,
        projCoords.xy,
        distance
    );

    // Blend with next cascade if near boundary
    if (cascadeIndex < cascadeCount - 1)
    {
        float splitDepth = cascade.splitDepth;
        float blendStart = splitDepth * (1.0 - blendRegion);

        if (viewDepth > blendStart)
        {
            // Sample next cascade
            CascadeData nextCascade = cascades[cascadeStartIndex + cascadeIndex + 1];
            float4 nextLightSpacePos = mul(float4(worldPos, 1.0), nextCascade.viewProjection);
            nextLightSpacePos = mul(nextLightSpacePos, biasMat);
            float3 nextProjCoords = nextLightSpacePos.xyz / nextLightSpacePos.w;
            nextProjCoords.y = 1.0 - nextProjCoords.y;

            float nextTexelBias = nextCascade.texelSize * 2.0;
            float nextBias = clamp(slopeBias + nextTexelBias, 0.001, 0.03);
            float nextDistance = nextProjCoords.z - nextBias;

            float nextShadow = 1.0 - shadowMaps[shadowMapBaseIndex + cascadeIndex + 1].SampleCmpLevelZero(
                shadowSampler,
                nextProjCoords.xy,
                nextDistance
            );

            // Blend factor
            float blendFactor = (viewDepth - blendStart) / (splitDepth - blendStart);
            shadow = lerp(shadow, nextShadow, blendFactor);
        }
    }

    // Edge falloff
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(projCoords.xy - center);
    float falloff = 1.0 - smoothstep(0.4, 0.5, distFromCenter);
    shadow *= falloff;

    return shadow;
}