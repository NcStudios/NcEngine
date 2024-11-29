#include "PostProcessBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

namespace nc::graphics
{
PostProcessBufferResource::PostProcessBufferResource(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     Diligent::IShaderResourceVariable& variable)
{
    // basic init here...
    (void)context;
    (void)device;
    (void)variable;
}

void PostProcessBufferResource::Update(Diligent::IDeviceContext& context,
                                       PostProcessPass::type passId,
                                       std::span<const char* const> data)
{
    auto& buffer = GetBuffer(passId);
    void* mapped = nullptr;
    context.MapBuffer(
        &buffer,
        Diligent::MAP_WRITE,
        Diligent::MAP_FLAG_DISCARD,
        mapped
    );

    std::memcpy(mapped, data.data(), data.size());
    context.UnmapBuffer(&buffer, Diligent::MAP_WRITE);
}
} // namespace nc::graphics
