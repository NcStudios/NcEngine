#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/vk/CubeMap.h"

namespace nc::graphics
{
class Base;
class GpuAllocator;

class CubeMapStorage
{
    public:
        CubeMapStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);

        void UpdateBuffer(const CubeMapBufferData& cubeMapBufferData);

    private:
        void LoadCubeMapBuffer(const CubeMapBufferData& cubeMapBufferData);
        void UnloadCubeMapBuffer(const CubeMapBufferData& cubeMapBufferData);
        void UnloadAllCubeMapBuffer();

        Base* m_base;
        GpuAllocator* m_allocator;
        std::vector<CubeMap> m_cubeMaps;
        vk::UniqueSampler m_sampler;
        nc::Connection<const CubeMapBufferData&> m_onCubeMapUpdate;
};
}