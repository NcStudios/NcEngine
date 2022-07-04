#include "assets/Assets.h"
#include "MeshAssetManager.h"

namespace nc
{
        Assets::Assets(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings)
            : m_gpuAccessorChannels{},
              m_meshManager{std::make_unique<MeshAssetManager>(projectSettings.meshesPath, &m_gpuAccessorChannels.onMeshAdd)}
             
        {
        }

        Assets::~Assets() {}

        GpuAccessorChannels* Assets::GpuAccessorChannels()
        {
            return &m_gpuAccessorChannels;
        }
}