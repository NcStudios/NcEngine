#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/vk/CubeMap.h"

namespace nc::graphics
{
class GpuAllocator;

class CubeMapStorage
{
    public:
        CubeMapStorage(vk::Device device, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);

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
}