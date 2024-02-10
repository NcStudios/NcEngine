#include "ShaderResourceBus.h"
#include "UniformBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include <ranges>

namespace nc::graphics
{
auto ShaderResourceBus::CreateUniformBuffer(std::string uid, const char* data, uint32_t slot, shader_stage stage) -> UniformBufferHandle
{
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        uniformBufferChannel.Emit(
            UboUpdateEventData
            {
                uid,
                i,
                slot,
                data,
                stage,
                UboUpdateAction::Initialize
            }
        );
    }
    
    return UniformBufferHandle(std::move(uid), slot, stage, uniformBufferChannel);
}
}