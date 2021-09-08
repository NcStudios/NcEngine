#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // Camera world position
    vec3 cameraPos;
} pc;

struct PointLight
{
    mat4 lightViewProj;
    vec3 lightPos;
    float attConst;
    vec3 ambientColor;
    float attLin;
    vec3 diffuseColor;
    float attQuad;
    float diffuseIntensity;
    int isInitialized;
};

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;

    int isInitialized;
};

layout(std140, set=2, binding=0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (set = 0, binding = 0) uniform sampler smplr;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (std140, set=1, binding=0) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (location = 0) in vec3 inViewPosition;
layout (location = 1) in vec4 inLightSpacePosition;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;
layout (location = 4) in mat3 inTBN;
layout (location = 7) in flat int inObjectInstance;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(int textureIndex)
{
   return vec3(texture(sampler2D(textures[textureIndex], smplr), inUV));
}

const float specularPower = 32.0;
const float specularIntensity = 0.6;

layout (set = 3, binding = 0) uniform sampler2D shadowMap;

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, (shadowCoord.st + off) ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = 0.1;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

vec3 CalculatePointLight(PointLight light, vec3 calculatedNormal, vec3 baseColor, vec3 roughnessColor)
{
    const vec3 vToL = light.lightPos - inViewPosition;
    const float distToL = length(vToL);
    const vec3 dirToL = vToL / distToL;

    // Attenuation
    const float att = 1.0 / (light.attConst + light.attLin * distToL + light.attQuad * (distToL * distToL));

    // Diffuse
    const vec3 diffuse = light.diffuseColor.rgb * att * max(0.0, dot(dirToL, calculatedNormal));
    const vec3 w = calculatedNormal * dot(vToL, calculatedNormal);
    const vec3 r = normalize(w * 2.0 - vToL);

    // Specular
    const vec3 viewCamToFrag = normalize(inViewPosition);
    const vec3 specular = att * light.diffuseColor * roughnessColor.rrr * light.diffuseIntensity * pow(max(0.0, dot(-r, viewCamToFrag)), specularPower);

    return (clamp(((diffuse + light.ambientColor * 0.75) * baseColor + specular), 0.0, 1.0));
}

void main() 
{
    vec3 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex);
    vec3 normalColor = MaterialColor(objectBuffer.objects[inObjectInstance].normalIndex);
    vec3 roughnessColor = MaterialColor(objectBuffer.objects[inObjectInstance].roughnessIndex);

    vec3 calculatedNormal =  inNormal;
    calculatedNormal.x = normalColor.x * 2.0 - 1.0;
    calculatedNormal.y = -normalColor.y * 2.0 + 1.0;
    calculatedNormal.z = normalColor.z;

    calculatedNormal = (calculatedNormal + inNormal) / 2;

    vec3 result = vec3(0.0);

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        result += CalculatePointLight(pointLights.lights[i], calculatedNormal, baseColor, roughnessColor);
    }
    const float shadow = clamp(filterPCF(inLightSpacePosition / inLightSpacePosition.w), 0.3, 1.0);

    outFragColor = vec4(result * shadow, 1.0);
}