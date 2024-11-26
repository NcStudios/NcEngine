#include "MaterialPassBackend.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
auto ToDrawAttribs(const nc::graphics::PassTarget& target) -> Diligent::DrawIndexedAttribs
{
    constexpr auto drawFlags = Diligent::DRAW_FLAG_VERIFY_ALL |
                               Diligent::DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;

    return Diligent::DrawIndexedAttribs{
        target.indexCount,
        Diligent::VT_UINT32,
        drawFlags,
        1,
        target.firstIndex,
        target.firstVertex,
        target.instance
    };
}

void DrawIndexed(Diligent::IDeviceContext& context, const std::vector<nc::graphics::PassTarget>& targets)
{
    for (const auto& target : targets)
    {
        context.DrawIndexed(ToDrawAttribs(target));
    }
}
} // anonymous namespace

namespace nc::graphics
{
void MaterialPassBackend::Render(Diligent::IDeviceContext& context,
                                 const std::vector<PassRenderState>& passStates)
{
    NC_ASSERT(m_passes.size() == passStates.size(), "Frontend/Backend passes out of sync.");
    for (auto [pass, state] : std::views::zip(m_passes, passStates))
    {
        context.SetPipelineState(pass.pso);
        DrawIndexed(context, state.dynamicTargets);
        DrawIndexed(context, state.staticTargets);
    }
}
} // namespace nc::graphics
