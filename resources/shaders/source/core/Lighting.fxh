SamplerComparisonState  UniShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
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
    float shadow = 1-depthTex.SampleCmpLevelZero(UniShadowMapSinks_sampler, projCoords.xy, distance);

    // Falloff as edge of UV Shadow Map is reached
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(projCoords.xy - center);
    float falloff = 1.0 - smoothstep(0.1, 0.5, distFromCenter);
    shadow *= falloff;

    return shadow;
}

float PointShadowCalculation(float4 fragPosWorldSpace, float3 lightPosWorldSpace, TextureCube depthTex, float3 normal)
{
    float3 lightToFrag = fragPosWorldSpace.xyz - lightPosWorldSpace;
    float linearDistance = length(lightToFrag);

    // MUST match LightSubsystem.cpp m_pointLightProjection!
    const float nearPlane = 1.0f;
    const float farPlane = 40.0f;

    float3 sampleDir = normalize(lightToFrag);

    // Convert our linear distance to perspective depth (what the hardware depth buffer stores)
    // Inverse of: linearDepth = near*far / (far - perspDepth*(far-near))
    // Solving for perspDepth: perspDepth = (far - near*far/linearDistance) / (far - near)
    float perspectiveDepth = (farPlane - (nearPlane * farPlane) / linearDistance) / (farPlane - nearPlane);
    perspectiveDepth = saturate(perspectiveDepth);

    // Bias in perspective space
    float bias = max(0.002f * (1.0f - dot(normal, -sampleDir)), 0.001f);

    // Hardware comparison with SamplerComparisonState - returns 0 if in shadow, 1 if lit
    float lit = depthTex.SampleCmpLevelZero(PointShadowMapSinks_sampler, sampleDir, perspectiveDepth - bias);

    return 1.0f - lit;  // Return shadow amount (1 = shadowed, 0 = lit)
}
