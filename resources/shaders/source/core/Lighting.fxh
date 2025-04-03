SamplerState  UniShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
SamplerState  PointShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
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

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor, specularTotal, diffuseTotal, light.intensity};
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

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor,  specularTotal, diffuseTotal, intensity * light.intensity};
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

float UniShadowCalculation(float4 fragPosLightSpace, float lightIntensity, Texture2D depthTex)
{
    if (lightIntensity <= 0.0f)
    {
        return 1.0f;
    }

    // Perform perspective divide
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Flip Y
    projCoords.y = 1-projCoords.y;

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float shadow = 0.0;

    // Get shadow map sampling radius for soft edges
    uint shadowMapWidth, shadowMapHeight;
    depthTex.GetDimensions(shadowMapWidth, shadowMapHeight);
    float2 texelSize = float2(1.0 / shadowMapWidth, 1.0 / shadowMapHeight);
    const int sampleRadius = 2;

    [unroll]
    for(int y = -sampleRadius; y <= sampleRadius; y++)
    {
        [unroll]
        for(int x = -sampleRadius; x <= sampleRadius; x++)
        {
            // Get closest depth value from light's perspective
            float closestDepth = depthTex.SampleLevel(UniShadowMapSinks_sampler, 
            projCoords.xy + float2(x, y) * texelSize, 0).r;
            if (currentDepth > closestDepth)
            {
                shadow += 1.0;
            }
        }
    }
    
    shadow /= pow((sampleRadius * 2 + 1), 2);

    if (projCoords.z > 1.0 || projCoords.z < 0)
    {
        shadow = 0.0;
    }

    // Falloff as edge of UV Shadow Map is reached
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(projCoords.xy - center);
    float falloff = 1.0 - smoothstep(0.25, 0.5, distFromCenter);
    shadow *= falloff;

    return shadow;
}

float PointShadowCalculation(float4 fragPosWorldSpace, float3 lightPosWorldSpace, float lightIntensity, TextureCube depthTex)
{
    if (lightIntensity <= 0.0f)
    {
        return 1.0f;
    }

    // Get the direction to sample the cubemap
    float3 lightToFrag = lightPosWorldSpace - fragPosWorldSpace.xyz;
    float distance = length(lightToFrag) / 150.0f;
    float3 sampleDir = -normalize(lightToFrag);

    // Set up params for the blur
    float shadow = 0.0;
    uint shadowMapWidth, shadowMapHeight;
    depthTex.GetDimensions(shadowMapWidth, shadowMapHeight);
    float texelSize = float2(1.0 / shadowMapWidth);
    const int sampleCount = 4;

    float3 offsets[4] = {
        float3(3.0, 0.0, 0.0) * texelSize,
        float3(-3.0, 0.0, 0.0) * texelSize,
        float3(0.0, 3.0, 0.0) * texelSize,
        float3(0.0, -3.0, 0.0) * texelSize,
    };

    // Blur the shadow
    [unroll]
    for (int i = 0; i < sampleCount; i++) 
    {
        float3 perturbedDir = normalize(sampleDir + offsets[i]);
        float closestDepth = depthTex.Sample(PointShadowMapSinks_sampler, perturbedDir).r;
        float gradientAmt = 0.0001f; // Adjust softness
        shadow += smoothstep(closestDepth - gradientAmt, closestDepth + gradientAmt, distance);
    }

    shadow /= float(sampleCount);
    return shadow;
}

// float GetShadowFactor(float4 worldPos)
// {
//     uint uniShadowMapIndex = 0u;
//     uint pointShadowMapIndex = 0u;
//     uint shadowCasterCount = 0u;
//     float shadowFactor = 1.0f;
//     for (int i = 0; i < lightCount; i++)
//     {
//         // Calculate shadows
//         if (Lights[i].castsShadows)
//         {
//             if (Lights[i].type == 1)
//             {
//                 TextureCube depthTex = PointShadowMapSinks[pointShadowMapIndex];
//                 float rawShadow = PointShadowCalculation(worldPos, Lights[i].position, Lights[i].radius, depthTex);
//                 shadowFactor += (rawShadow);
//                 pointShadowMapIndex++;
//             }
//             else
//             {
//                 Texture2D depthTex = UniShadowMapSinks[uniShadowMapIndex];
//                 float rawShadow = UniShadowCalculation(mul(mul(worldPos, LightMatrices[Lights[i].lightMatrixIndex].viewProjection), biasMat), depthTex);
//                 shadowFactor += (rawShadow); 
//                 uniShadowMapIndex++;
//             }
//             shadowCasterCount++;
//         }
//     }
//     shadowFactor /= max(float(shadowCasterCount), 1);
//     return min((1-shadowFactor) + 1.0f, 1.0f);
// }