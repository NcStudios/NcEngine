#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/graphics/Material.h"

#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <ranges>
#include <vector>

namespace nc::graphics
{
struct PassTarget
{
    explicit PassTarget(uint32_t instanceIndex, const asset::MeshView& mesh)
        : instance{instanceIndex},
          indexCount{mesh.indexCount},
          firstIndex{mesh.firstIndex},
          firstVertex{mesh.firstVertex}
    {
    }

    void UpdateMesh(const asset::MeshView& mesh)
    {
        indexCount = mesh.indexCount;
        firstIndex = mesh.firstIndex;
        firstVertex = mesh.firstVertex;
    }

    uint32_t instance = UINT32_MAX;
    uint32_t indexCount = UINT32_MAX;
    uint32_t firstIndex = UINT32_MAX;
    uint32_t firstVertex = UINT32_MAX;
};

inline auto ToDrawAttribs(const PassTarget& target) -> Diligent::DrawIndexedAttribs
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


struct Pass
{
    explicit Pass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> state,
                  MaterialPass::type passId)
        : pso{std::move(state)},
          id{passId}
    {
    }

    std::vector<PassTarget> dynamicTargets;
    std::vector<PassTarget> staticTargets;
    std::vector<uint32_t> staticTargetEntities;
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    MaterialPass::type id;
};

// todo: we want some common dispatcher that
// - registers MeshRenderer OnAdd/OnRemove and dispatches to ObjectSystem and MaterialPassSystem (if static)
// - MeshRenderer has ctx pointer to dispatch here on change of mesh/material for static renderers

// todo: I think we have to 'stage' changes or dynamically produce state or something for parallel reasons

class MaterialPassSystem
{
    public:
        explicit MaterialPassSystem(std::vector<Pass> passes)
            : m_passes{std::move(passes)}
        {
        }

        void Draw(Diligent::IDeviceContext& context)
        {
            for (auto& pass : m_passes)
            {
                context.SetPipelineState(pass.pso);
                DrawIndexed(context, pass.dynamicTargets);
                DrawIndexed(context, pass.staticTargets);
            }
        }

        void AddDynamicTarget(MaterialPasses passes, uint32_t instanceIndex, const asset::MeshView& mesh)
        {
            ForEnabledPass(passes, [instanceIndex, &mesh](auto& pass){
                pass.dynamicTargets.emplace_back(instanceIndex, mesh);
            });
        }

        void ClearDynamicTargets()
        {
            for (auto& pass : m_passes)
            {
                pass.dynamicTargets.clear();
            }
        }

        void AddStaticTarget(MaterialPasses passes, uint32_t entityId, uint32_t instanceIndex, const asset::MeshView& mesh)
        {
            ForEnabledPass(passes, [entityId, instanceIndex, &mesh] (auto& pass) {
                pass.staticTargetEntities.push_back(entityId);
                pass.staticTargets.emplace_back(instanceIndex, mesh);
            });
        }

        void UpdateStaticTargetInstance(MaterialPasses passes, uint32_t entityId, uint32_t instanceIndex)
        {
            ForEnabledPass(passes, [entityId, instanceIndex](auto& pass){
                const auto index = FindItemIndex(pass.staticTargetEntities, entityId);
                pass.staticTargets[index].instance = instanceIndex;
            });
        }

        void UpdateStaticTargetMesh(MaterialPasses passes, uint32_t entityId, const asset::MeshView& mesh)
        {
            ForEnabledPass(passes, [entityId, &mesh](auto& pass){
                const auto index = FindItemIndex(pass.staticTargetEntities, entityId);
                pass.staticTargets[index].UpdateMesh(mesh);
            });
        }

        void UpdateStaticTargetPasses(MaterialPasses oldPasses,
                                      MaterialPasses newPasses,
                                      uint32_t entityId,
                                      uint32_t instanceIndex,
                                      const asset::MeshView& mesh)
        {
            // todo: can prob do better, not even sure about params here
            RemoveStaticTarget(oldPasses, entityId);
            AddStaticTarget(newPasses, entityId, instanceIndex, mesh);
        }

        void RemoveStaticTarget(MaterialPasses passes, uint32_t entityId)
        {
            ForEnabledPass(passes, [entityId](auto& pass){
                const auto index = FindItemIndex(pass.staticTargetEntities, entityId);
                pass.staticTargetEntities[index] = pass.staticTargetEntities.back();
                pass.staticTargetEntities.pop_back();
                pass.staticTargets[index] = pass.staticTargets.back();
                pass.staticTargetEntities.pop_back();
            });
        }

        void Clear() noexcept
        {
            for (auto& pass : m_passes)
            {
                pass.dynamicTargets.clear();
                pass.dynamicTargets.shrink_to_fit();
                pass.staticTargets.clear();
                pass.staticTargets.shrink_to_fit();
                pass.staticTargetEntities.clear();
                pass.staticTargetEntities.shrink_to_fit();
            }
        }

        auto GetPass(MaterialPass::type id) -> Pass&
        {
            auto pos = std::ranges::find(m_passes, id, &Pass::id);
            NC_ASSERT(pos != m_passes.end(), "MaterialPass not found");
            return *pos;
        }

    private:
        std::vector<Pass> m_passes;

        template<class F>
        void ForEnabledPass(MaterialPasses enabledPasses, F func)
        {
            for (auto& pass : m_passes)
            {
                if (enabledPasses & pass.id)
                {
                    func(pass);
                }
            }
        }

        static void DrawIndexed(Diligent::IDeviceContext& context,
                                const std::vector<PassTarget>& targets)
        {
            for (const auto& target : targets)
            {
                context.DrawIndexed(ToDrawAttribs(target));
            }
        }

        static auto FindItemIndex(const std::vector<uint32_t>& items, uint32_t target) -> size_t
        {
            const auto pos = std::ranges::find(items, target);
            NC_ASSERT(pos != items.end(), "Static Entity not registered to MaterialPass.");
            return static_cast<size_t>(std::distance(items.begin(), pos));
        }
};
} // namespacae nc::graphics
