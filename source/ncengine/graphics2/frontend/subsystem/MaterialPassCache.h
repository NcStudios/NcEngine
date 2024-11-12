#pragma once

#include "MeshRendererRenderState.h"

#include "ncutility/NcError.h"

#include <vector>

namespace nc::graphics
{
struct PassTargets
{
    explicit PassTargets(MaterialPass::type id_)
        : id{id_}
    {
    }

    std::vector<PassTarget> dynamicTargets;
    std::vector<PassTarget> staticTargets;
    std::vector<uint32_t> staticTargetEntities;
    MaterialPass::type id;
};

class MaterialPassCache
{
    public:
        explicit MaterialPassCache(std::span<const MaterialPass::type> passes)
            : m_passTargets{passes.begin(), passes.end()}
        {
        }

        auto BuildState() const -> std::vector<PassRenderState>
        {
            auto states = std::vector<PassRenderState>{};
            states.reserve(m_passTargets.size());
            for (const auto& targets : m_passTargets)
            {
                states.emplace_back(targets.dynamicTargets, targets.staticTargets);
            }

            return states;
        }

        void AddDynamicTarget(MaterialPasses passes, uint32_t instanceIndex, const asset::MeshView& mesh)
        {
            ForEnabledPass(passes, [instanceIndex, &mesh](auto& pass){
                pass.dynamicTargets.emplace_back(instanceIndex, mesh);
            });
        }

        void AddStaticTarget(MaterialPasses passes, uint32_t entityId, uint32_t instanceIndex, const asset::MeshView& mesh)
        {
            ForEnabledPass(passes, [entityId, instanceIndex, &mesh] (auto& pass) {
                pass.staticTargetEntities.push_back(entityId);
                pass.staticTargets.emplace_back(instanceIndex, mesh);
            });
        }

        void RemoveStaticTarget(MaterialPasses passes, uint32_t entityId)
        {
            ForEnabledPass(passes, [entityId](auto& pass){
                const auto index = FindItemIndex(pass.staticTargetEntities, entityId);
                pass.staticTargetEntities[index] = pass.staticTargetEntities.back();
                pass.staticTargetEntities.pop_back();
                pass.staticTargets[index] = pass.staticTargets.back();
                pass.staticTargets.pop_back();
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

        void ClearDynamicTargets()
        {
            for (auto& pass : m_passTargets)
            {
                pass.dynamicTargets.clear();
            }
        }

        void Clear() noexcept
        {
            for (auto& pass : m_passTargets)
            {
                pass.dynamicTargets.clear();
                pass.dynamicTargets.shrink_to_fit();
                pass.staticTargets.clear();
                pass.staticTargets.shrink_to_fit();
                pass.staticTargetEntities.clear();
                pass.staticTargetEntities.shrink_to_fit();
            }
        }

        auto GetTargets(MaterialPass::type pass) const -> const PassTargets&
        {
            const auto pos = std::ranges::find(m_passTargets, pass, &PassTargets::id);
            NC_ASSERT(pos != m_passTargets.end(), "Pass not found");
            return *pos;
        }

    private:
        std::vector<PassTargets> m_passTargets;

        template<class F>
        void ForEnabledPass(MaterialPasses enabledPasses, F func)
        {
            for (auto& pass : m_passTargets)
            {
                if (enabledPasses & pass.id)
                {
                    func(pass);
                }
            }
        }

        static auto FindItemIndex(const std::vector<uint32_t>& items, uint32_t target) -> size_t
        {
            const auto pos = std::ranges::find(items, target);
            NC_ASSERT(pos != items.end(), "Static Entity not registered to MaterialPass.");
            return static_cast<size_t>(std::distance(items.begin(), pos));
        }
};
} // namespace nc::graphics
