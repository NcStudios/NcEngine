#include "MaterialPassBackend.h"
#include "ncengine/debug/Profile.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
auto ToDrawAttribs(const nc::graphics::Batch& batch) -> Diligent::DrawIndexedAttribs
{
    constexpr auto drawFlags = Diligent::DRAW_FLAG_VERIFY_ALL |
                               Diligent::DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;

    return Diligent::DrawIndexedAttribs{
        batch.indexCount,
        Diligent::VT_UINT32,
        drawFlags,
        batch.instanceCount,
        batch.firstIndex,
        batch.vertexOffset,
        batch.firstInstance
    };
}

void DrawIndexed(Diligent::IDeviceContext& context, const std::vector<nc::graphics::Batch>& batches)
{
    NC_PROFILE_SCOPE("DrawIndexed()", nc::ProfileCategory::Rendering);
    for (const auto& batch : batches)
    {
        context.DrawIndexed(ToDrawAttribs(batch));
    }
}
} // anonymous namespace

namespace nc::graphics
{
void MaterialPassBackend::Render(Diligent::IDeviceContext& context,
                                 const std::vector<std::vector<Batch>>& staticMeshBatches,
                                 const std::vector<std::vector<Batch>>& skinnedMeshBatches)
{
    NC_PROFILE_SCOPE("MaterialPassBackend::Render()", ProfileCategory::Rendering);
    NC_ASSERT(m_staticMeshPasses.size() == staticMeshBatches.size(), "Frontend/Backend passes out of sync.");
    for (auto [pass, batches] : std::views::zip(m_staticMeshPasses, staticMeshBatches))
    {
        context.SetPipelineState(pass.pso);
        DrawIndexed(context, batches);
    }

    // prob not ok, want to interleave?
    NC_ASSERT(m_skinnedMeshPasses.size() == skinnedMeshBatches.size(), "Frontend/Backend passes out of sync.");
    for (auto [pass, batches] : std::views::zip(m_skinnedMeshPasses, skinnedMeshBatches))
    {
        context.SetPipelineState(pass.pso);
        DrawIndexed(context, batches);
    }
}
} // namespace nc::graphics
