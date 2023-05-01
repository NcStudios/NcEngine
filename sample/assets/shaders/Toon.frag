#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
    int overlayIndex;
    int lightShadingIndex;
    int heavyShadingIndex;
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

		// Material data
		vec3 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex, 1);
		float hatchingTexture = MaterialColor(objectBuffer.objects[inObjectInstance].heavyShadingIndex, 8).x;

		// Cel shading levels
		float highlightLevel = 0.85f;
		float midLevel = 0.2f;
		float darkestLevel = 0.0f;
		float blurAmount = 0.05f;
		if (lightIntensity <= darkestLevel + blurAmount)
		{
			baseColor = baseColor * lightColor * mix(darkestLevel, hatchingTexture, (lightIntensity - darkestLevel) * 1/(blurAmount));
		}
		else if (lightIntensity <= midLevel)
		{
			baseColor *= lightColor * hatchingTexture;
		}
		else if (lightIntensity <= midLevel + blurAmount)
		{
			baseColor = baseColor * lightColor * mix(hatchingTexture, 1.0f, (lightIntensity - midLevel) * 1/(blurAmount));
		}
		else if (lightIntensity <= highlightLevel)
		{
			baseColor *= lightColor;
		}
		else if (lightIntensity <= highlightLevel + blurAmount)
		{
			baseColor *=  mix(lightColor, (lightColor + lightAmbient), (lightIntensity - highlightLevel) * 1/(blurAmount));
		}
		else baseColor *= lightColor + lightAmbient;
		result += max(vec3(0.0f), baseColor);
	}
    outFragColor = vec4(result, 1.0f);
}
