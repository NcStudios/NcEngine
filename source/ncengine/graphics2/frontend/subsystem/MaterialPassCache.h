// #pragma once

// #include "MeshRendererRenderState.h"

// #include "ncutility/NcError.h"

// #include <vector>

// namespace nc::graphics
// {
// struct PassTargets
// {
//     explicit PassTargets(MaterialPass::type id_)
//         : id{id_}
//     {
//     }

//     std::vector<PassTarget> targets;
//     std::vector<uint32_t> entities;
//     MaterialPass::type id;
// };

// class MaterialPassCache
// {
//     public:
//         explicit MaterialPassCache(std::span<const MaterialPass::type> passes)
//             : m_passTargets{passes.begin(), passes.end()}
//         {
//         }

//         auto BuildState() const -> std::vector<PassRenderState>
//         {
//             auto states = std::vector<PassRenderState>{};
//             states.reserve(m_passTargets.size());
//             for (const auto& pass : m_passTargets)
//             {
//                 states.emplace_back(pass.targets);
//             }

//             return states;
//         }

//         void AddTarget(MaterialPasses passes, uint32_t entityId, uint32_t instanceIndex, const asset::MeshView& mesh)
//         {
//             ForEnabledPass(passes, [entityId, instanceIndex, &mesh] (auto& pass) {
//                 pass.entities.push_back(entityId);
//                 pass.targets.emplace_back(instanceIndex, mesh);
//             });
//         }

//         void RemoveTarget(MaterialPasses passes, uint32_t entityId)
//         {
//             ForEnabledPass(passes, [entityId](auto& pass){
//                 const auto index = FindItemIndex(pass.entities, entityId);
//                 pass.entities[index] = pass.entities.back();
//                 pass.entities.pop_back();
//                 pass.targets[index] = pass.targets.back();
//                 pass.targets.pop_back();
//             });
//         }

//         void UpdateTargetInstance(MaterialPasses passes, uint32_t entityId, uint32_t instanceIndex)
//         {
//             ForEnabledPass(passes, [entityId, instanceIndex](auto& pass){
//                 const auto index = FindItemIndex(pass.entities, entityId);
//                 pass.targets[index].instance = instanceIndex;
//             });
//         }

//         void UpdateTargetMesh(MaterialPasses passes, uint32_t entityId, const asset::MeshView& mesh)
//         {
//             ForEnabledPass(passes, [entityId, &mesh](auto& pass){
//                 const auto index = FindItemIndex(pass.entities, entityId);
//                 pass.targets[index].UpdateMesh(mesh);
//             });
//         }

//         void Clear() noexcept
//         {
//             for (auto& pass : m_passTargets)
//             {
//                 pass.targets.clear();
//                 pass.targets.shrink_to_fit();
//                 pass.entities.clear();
//                 pass.entities.shrink_to_fit();
//             }
//         }

//         auto GetTargets(MaterialPass::type pass) const -> const PassTargets&
//         {
//             const auto pos = std::ranges::find(m_passTargets, pass, &PassTargets::id);
//             NC_ASSERT(pos != m_passTargets.end(), "Pass not found");
//             return *pos;
//         }

//     private:
//         std::vector<PassTargets> m_passTargets;

//         template<class F>
//         void ForEnabledPass(MaterialPasses enabledPasses, F func)
//         {
//             for (auto& pass : m_passTargets)
//             {
//                 if (enabledPasses & pass.id)
//                 {
//                     func(pass);
//                 }
//             }
//         }

//         static auto FindItemIndex(const std::vector<uint32_t>& items, uint32_t target) -> size_t
//         {
//             const auto pos = std::ranges::find(items, target);
//             NC_ASSERT(pos != items.end(), "Static Entity not registered to MaterialPass.");
//             return static_cast<size_t>(std::distance(items.begin(), pos));
//         }
// };
// } // namespace nc::graphics
