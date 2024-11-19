struct DirectionalLightData
{
    float3 color;
    float padding;
    float3 direction;
    float padding2;
};

struct PointLightData
{
    float4x4 viewProj;
    float3 position;
    int castsShadows;
    float3 color;
    float radius;
};

struct SpotLightData
{
    float4x4 viewProj;
    float3 position;
    int castsShadows;
    float3 color;
    float innerAngle;
    float3 direction;
    float outerAngle;
    float3 padding;
    float radius;
};

struct LightInfluence
{
    float3 color;
    float specularAmt;
    float diffuseAmt;
};

LightInfluence DirectionalLightRadiance(DirectionalLightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(-light.direction); // Vector from light to fragment
    float normalDotLightVec = saturate(dot(normal, lightVec)); // Light influence is proportional to the angle the light hits the fragment
    float diffuseTotal = normalDotLightVec;

    // Specular
    float3 cameraVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float3 reflectVec = reflect(-lightVec, normal); // Vector of reflected light to normal
    float specular = pow(saturate(dot(cameraVec, reflectVec)), 32);
    float specularTotal = specular * 0.5f;

    LightInfluence lightInfluence = {light.color, specularTotal, diffuseTotal};
    return lightInfluence;
}

LightInfluence PointLightRadiance(PointLightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(light.position - fragWorldPos); // Vector from light to fragment
    float normalDotLightVec = saturate(dot(normal, lightVec)); // Light influence is proportional to the angle the light hits the fragment
    float diffuseTotal = normalDotLightVec;

    // Specular
    float3 cameraVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float3 reflectVec = reflect(-lightVec, normal); // Vector of reflected light to normal
    float specular = pow(saturate(dot(cameraVec, reflectVec)), 32);
    float specularTotal = specular * 0.5f;

    // Attenuation
    float distance = length(light.position - fragWorldPos);
    float attenuation = saturate(1.0 / pow(max(distance, 0.01), 4.0f)) * pow(light.radius, 3);
    diffuseTotal *= attenuation;
    specularTotal *= attenuation;

    LightInfluence lightInfluence = {light.color, specularTotal, diffuseTotal};
    return lightInfluence;
}

LightInfluence SpotLightRadiance(SpotLightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(light.position - fragWorldPos); // Vector from light to fragment
    float normalDotLightVec = saturate(dot(normal, lightVec)); // Light influence is proportional to the angle the light hits the fragment
    float diffuseTotal = normalDotLightVec;

    // Specular
    float3 cameraVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float3 reflectVec = reflect(-lightVec, normal); // Vector of reflected light to normal
    float specular = pow(saturate(dot(cameraVec, reflectVec)), 32);
    float specularTotal = specular * 0.5f;

    // Spot Light Cutoff
    float theta = dot(lightVec, normalize(-light.direction));
    float epsilon = light.outerAngle - light.innerAngle;
    float intensity = saturate((theta - light.innerAngle) / epsilon);
    diffuseTotal *= intensity;
    specularTotal *= intensity;

    // Attenuation
    float distance = length(light.position - fragWorldPos);
    float attenuation = saturate(1.0 / pow(max(distance, 0.01), 4.0f)) * pow(light.radius, 3);

    diffuseTotal *= attenuation;
    specularTotal *= attenuation;

    LightInfluence lightInfluence = {light.color, specularTotal, diffuseTotal};
    return lightInfluence;
}
