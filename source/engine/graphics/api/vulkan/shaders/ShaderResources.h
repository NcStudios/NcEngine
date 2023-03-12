#pragma once

#include "CubeMapShaderResource.h"
#include "EnvironmentDataShaderResource.h"
#include "ObjectDataShaderResource.h"
#include "PointLightShaderResource.h"
#include "ShaderDescriptorSets.h"
#include "ShadowMapShaderResource.h"
#include "TextureShaderResource.h"
#include "config/Config.h"


#include "graphics/shader_resource/EnvironmentData.h"
#include "graphics/shader_resource/ShaderResourceBus.h"



namespace nc::graphics
{
struct ShaderResources
{
    ShaderResources(vk::Device device,
                    ShaderDescriptorSets* shaderDescriptorSets,
                    GpuAllocator* allocator,
                    const config::MemorySettings& memorySettings,
                    ShaderResourceBus& resourceBus)
        : objectDataShaderResource{0, allocator, shaderDescriptorSets, memorySettings.maxRenderers},
          pointLightShaderResource{1, allocator, shaderDescriptorSets, memorySettings.maxPointLights},
          textureShaderResource{2, shaderDescriptorSets, memorySettings.maxTextures},
          shadowMapShaderResource{3, device, shaderDescriptorSets, memorySettings.maxPointLights},
          cubeMapShaderResource{4, device, shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
          environmentDataShaderResource{5, allocator, shaderDescriptorSets}
    {
        shaderDescriptorSets->CreateSet(BindFrequency::per_frame);


        // TODO: Hack to get things testable - actually store these somewhere.
        static auto envCon = resourceBus.environmentChannel.Connect([this](const EnvironmentData& data)
        {
            auto buffer = std::vector<EnvironmentData>{};
            buffer.push_back(data);
            environmentDataShaderResource.Update(buffer);
        });

        static auto objCon = resourceBus.objectChannel.Connect([this](const std::vector<ObjectData>& data)
        {
            objectDataShaderResource.Update(data);
        });

        static auto con = resourceBus.pointLightChannel.Connect([this](const std::vector<PointLightData>& data)
        {
            pointLightShaderResource.Update(data);
        });
    }

    ObjectDataShaderResource objectDataShaderResource;
    PointLightShaderResource pointLightShaderResource;
    TextureShaderResource textureShaderResource;
    ShadowMapShaderResource shadowMapShaderResource;
    CubeMapShaderResource cubeMapShaderResource;
    EnvironmentDataShaderResource environmentDataShaderResource;
};
} // namespace nc::graphics