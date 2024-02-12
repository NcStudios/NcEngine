#pragma once

#include "utility/Signal.h"
#include "ShaderTypes.h"

namespace nc::graphics
{
struct ObjectData;
struct PointLightData;
struct SkeletalAnimationData;
struct SsboUpdateEventData;
class StorageBufferHandle;
struct UboUpdateEventData;
class UniformBufferHandle;


/** @brief Data transfer channels for shader resources. Signals are emitted from frontend graphics
 *         systems to be picked up by API-specific backend handlers. */
struct ShaderResourceBus
{
    inline static uint32_t StorageBufferUid = 0u;
    auto CreateStorageBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set) -> StorageBufferHandle;

    inline static uint32_t UniformBufferUid = 0u;
    auto CreateUniformBuffer(size_t size, shader_stage stage, uint32_t slot, uint32_t set) -> UniformBufferHandle;

    Signal<const SsboUpdateEventData&> storageBufferChannel;
    Signal<const UboUpdateEventData&> uniformBufferChannel;
    Signal<const std::vector<ObjectData>&> objectChannel;
    Signal<const std::vector<PointLightData>&> pointLightChannel;
    Signal<const std::vector<SkeletalAnimationData>&> skeletalAnimationChannel;
};
} // namespace nc::graphics
