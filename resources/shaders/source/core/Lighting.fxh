SamplerComparisonState  UniShadowMapSinks_sampler; // By convention, texture samplers must use the '_sampler' suffix
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