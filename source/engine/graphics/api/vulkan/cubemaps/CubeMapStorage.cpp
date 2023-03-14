#include "CubeMapStorage.h"
#include "asset/AssetData.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/shaders/CubeMapShaderResource.h"

namespace nc::graphics
{
CubeMapStorage::CubeMapStorage(vk::Device device, GpuAllocator* allocator, Signal<const CubeMapBufferData&>& onCubeMapUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_cubeMaps{},
      m_sampler{graphics::CreateTextureSampler(m_device, vk::SamplerAddressMode::eRepeat)},
      m_onCubeMapUpdate{onCubeMapUpdate.Connect(this, &CubeMapStorage::UpdateBuffer)}
{
}

void CubeMapStorage::UpdateBuffer(const CubeMapBufferData& CubeMapBufferData)
{
    switch (CubeMapBufferData.updateAction)
    {
        case UpdateAction::Load:
        {
            LoadCubeMapBuffer(CubeMapBufferData);
            break;
        }
        case UpdateAction::Unload:
        {
            UnloadCubeMapBuffer(CubeMapBufferData);
            break;
        }
        case UpdateAction::UnloadAll:
        {
            UnloadAllCubeMapBuffer();
            break;
        }
    }
}

void CubeMapStorage::LoadCubeMapBuffer(const CubeMapBufferData& cubeMapBufferData)
{
    for (auto i = 0u; i < cubeMapBufferData.ids.size(); ++i)
    {
        const auto& cubeMapData = cubeMapBufferData.data[i];
        m_cubeMaps.emplace_back(m_device, m_allocator, cubeMapData);
    }

    graphics::ShaderResourceService<graphics::CubeMap>::Get()->Update(m_cubeMaps);
}

void CubeMapStorage::UnloadCubeMapBuffer(const CubeMapBufferData& cubeMapBufferData)
{
    const auto& id = cubeMapBufferData.ids[0];
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