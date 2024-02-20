#include "ShaderResourceBus.h"
#include "CubeMapArrayBufferHandle.h"
#include "StorageBufferHandle.h"
#include "TextureArrayBufferHandle.h"
#include "UniformBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include <ranges>

namespace nc::graphics
{
auto ShaderResourceBus::CreateCubeMapArrayBuffer(uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> CubeMapArrayBufferHandle
{
    auto uid = ShaderResourceBus::CubeMapArrayBufferUid++;
    if (isStatic)
    {
        cubeMapArrayBufferChannel.Emit(
            CabUpdateEventData
            {
                std::span<const asset::CubeMapWithId>{},
                std::numeric_limits<uint32_t>::max(),
                uid,
                slot,
                set,
                capacity,
                stage,
                CabUpdateAction::Initialize,
                true
            }
        );
    }
    else
    {
        for (auto i : std::views::iota(0u, MaxFramesInFlight))
        {
            cubeMapArrayBufferChannel.Emit(
                CabUpdateEventData
                {
                    std::span<const asset::CubeMapWithId>{},
                    i,
                    uid,
                    slot,
                    set,
                    capacity,
                    stage,
                    CabUpdateAction::Initialize,
                    false
                }
            );
        }
    }
    return CubeMapArrayBufferHandle(uid, stage, &cubeMapArrayBufferChannel, slot, set);
}

auto ShaderResourceBus::CreateStorageBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> StorageBufferHandle
{
    auto uid = ShaderResourceBus::StorageBufferUid++;
    if (isStatic)
    {
        storageBufferChannel.Emit(
            SsboUpdateEventData
            {
                uid,
                std::numeric_limits<uint32_t>::max(),
                slot,
                set,
                nullptr,
                size,
                stage,
                SsboUpdateAction::Initialize,
                true
            }
        );
    }
    else
    {
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
                    SsboUpdateAction::Initialize,
                    false
                }
            );
        }
    }
    return StorageBufferHandle(uid, size, stage, &storageBufferChannel, slot, set);
}

auto ShaderResourceBus::CreateTextureArrayBuffer(uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> TextureArrayBufferHandle
{
    auto uid = ShaderResourceBus::TextureArrayBufferUid++;
    if (isStatic)
    {
        textureArrayBufferChannel.Emit(
            TabUpdateEventData
            {
                std::span<const asset::TextureWithId>{},
                std::numeric_limits<uint32_t>::max(),
                uid,
                slot,
                set,
                capacity,
                stage,
                TabUpdateAction::Initialize,
                true
            }
        );
    }
    else
    {
        for (auto i : std::views::iota(0u, MaxFramesInFlight))
        {
            textureArrayBufferChannel.Emit(
                TabUpdateEventData
                {
                    std::span<const asset::TextureWithId>{},
                    i,
                    uid,
                    slot,
                    set,
                    capacity,
                    stage,
                    TabUpdateAction::Initialize,
                    false
                }
            );
        }
    }
    return TextureArrayBufferHandle(uid, stage, &textureArrayBufferChannel, slot, set);
}

auto ShaderResourceBus::CreateUniformBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> UniformBufferHandle
{
    auto uid = ShaderResourceBus::UniformBufferUid++;
    if (isStatic)
    {
        uniformBufferChannel.Emit(
            UboUpdateEventData
            {
                uid,
                std::numeric_limits<uint32_t>::max(),
                slot,
                set,
                nullptr,
                size,
                stage,
                UboUpdateAction::Initialize,
                true
            }
        );
    }
    else
    {
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
                    UboUpdateAction::Initialize,
                    false
                }
            );
        }
    }
    return UniformBufferHandle(uid, size, stage, &uniformBufferChannel, slot, set);
}
} // namespace nc::graphics
