#pragma once

#include "utility/Signal.h"
#include "ShaderTypes.h"

namespace nc::graphics
{
struct ObjectData;
struct PointLightData;
struct SkeletalAnimationData;
struct UboUpdateEventData;
class UniformBufferHandle;

/** @brief Data transfer channels for shader resources. Signals are emitted from frontend graphics
 *         systems to be picked up by API-specific backend handlers. */
struct ShaderResourceBus
{
    auto CreateUniformBuffer(std::string uid, const char* data, uint32_t slot, shader_stage stage) -> UniformBufferHandle;

    Signal<const UboUpdateEventData&> uniformBufferChannel;
    Signal<const std::vector<ObjectData>&> objectChannel;
    Signal<const std::vector<PointLightData>&> pointLightChannel;
    Signal<const std::vector<SkeletalAnimationData>&> skeletalAnimationChannel;
};
} // namespace nc::graphics
