#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncengine/asset/AssetViews.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/utility/SparseMap.h"

#include <ranges>
#include <vector>


#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"


namespace nc::graphics
{

using InstanceId = uint32_t;



// instead of batch, should just make attribs...


// not really sure best way to manage
struct BatchRegion
{
    uint32_t offset;
    uint32_t count;
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t vertexOffset;

    uint32_t capacity;
    uint64_t meshId;
    MaterialPasses passes;
};

struct StagedInstance
{
    uint64_t meshId;
    MaterialPasses passes;
    InstanceId instanceId;
    uint32_t transformIndex;
    MaterialInstanceHandle material;
};

struct StagedBatch
{
    MaterialPasses passes;
    asset::MeshView mesh;
};

class InstanceCache2
{
    public:
        explicit InstanceCache2()
            : m_batches{100}, // todo: don't know
              m_indexLookup{100, 500000}
        {
        }

        //// for testing
        auto IsValidInstance(InstanceId id) const -> bool
        {
            return m_indexLookup.contains(id);
        }

        auto GetInstanceIndex(InstanceId id) const -> uint32_t
        {
            return m_indexLookup.at(id);
        }

        auto GetBatch(uint64_t meshId, MaterialPasses passes) -> BatchRegion&
        {
            auto pos = GetBatchIt(meshId, passes);
            NC_ASSERT(pos != m_batches.end(), "Batch not found");
            return *pos;
        }
        ///

        // todo: why give back 'InstanceId' - just use Entity::Index()??
        auto StageAdd(uint32_t transformIndex,
                         MaterialInstanceHandle materialIndex,
                         MaterialPasses passes,
                         const asset::MeshView& mesh) -> InstanceId
        {
            if (!HasBatchFor(mesh.id, passes))
            {
                m_pendingBatches.emplace_back(passes, mesh);
            }

            const auto id = m_nextId++;
            m_pendingAdditions.emplace_back(mesh.id, passes, id, transformIndex, materialIndex);
            return id;
        }

        void StageRemove(InstanceId id,
                         uint64_t meshId,
                         MaterialPasses passes)
        {
            m_pendingRemovals.emplace_back(meshId, passes, id);
        }

        void CommitPendingChanges()
        {
            auto batchIndex = static_cast<uint32_t>(m_buffer.size()); // ?
            for (const auto& batch : m_pendingBatches)
            {
                m_batches.emplace_back(
                    batchIndex++,
                    0,
                    batch.mesh.firstIndex,
                    batch.mesh.indexCount,
                    batch.mesh.firstVertex,
                    0,
                    batch.mesh.id,
                    batch.passes
                );
            }

            for (const auto& toRemove : m_pendingRemovals)
            {
                const auto instanceIndex = m_indexLookup.at(toRemove.instanceId);
                m_indexLookup.erase(toRemove.instanceId);
                // todo: reclaim - prob don't have too if we key off of entity
                auto& batch = GetBatch(toRemove.meshId, toRemove.passes);
                if (instanceIndex != batch.offset + batch.count)
                {
                    for (auto i = instanceIndex + 1; i < batch.offset + batch.count; ++i)
                    {
                        --m_indexLookup.at(i);
                        m_buffer.at(i - 1) = m_buffer.at(i);
                    }
                }

                --batch.count;
            }

            if (m_pendingAdditions.empty())
            {
                return;
            }


            for (auto& toAdd : m_pendingAdditions)
            {
                const auto& [meshId, passes, instanceId, transformIndex, materialIndex] = toAdd;
            
                auto& batch = GetBatch(meshId, passes);
                const auto instanceIndex = batch.offset + batch.count;
                m_indexLookup.emplace(instanceId, instanceIndex);
                ++batch.count;
                if (batch.count > batch.capacity)
                {
                    ++batch.capacity;
                }

                // weird
                toAdd.instanceId = instanceIndex;

            }

            std::ranges::sort(m_pendingAdditions, [](const auto& lhs, const auto& rhs){
                return lhs.instanceId > rhs.instanceId;
            });

            struct IndexOffsets
            {
                uint32_t insertIndex;
                uint32_t shiftCount;
            };

            auto indexOffsets = std::vector<IndexOffsets>{};
            indexOffsets.reserve(m_pendingAdditions.size());

            m_buffer.resize(m_buffer.size() + m_pendingAdditions.size());

            auto shiftCount = m_pendingAdditions.size();
            auto rngEnd = m_buffer.end();
            for (auto& toAdd : m_pendingAdditions)
            {
                const auto i = toAdd.instanceId;
                indexOffsets.emplace_back(i, (uint32_t)shiftCount);

                auto rngBeg = m_buffer.begin() + i;
                std::ranges::shift_right(rngBeg, rngEnd, shiftCount);
                --shiftCount;
                rngEnd = rngBeg;
                *rngEnd = InstanceData{toAdd.transformIndex, toAdd.material};

            }

            for (auto& i : m_indexLookup.values())
            {
                for (const auto& [insertIndex, shiftBy] : indexOffsets)
                {
                    if (insertIndex < i)
                    {
                        i += shiftBy;
                        break;
                    }
                }
            }

            m_pendingBatches.clear();
            m_pendingAdditions.clear();
            m_pendingRemovals.clear();
        }

