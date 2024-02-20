#pragma once

#include "utility/Signal.h"
#include "ShaderTypes.h"

namespace nc::graphics
{
struct CabUpdateEventData;
class CubeMapArrayBufferHandle;
struct SsboUpdateEventData;
class StorageBufferHandle;
struct UboUpdateEventData;
class UniformBufferHandle;
struct TabUpdateEventData;
class TextureArrayBufferHandle;


/** @brief Data transfer channels for shader resources. Signals are emitted from frontend graphics
 *         systems to be picked up by API-specific backend handlers. */
struct ShaderResourceBus
{
    inline static uint32_t CubeMapArrayBufferUid = 0u;
    auto CreateCubeMapArrayBuffer(uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> CubeMapArrayBufferHandle;

    inline static uint32_t StorageBufferUid = 0u;
    auto CreateStorageBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> StorageBufferHandle;

    inline static uint32_t TextureArrayBufferUid = 0u;
    auto CreateTextureArrayBuffer(uint32_t capacity, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> TextureArrayBufferHandle;

    inline static uint32_t UniformBufferUid = 0u;
    auto CreateUniformBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set, bool isStatic) -> UniformBufferHandle;

    Signal<const CabUpdateEventData&> cubeMapArrayBufferChannel;
    Signal<const SsboUpdateEventData&> storageBufferChannel;
    Signal<const TabUpdateEventData&> textureArrayBufferChannel;
    Signal<const UboUpdateEventData&> uniformBufferChannel;
};
} // namespace nc::graphics
