#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
	mat4 model;
	mat4 modelView;
	mat4 viewProjection;
	int baseColorIndex;
	int overlayIndex;
	int hatchingIndex;
	int hatchingTiling;
};

struct PointLight
{
	mat4 lightViewProj;
	vec3 lightPos;
	int castShadows;
	vec3 ambientColor;
	float attLin;
	vec3 diffuseColor;
	float attQuad;
	float specularIntensity;
	int isInitialized;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
	ObjectData objects[];
} objectBuffer;

layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
	PointLight lights[];
} pointLights;

layout (set = 0, binding = 2) uniform sampler2D textures[];
layout (set = 0, binding = 3) uniform sampler2D shadowMaps[];
layout (set = 0, binding = 4) uniform samplerCube cubeMaps[];
layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
	vec3 cameraWorldPosition;
	int skyboxCubemapIndex;
} environmentData;


layout (location = 0) in vec3 inFragPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in flat int inObjectInstance;
layout (location = 4) in vec3 inStencilPosition;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(int textureIndex, int scale)
{
	return vec3(texture(textures[textureIndex], inUV * scale));
}

vec3 SkyboxColor(int cubeMapIndex, vec3 angleVector)
{
	return vec3(texture(cubeMaps[cubeMapIndex], angleVector));
}

float ShadowCalculation(vec4 fragPosLightSpace, int index)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMaps[index], projCoords.xy).r; 
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// check whether current frag pos is in shadow
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMaps[index], 0);

	for(int x = 0; x <=1; ++x)
	{
		for(int y = 0; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;  

	if (projCoords.z > 1.0 || projCoords.z < 0)
	{
		shadow = 0.0;
	}

	return shadow;
}

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

void main() 
{
	vec3 result = vec3(0.0);

	for (int i = 0; i < pointLights.lights.length(); i++)
	{
		if (pointLights.lights[i].isInitialized == 0) continue;

		// Light data
		PointLight light = pointLights.lights[i];
		vec3 lightDir = normalize(light.lightPos - inFragPosition);
		float lightIntensity = dot(lightDir, normalize(inNormal));
		vec3 lightColor = light.diffuseColor;
		vec3 lightAmbient = light.ambientColor;

		// Shadow
		if (light.castShadows == 1)
		{
			float shadow = 0.0;
			vec4 lightViewPos = vec4(0.0);
			lightViewPos = biasMat * pointLights.lights[i].lightViewProj * vec4(inFragPosition, 1.0);
			shadow = ShadowCalculation(lightViewPos, i);
			lightIntensity -= shadow;
		}

		// Material data
		vec3 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex, 1);
		float hatchingTexture = MaterialColor(objectBuffer.objects[inObjectInstance].hatchingIndex, objectBuffer.objects[inObjectInstance].hatchingTiling).x;

		// Cel shading levels
		float highlightLevel = 0.85f;
		float hatchingLevel = 0.4f;
		float darkestLevel = 0.0f;
		float blurAmount = 0.05f;
		vec3 pixelColor = baseColor;

		if (lightIntensity <= darkestLevel + blurAmount)
		{
			pixelColor = pixelColor * lightColor * mix(darkestLevel, hatchingTexture, (lightIntensity - darkestLevel) * 1/(blurAmount));
		}
		else if (lightIntensity <= hatchingLevel)
		{
			pixelColor *= lightColor * hatchingTexture;
		}
		else if (lightIntensity <= hatchingLevel + blurAmount)
		{
			pixelColor = pixelColor * lightColor * mix(hatchingTexture, 1.0f, (lightIntensity - hatchingLevel) * 1/(blurAmount));
		}
		else if (lightIntensity <= highlightLevel)
		{
			pixelColor *= lightColor;
		}
		else if (lightIntensity <= highlightLevel + blurAmount)
		{
			pixelColor *=  mix(lightColor, (lightColor + lightAmbient), (lightIntensity - highlightLevel) * 1/(blurAmount));
		}
		else pixelColor *= lightColor + lightAmbient;
		result += max(vec3(0.0f), pixelColor);
	}

	if (environmentData.skyboxCubemapIndex > -1)
	{
		// Environment reflection
		vec3 I = normalize(inFragPosition - environmentData.cameraWorldPosition);
		vec3 surfaceNormal = normalize(inNormal);
		vec3 reflected = reflect(I, surfaceNormal);
		vec3 environmentReflectionColor = SkyboxColor(environmentData.skyboxCubemapIndex, reflected);
		result = mix(result, result + environmentReflectionColor, 0.01f);
	}

	// Overlay
	result = mix(result, result * MaterialColor(objectBuffer.objects[inObjectInstance].overlayIndex, 1), 0.25f);
	outFragColor = vec4(result, 1.0f);
}
