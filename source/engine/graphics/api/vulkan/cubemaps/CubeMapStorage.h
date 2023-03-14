#pragma once

#include "CubeMap.h"
#include "utility/Signal.h"

namespace nc
{
struct CubeMapUpdateEventData;

namespace graphics
{
class GpuAllocator;

class CubeMapStorage
{
    public:
        CubeMapStorage(vk::Device device, GpuAllocator* allocator, Signal<const CubeMapUpdateEventData&>& onCubeMapUpdate);

        void UpdateBuffer(const CubeMapUpdateEventData& eventData);

    private:
        void LoadCubeMapBuffer(const CubeMapUpdateEventData& eventData);
        void UnloadCubeMapBuffer(const CubeMapUpdateEventData& eventData);
        void UnloadAllCubeMapBuffer();

        vk::Device m_device;
        GpuAllocator* m_allocator;
        std::vector<CubeMap> m_cubeMaps;
        vk::UniqueSampler m_sampler;
        nc::Connection<const CubeMapUpdateEventData&> m_onCubeMapUpdate;
};
} // namespace graphics
} // namespace nc