        // todo: multi add
        auto AddInstance(uint32_t transformIndex,
                         MaterialInstanceHandle materialIndex,
                         MaterialPasses passes,
                         const asset::MeshView& mesh) -> InstanceId
        {
            if (!HasBatchFor(mesh.id, passes))
            {
                // prob need to add an item to buffer as well
                //   -- maybe not.. insert() below might handle it well
                m_batches.emplace_back(
                    static_cast<uint32_t>(m_buffer.size()),
                    0,
                    mesh.firstIndex,
                    mesh.indexCount,
                    mesh.firstVertex,
                    0,
                    mesh.id,
                    passes
                );
            }

            auto& batch = GetBatch(mesh.id, passes);
            const auto instanceIndex = batch.offset + batch.count;
            const auto id = AssignId(instanceIndex);
            ++batch.count;
            if (batch.count > batch.capacity)
            {
                m_buffer.insert(m_buffer.begin() + instanceIndex, InstanceData{transformIndex, materialIndex});
                ++batch.capacity;
                // increment all batches offsets after the new instance
                for (auto& b : m_batches)
                {
                    if (b.offset > batch.offset)
                        ++b.offset;
                }

                // increment all instance indices after the new instance
                for (auto& i : m_indexLookup.values())
                {
                    if (i > instanceIndex)
                        ++i;
                }

            }
            else
            {
                m_buffer.at(instanceIndex) = InstanceData{transformIndex, materialIndex};
            }

            return id;
        }

        void RemoveInstance(InstanceId id,
                            uint64_t meshId,
                            MaterialPasses passes)
        {
            const auto instanceIndex = m_indexLookup.at(id);
            ReleaseId(id);
            // todo: ...reclaim id
            auto& batch = GetBatch(meshId, passes);
            if (instanceIndex != batch.offset + batch.count)
            {
                for (auto i = instanceIndex + 1; i < batch.offset + batch.count; ++i)
                {
                    --m_indexLookup.at(i);
                    m_buffer.at(i - 1) = m_buffer.at(i);
                }
            }

            --batch.count;
        }

        // todo: (i think)
        // void UpdateInstance(InstanceId id, oldData, newData)


        // todo: don't do whole thing
        auto BuildState() -> BufferUpdateInfo<InstanceData>
        {
            return BufferUpdateInfo<InstanceData>{
                .instances = m_buffer,
                .dirtyRanges = {{0, static_cast<uint32_t>(m_buffer.size())}}
            };
        }

        // todo: we prob want to just make Diligent::XXXAttribs here... but can't be exactly here b/c its in frontend
        auto BuildBatches(std::span<const MaterialPass::type> passes) -> std::vector<std::vector<Batch>>
        {
            NC_PROFILE_SCOPE("InstanceCache::BuildBatches()", ProfileCategory::Rendering);
            auto out = std::vector<std::vector<Batch>>(passes.size(), {});
            for (const auto& batch : m_batches)
            {
                for (auto [i, pass] : std::views::enumerate(passes))
                {
                    if (!(batch.passes & pass))
                        continue;

                    out.at(i).push_back(Batch{
                        batch.offset,
                        batch.count,
                        batch.indexOffset,
                        batch.indexCount,
                        batch.vertexOffset
                    });
                }
            }

            return out;
        }

    private:
        std::vector<InstanceData> m_buffer;
        std::vector<BatchRegion> m_batches;
        sparse_map<uint32_t> m_indexLookup; // maps instanceId -> instanceIndex
        std::vector<StagedBatch> m_pendingBatches;
        std::vector<StagedInstance> m_pendingAdditions;
        std::vector<StagedInstance> m_pendingRemovals;
        uint32_t m_nextId = 0;

        // todo: reclaim
        auto AssignId(uint32_t instanceIndex) -> InstanceId
        {
            const auto id = m_nextId++;
            m_indexLookup.emplace(id, instanceIndex);
            return id;
        }

        void ReleaseId(InstanceId id)
        {
            m_indexLookup.erase(id);
            // todo: reclaim
        }

        auto GetBatchIt(uint64_t meshId, MaterialPasses passes)
        {
            return std::ranges::find_if(
                m_batches,
                [meshId, passes](const auto& batch) {
                    return batch.meshId == meshId && batch.passes == passes;
                }
            );
        }

        auto HasBatchFor(uint64_t meshId, MaterialPasses passes) -> bool
        {
            return GetBatchIt(meshId, passes) != m_batches.end();
        }
};
} // namespace nc::graphics
