#pragma once

#include "asset/AssetData.h"
#include "graphics/api/vulkan/buffers/CubeMapArrayBuffer.h"
#include "graphics/api/vulkan/buffers/MeshArrayBuffer.h"
#include "graphics/api/vulkan/buffers/PPImageArrayBuffer.h"
#include "graphics/api/vulkan/buffers/StorageBuffer.h"
#include "graphics/api/vulkan/buffers/TextureArrayBuffer.h"
#include "graphics/api/vulkan/buffers/UniformBuffer.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "ncengine/type/StableAddress.h"
#include "ncengine/utility/Signal.h"
#include "ncengine/utility/SparseMap.h"

#include <string>
#include <memory>
#include <vector>

namespace nc::graphics
{
struct CabUpdateEventData;
struct MabUpdateEventData;
struct PpiaUpdateEventData;
struct SsboUpdateEventData;
struct TabUpdateEventData;
struct UboUpdateEventData;

namespace vulkan
{
class GpuAllocator;
class RenderGraph;
class ShaderBindingManager;

struct MeshArrayBufferStorage
{
    MeshArrayBufferStorage(std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets_);
    std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets;
    MeshArrayBuffer buffer;
};

/** @todo: Remove unique_ptr upon resolution of #656 */
using UniqueCabMap = sparse_map<std::unique_ptr<CubeMapArrayBuffer>>;
using UniquePpiaMap = sparse_map<std::unique_ptr<PPImageArrayBuffer>>;
using UniqueSsboMap = sparse_map<std::unique_ptr<StorageBuffer>>;
using UniqueUboMap = sparse_map<std::unique_ptr<UniformBuffer>>;
using UniqueTabMap = sparse_map<std::unique_ptr<TextureArrayBuffer>>;

struct ShaderStorage : StableAddress
{
    ShaderStorage(vk::Device device,
                  GpuAllocator* allocator, 
                  ShaderBindingManager* shaderBindingManager,
                  std::array<vk::CommandBuffer*, MaxFramesInFlight> cmdBuffers,
                  Signal<const CabUpdateEventData&>& onCubeMapArrayBufferUpdate,
                  Signal<const MabUpdateEventData&>& onMeshArrayBufferUpdate,
                  Signal<const PpiaUpdateEventData&>& onPPImageArrayBufferUpdate,
                  Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                  Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                  Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate);

    void UpdateCubeMapArrayBuffer(const CabUpdateEventData& eventData);
    void UpdateMeshArrayBuffer(const MabUpdateEventData& eventData);
    void UpdatePPImageArrayBuffer(const PpiaUpdateEventData& eventData);
    void UpdateStorageBuffer(const SsboUpdateEventData& eventData);
    void UpdateUniformBuffer(const UboUpdateEventData& eventData);
    void UpdateTextureArrayBuffer(const TabUpdateEventData& eventData);
    void SinkPostProcessImages(std::vector<vk::ImageView> postProcessImages, PostProcessImageType imageType, uint32_t frameIndex);

    private:

    vk::Device m_device;
    GpuAllocator* m_allocator;
    ShaderBindingManager* m_shaderBindingManager;
    RenderGraph* m_renderGraph;

    std::array<UniqueCabMap, MaxFramesInFlight> m_perFrameCabStorage;
    UniqueCabMap m_staticCabStorage;
    nc::Connection<const CabUpdateEventData&> m_onCubeMapArrayBufferUpdate;

    MeshArrayBufferStorage m_staticMabStorage;
    nc::Connection<const MabUpdateEventData&> m_onMeshArrayBufferUpdate;

    std::array<UniquePpiaMap, MaxFramesInFlight> m_perFramePpiaStorage;
    nc::Connection<const PpiaUpdateEventData&> m_onPPImageArrayBufferUpdate;

    std::array<UniqueSsboMap, MaxFramesInFlight> m_perFrameSsboStorage;
    UniqueSsboMap m_staticSsboStorage;
    nc::Connection<const SsboUpdateEventData&> m_onStorageBufferUpdate;

    std::array<UniqueUboMap, MaxFramesInFlight> m_perFrameUboStorage;
    UniqueUboMap m_staticUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;

    UniqueTabMap m_staticTabStorage;
    nc::Connection<const TabUpdateEventData&> m_onTextureArrayBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
