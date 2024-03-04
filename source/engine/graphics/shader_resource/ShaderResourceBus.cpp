#include "ShaderResourceBus.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/shader_resource/CubeMapArrayBufferHandle.h"
#include "graphics/shader_resource/MeshArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"
#include "graphics/shader_resource/UniformBufferHandle.h"

#include <ranges>

namespace nc::graphics
{
auto ShaderResourceBus::CreateCubeMapArrayBuffer(uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set) -> CubeMapArrayBufferHandle
{
    auto uid = ShaderResourceBus::CubeMapArrayBufferUid++;
        cubeMapArrayBufferChannel.Emit(
            CabUpdateEventData
            {
                std::span<const asset::CubeMapWithId>{},
                uid,
                slot,
                set,
                capacity,
                stage,
                CabUpdateAction::Initialize
            }
        );
    return CubeMapArrayBufferHandle(uid, stage, &cubeMapArrayBufferChannel, slot, set);
}

auto ShaderResourceBus::CreateMeshArrayBuffer() -> MeshArrayBufferHandle
{
    return MeshArrayBufferHandle(&meshArrayBufferChannel);
}

auto ShaderResourceBus::CreatePPImageArrayBuffer(PostProcessImageType imageType, uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set) -> PPImageArrayBufferHandle
{
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        ppImageArrayBufferChannel.Emit(
            PpiaUpdateEventData
            {
                imageType,
                i,
                slot,
                set,
                capacity,
                stage,
                PpiaUpdateAction::Initialize
            }
        );
    }
    return PPImageArrayBufferHandle(imageType, stage, &ppImageArrayBufferChannel, slot, set);
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

auto ShaderResourceBus::CreateTextureArrayBuffer(uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set) -> TextureArrayBufferHandle
{
    auto uid = ShaderResourceBus::TextureArrayBufferUid++;
    textureArrayBufferChannel.Emit(
        TabUpdateEventData
        {
            std::span<const asset::TextureWithId>{},
            uid,
            slot,
            set,
            capacity,
            stage,
            TabUpdateAction::Initialize
        }
    );

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
