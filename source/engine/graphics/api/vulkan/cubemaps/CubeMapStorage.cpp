#include "CubeMapStorage.h"
#include "asset/AssetData.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/shaders/CubeMapShaderResource.h"

namespace nc::graphics
{
CubeMapStorage::CubeMapStorage(vk::Device device, GpuAllocator* allocator, Signal<const asset::CubeMapUpdateEventData&>& onCubeMapUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_cubeMaps{},
      m_sampler{graphics::CreateTextureSampler(m_device, vk::SamplerAddressMode::eRepeat, 1)},
      m_onCubeMapUpdate{onCubeMapUpdate.Connect(this, &CubeMapStorage::UpdateBuffer)}
{
}

void CubeMapStorage::UpdateBuffer(const asset::CubeMapUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            LoadCubeMapBuffer(eventData);
            break;
        }
        case asset::UpdateAction::Unload:
        {
            UnloadCubeMapBuffer(eventData);
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            UnloadAllCubeMapBuffer();
            break;
        }
    }
}

void CubeMapStorage::LoadCubeMapBuffer(const asset::CubeMapUpdateEventData& eventData)
{
    for (auto i = 0u; i < eventData.ids.size(); ++i)
    {
        const auto& cubeMapData = eventData.data[i];
        m_cubeMaps.emplace_back(m_device, m_allocator, cubeMapData);
    }

    graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
}

void CubeMapStorage::UnloadCubeMapBuffer(const asset::CubeMapUpdateEventData& eventData)
{
    const auto& id = eventData.ids[0];
    auto pos = std::ranges::find_if(m_cubeMaps, [&id](const auto& cubeMap)
    {
        return cubeMap.GetUid() == id;
    });

    assert(pos != m_cubeMaps.end());
    m_cubeMaps.erase(pos);
    graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
}

void CubeMapStorage::UnloadAllCubeMapBuffer()
{
    /** No need to write an empty buffer to the GPU. **/
    m_cubeMaps.clear();
}
} // namespace nc::graphics