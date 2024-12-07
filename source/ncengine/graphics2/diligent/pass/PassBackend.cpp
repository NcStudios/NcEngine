#include "PassBackend.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/PostProcessPropertyBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "ncengine/debug/Profile.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
using namespace nc;
using namespace nc::graphics;

auto UpdateBuffer(Diligent::IDeviceContext& context,
                  const PostProcessPassProperties& properties,
                  PostProcessPipelineInstance& instance)
{
    std::visit(
        [&context, &instance](const auto& unpacked){
            NC_ASSERT(instance.buffer.has_value(), "Pass instance does not have a UniformBuffer");
            instance.buffer->Write(context, unpacked);
        },
        properties
    );
}

void EnableInstance(PostProcessEffectId effectId,
                    PostProcessPass& pass)
{
    pass.anyEnabled = true;
    auto instance = std::ranges::find(pass.instances, effectId, &PostProcessPipelineInstance::effectId);
    NC_ASSERT(instance != pass.instances.end(), "Post process pass instance not found");
    instance->enabled = true;
}

void DisableInstance(PostProcessEffectId effectId,
                     PostProcessPass& pass)
{
    auto anyEnabled = false;
    for (auto& instance : pass.instances)
    {
        if (instance.effectId == effectId)
        {
            instance.enabled = false;
        }
        else
        {
            anyEnabled = anyEnabled || instance.enabled;
        }
    }

    pass.anyEnabled = anyEnabled;
}

auto FindInstance(std::vector<PostProcessPass>& passes,
                  PostProcessEffectId effectId,
                  PostProcessPassFlag::type passId) -> PostProcessPipelineInstance&
{
    auto pass = std::ranges::find(passes, passId, &PostProcessPass::id);
    if (pass != passes.end())
    {
        auto instance = std::ranges::find(pass->instances, effectId, &PostProcessPipelineInstance::effectId);
        if (instance != pass->instances.end())
        {
            return *instance;
        }
    }

    NC_ASSERT(false, fmt::format(
        "Post process effect/pass mismatch: '{}'/'{}'.",
        effectId,
        passId
    ));

    std::unreachable();
}

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
PassBackend::PassBackend(Diligent::IRenderDevice& device,
                         Diligent::IDeviceContext& context,
                         Diligent::ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings)
    : m_materialPasses{MakeMaterialPasses
      (
          device,
          swapChain,
          shaderFactory,
          shaderBindings
      )},
      m_postProcessPasses{MakePostProcessPasses
      (
          context,
          device,
          swapChain,
          shaderFactory,
          shaderBindings
      )},
     m_wireframePass{WireframePass
     {
         device,
         swapChain,
         shaderFactory,
         shaderBindings
     }}
{
}

void PassBackend::Update(Diligent::IDeviceContext& context, const PostProcessState& postProcessState)
{
    for (const auto& [effectId, effectPasses, enabled] : postProcessState.toggledEffects)
    {
        for (auto& pass : m_postProcessPasses)
        {
            if (pass.id & effectPasses)
            {
                enabled ? EnableInstance(effectId, pass) : DisableInstance(effectId, pass);
            }
        }
    }

    for (const auto& [effectId, passId, properties] : postProcessState.modifiedProperties)
    {
        UpdateBuffer(context, properties, FindInstance(m_postProcessPasses, effectId, passId));
    }
}

void PassBackend::RenderMaterial(Diligent::IDeviceContext& context,
                                 Diligent::ISwapChain& swapChain,
                                 PerPassResourceSignature& perPassResourceSignature,
                                 const std::vector<std::vector<Batch>>& passBatches)
{
    NC_PROFILE_SCOPE("PassBackend::RenderMaterial()", ProfileCategory::Rendering);
    NC_ASSERT(m_materialPasses.size() == passBatches.size(), "Frontend/Backend passes out of sync.");
    for (auto [pass, batches] : std::views::zip(m_materialPasses, passBatches))
    {
        BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessSinkBufferResource(), pass.colorRTIndex, pass.depthRTIndex);
        context.SetPipelineState(pass.pso);
        DrawIndexed(context, batches);

        if (IsOffScreenTarget(pass.colorRTIndex, pass.depthRTIndex))
        {
            m_lastColorRenderTargetIndex = pass.colorRTIndex;
            m_lastDepthRenderTargetIndex = pass.depthRTIndex;
            context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
        }
    }
}

void PassBackend::RenderPostProcess(Diligent::IDeviceContext& context,
                                    Diligent::ISwapChain& swapChain,
                                    PerPassResourceSignature& perPassResourceSignature,
                                    PostProcessPropertyBufferResource& )
{
    constexpr auto drawAttribs = Diligent::DrawAttribs{4, Diligent::DRAW_FLAG_VERIFY_ALL};
    for (auto& pass : m_postProcessPasses)
    {
        if (!pass.anyEnabled)
        {
            continue;
        }

        BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessSinkBufferResource(), pass.colorRTIndex, pass.depthRTIndex);
        context.SetPipelineState(pass.pso);
        perPassResourceSignature.GetPostProcessSinkIndexBufferResource().Update(context, m_lastColorRenderTargetIndex, m_lastDepthRenderTargetIndex);

        for (auto& instance : pass.instances)
        {
            if (!instance.enabled)
            {
                continue;
            }

            // if (instance.buffer.has_value())
            // {
            //     resource.SetVariable(pass.id, instance.buffer->GetBuffer());
            // }

            context.Draw(drawAttribs);

            if (IsOffScreenTarget(pass.colorRTIndex, pass.depthRTIndex))
            {
                context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
            }
        }
    }
}

void PassBackend::RenderWireframe(Diligent::IDeviceContext& context,
                                  const WireframeRendererRenderState& state)
{
    m_wireframePass.Render(context, state);
}
} // namespace nc::graphics
