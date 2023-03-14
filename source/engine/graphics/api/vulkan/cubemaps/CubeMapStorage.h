#pragma once

#include "CubeMap.h"
#include "utility/Signal.h"

namespace nc
{
struct CubeMapBufferData;

namespace graphics
{
class GpuAllocator;

class CubeMapStorage
{
    public:
        CubeMapStorage(vk::Device device, GpuAllocator* allocator, Signal<const CubeMapBufferData&>& onCubeMapUpdate);

        void UpdateBuffer(const CubeMapBufferData& cubeMapBufferData);

    private:
        void LoadCubeMapBuffer(const CubeMapBufferData& cubeMapBufferData);
        void UnloadCubeMapBuffer(const CubeMapBufferData& cubeMapBufferData);
        void UnloadAllCubeMapBuffer();

        vk::Device m_device;
        GpuAllocator* m_allocator;
        std::vector<CubeMap> m_cubeMaps;
        vk::UniqueSampler m_sampler;
        nc::Connection<const CubeMapBufferData&> m_onCubeMapUpdate;
};
} // namespace graphics
} // namespace nc
