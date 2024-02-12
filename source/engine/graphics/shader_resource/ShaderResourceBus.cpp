#include "ShaderResourceBus.h"
#include "StorageBufferHandle.h"
#include "UniformBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include <ranges>

namespace nc::graphics
{
auto ShaderResourceBus::CreateStorageBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set) -> StorageBufferHandle
{
    auto uid = ShaderResourceBus::StorageBufferUid++;
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        storageBufferChannel.Emit(
            SsboUpdateEventData
            {
                uid,
                i,
                slot,
                set,
                nullptr,
                size,
                stage,
                SsboUpdateAction::Initialize
            }
        );
    }
    return StorageBufferHandle(uid, stage, storageBufferChannel, slot, set);
}

auto ShaderResourceBus::CreateUniformBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set) -> UniformBufferHandle
{
    auto uid = ShaderResourceBus::UniformBufferUid++;
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        uniformBufferChannel.Emit(
            UboUpdateEventData
            {
                uid,
                i,
                slot,
                set,
                nullptr,
                size,
                stage,
                UboUpdateAction::Initialize
            }
        );
    }
    return UniformBufferHandle(uid, stage, uniformBufferChannel, slot, set);
}
}