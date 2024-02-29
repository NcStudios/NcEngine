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
#include "utility/Signal.h"

#include <string>
#include <memory>
#include <vector>

namespace nc::graphics
{
struct CabUpdateEventData;
class GpuAllocator;
struct MabUpdateEventData;
struct PpiaUpdateEventData;
class RenderGraph;
class ShaderBindingManager;
struct SsboUpdateEventData;
struct TabUpdateEventData;
struct UboUpdateEventData;

namespace vulkan
{
struct CubeMapArrayBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<CubeMapArrayBuffer>> buffers;
};

struct MeshArrayBufferStorage
{
    MeshArrayBufferStorage(std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets_);
    std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets;
    MeshArrayBuffer buffer;
};

struct PPImageArrayBufferStorage
{
    std::unordered_map<PostProcessImageType, std::unique_ptr<PPImageArrayBuffer>> buffers;
};

struct UniformBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<UniformBuffer>> buffers;
};

struct StorageBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<StorageBuffer>> buffers;
};

struct TextureArrayBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<TextureArrayBuffer>> buffers;
};

struct ShaderStorage : StableAddress
{
    ShaderStorage(vk::Device device,
                  GpuAllocator* allocator, 
                  ShaderBindingManager* shaderBindingManager,
                  RenderGraph* renderGraph,
                  std::array<vk::CommandBuffer*, MaxFramesInFlight> cmdBuffers,
                  Signal<const CabUpdateEventData&>& onCubeMapArrayBufferUpdate,
                  Signal<const MabUpdateEventData&>& onMeshArrayBufferUpdate,
                  Signal<const graphics::PpiaUpdateEventData&>& onPPImageArrayBufferUpdate,
                  Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                  Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                  Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate);

    void UpdateCubeMapArrayBuffer(const CabUpdateEventData& eventData);
    void UpdateMeshArrayBuffer(const MabUpdateEventData& eventData);
    void UpdatePPImageArrayBuffer(const graphics::PpiaUpdateEventData& eventData);
    void UpdateStorageBuffer(const SsboUpdateEventData& eventData);
    void UpdateUniformBuffer(const UboUpdateEventData& eventData);
    void UpdateTextureArrayBuffer(const TabUpdateEventData& eventData);

    vk::Device m_device;
    GpuAllocator* m_allocator;
    ShaderBindingManager* m_shaderBindingManager;
    RenderGraph* m_renderGraph;

    std::array<CubeMapArrayBufferStorage, MaxFramesInFlight> m_perFrameCabStorage;
    CubeMapArrayBufferStorage m_staticCabStorage;
    nc::Connection<const CabUpdateEventData&> m_onCubeMapArrayBufferUpdate;

    MeshArrayBufferStorage m_staticMabStorage;
    nc::Connection<const MabUpdateEventData&> m_onMeshArrayBufferUpdate;

    std::array<PPImageArrayBufferStorage, MaxFramesInFlight> m_perFramePpiaStorage;
    nc::Connection<const graphics::PpiaUpdateEventData&> m_onPPImageArrayBufferUpdate;

    std::array<StorageBufferStorage, MaxFramesInFlight> m_perFrameSsboStorage;
    StorageBufferStorage m_staticSsboStorage;
    nc::Connection<const SsboUpdateEventData&> m_onStorageBufferUpdate;

    std::array<UniformBufferStorage, MaxFramesInFlight> m_perFrameUboStorage;
    UniformBufferStorage m_staticUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;

    TextureArrayBufferStorage m_staticTabStorage;
    nc::Connection<const TabUpdateEventData&> m_onTextureArrayBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
