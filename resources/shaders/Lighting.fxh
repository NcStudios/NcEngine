struct LightData {
    float3 diffuseColor;
    int type; // 0: Directional, 1: Point, 2: Spot
    float3 specularColor;
    float radius;
    float3 position;
    float innerAngle;
    float3 direction;
    float outerAngle;
    float3 shadowColor;
    int castsShadows;
    float4x4 viewProj;
};

struct LightInfluence
{
    float3 diffuseColor;
    float3 specularColor;
    float3 shadowColor;
    float specularAmt;
    float diffuseAmt;
};

LightInfluence DirectionalLightRadiance(LightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(-light.direction); // Vector from light to fragment
    float normalDotLightVec = pow(saturate(dot(normal, lightVec)), 0.75f); // Light influence is proportional to the angle the light hits the fragment
    float diffuseTotal = normalDotLightVec;

    // Specular
    float3 cameraVec = normalize(cameraPosition - fragWorldPos); // Vector from camera to fragment
    float3 reflectVec = reflect(-lightVec, normal); // Vector of reflected light to normal
    float specular = pow(saturate(dot(cameraVec, reflectVec)), 32);
    float specularTotal = specular * 0.5f;

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor, light.shadowColor, specularTotal, diffuseTotal};
    return lightInfluence;
}

LightInfluence PointLightRadiance(LightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(light.position - fragWorldPos); // Vector from light to fragment
    float normalDotLightVec = pow(saturate(dot(normal, lightVec)), 0.75f); // Light influence is proportional to the angle the light hits the fragment
    float diffuseTotal = normalDotLightVec;

     // Apply smoothstep for midtone emphasis
    diffuseTotal = 0.3 * (1.0 - cos(diffuseTotal * 3.14159)); // Apply gamma correction (value < 1 boosts midtones)


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

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor, light.shadowColor, specularTotal, diffuseTotal};
    return lightInfluence;
}

LightInfluence SpotLightRadiance(LightData light, float3 fragWorldPos, float3 cameraPosition, float3 normal)
{
    // Diffuse
    float3 lightVec = normalize(light.position - fragWorldPos); // Vector from light to fragment
    float normalDotLightVec = pow(saturate(dot(normal, lightVec)), 0.75f); // Light influence is proportional to the angle the light hits the fragment
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

    LightInfluence lightInfluence = {light.diffuseColor, light.specularColor, light.shadowColor, specularTotal, diffuseTotal};
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
