#pragma once

#include "CubeMap.h"
#include "utility/Signal.h"

namespace nc
{
namespace asset
{
struct CubeMapUpdateEventData;
} // namespace asset

namespace graphics
{
class GpuAllocator;

class CubeMapStorage
{
    public:
        CubeMapStorage(vk::Device device, GpuAllocator* allocator, Signal<const asset::CubeMapUpdateEventData&>& onCubeMapUpdate);

        void UpdateBuffer(const asset::CubeMapUpdateEventData& eventData);

    private:
        void LoadCubeMapBuffer(const asset::CubeMapUpdateEventData& eventData);
        void UnloadCubeMapBuffer(const asset::CubeMapUpdateEventData& eventData);
        void UnloadAllCubeMapBuffer();

        vk::Device m_device;
        GpuAllocator* m_allocator;
        std::vector<CubeMap> m_cubeMaps;
        vk::UniqueSampler m_sampler;
        nc::Connection<const asset::CubeMapUpdateEventData&> m_onCubeMapUpdate;
};
} // namespace graphics
} // namespace nc
