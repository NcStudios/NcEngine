#pragma once

#include "CubeMapShaderResource.h"
#include "EnvironmentDataShaderResource.h"
#include "ObjectDataShaderResource.h"
#include "PointLightShaderResource.h"
#include "ShaderDescriptorSets.h"
#include "ShadowMapShaderResource.h"
#include "SkeletalAnimationShaderResource.h"
#include "TextureShaderResource.h"
#include "config/Config.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct ShaderResourceBus;

struct ShaderResources
{
    ShaderResources(vk::Device device,
                    ShaderDescriptorSets* shaderDescriptorSets,
                    GpuAllocator* allocator,
                    const config::MemorySettings& memorySettings,
                    ShaderResourceBus& resourceBus);

    ObjectDataShaderResource objectDataShaderResource;
    PointLightShaderResource pointLightShaderResource;
    TextureShaderResource textureShaderResource;
    ShadowMapShaderResource shadowMapShaderResource;
    CubeMapShaderResource cubeMapShaderResource;
    EnvironmentDataShaderResource environmentDataShaderResource;
    SkeletalAnimationShaderResource skeletalAnimationShaderResource;

    Connection<const std::vector<ObjectData>&> objectDataConnection;
    Connection<const std::vector<PointLightData>&> pointLightDataConnection;
    Connection<const EnvironmentData&> environmentDataConnection;
    Connection<const std::vector<SkeletalAnimationData>&> skeletalAnimationDataConnection;
};
} // namespace nc::graphics
