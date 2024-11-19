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

// instead of batch, should just make attribs...

/**
struct BatchKey
{
    MaterialPasses passes;
    uint64_t meshId;
};

struct Batch
{
    uint32_t instanceOffset = UINT32_MAX;
    uint32_t instanceCount = UINT32_MAX;
    uint32_t indexOffset = UINT32_MAX;
    uint32_t indexCount = UINT32_MAX;
    uint32_t vertexOffset = UINT32_MAX;
};

struct BatchRegion
{
    BatchKey key;
    Batch batch;
    uint32_t capacity;
};

*/

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
    uint32_t id; // currently, is entityId when staged, becomes instanceIndex while commiting
    uint32_t transformIndex;
    MaterialInstanceHandle material;
};

struct StagedBatch
{
    MaterialPasses passes;
    asset::MeshView mesh;
};

// todo: would be nice to split out into this, batch list is shared though...
class InstanceStaging
{
    public:

    private:
        std::vector<StagedBatch> m_pendingBatches;
        std::vector<StagedInstance> m_pendingAdditions;
        std::vector<StagedInstance> m_pendingRemovals;
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
        auto IsValidInstance(uint32_t entityId) const -> bool
        {
            return m_indexLookup.contains(entityId);
        }

        auto GetInstanceIndex(uint32_t entityId) const -> uint32_t
        {
            return m_indexLookup.at(entityId);
        }

        auto GetBatch(uint64_t meshId, MaterialPasses passes) -> BatchRegion&
        {
            auto pos = GetBatchIt(meshId, passes);
            NC_ASSERT(pos != m_batches.end(), "Batch not found");
            return *pos;
        }
        ///

        void StageAdd(uint32_t entityId,
                      uint32_t transformIndex,
                      MaterialInstanceHandle materialIndex,
                      MaterialPasses passes,
                      const asset::MeshView& mesh)
        {
            if (!HasBatchFor(mesh.id, passes))
            {
                m_pendingBatches.emplace_back(passes, mesh);
            }

            m_pendingAdditions.emplace_back(mesh.id, passes, entityId, transformIndex, materialIndex);
        }

        void StageRemove(uint32_t entityId,
                         uint64_t meshId,
                         MaterialPasses passes)
        {
            m_pendingRemovals.emplace_back(meshId, passes, entityId);
        }

        // todo: (i think)
        void UpdateInstance(uint32_t entityId,
                            uint32_t transformIndex,
                            MaterialInstanceHandle newMaterialIndex,
                            MaterialPasses oldPasses,
                            MaterialPasses newPasses,
                            uint64_t oldMeshId,
                            const asset::MeshView& newMesh)
        {
            StageRemove(entityId, oldMeshId, oldPasses);
            StageAdd(entityId, transformIndex, newMaterialIndex, newPasses, newMesh);
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

            m_pendingBatches.clear();


            for (const auto& toRemove : m_pendingRemovals)
            {
                const auto instanceIndex = m_indexLookup.at(toRemove.id);
                m_indexLookup.erase(toRemove.id);
                auto& batch = GetBatch(toRemove.meshId, toRemove.passes);
                NC_ASSERT(batch.count != 0, "p bad");
                const auto batchEndIndex = batch.offset + batch.count - 1;
                NC_ASSERT(batchEndIndex < m_buffer.size(), "p bad again");

                // if equal, is last item, if greater, we removed others from this batch, either way, don't swap
                if (instanceIndex < batchEndIndex)
                {
                    // another option: instead of shifting, do pop and swap with back of range...?
                    m_buffer[instanceIndex] = m_buffer[batchEndIndex];
                    // this is still dumb
                    for (auto& i : m_indexLookup.values())
                    {
                        if (i == batchEndIndex)
                        {
                            i = instanceIndex;
                            break;
                        }
                    }

                    // for (auto i = instanceIndex + 1; i < batch.offset + batch.count; ++i)
                    // {
                    //     // todo: this is wrong, we need to know each entity...
                    //     --m_indexLookup.at(i);
                    //     // todo: shift left
                    //     m_buffer.at(i - 1) = m_buffer.at(i);
                    // }
                }

                --batch.count;
            }

            m_pendingRemovals.clear();

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

                // todo: if else here, able to emplace, can we do this somehow?... maybe:
                //   - reverse iterate
                //   - if fits in capacity
                //     - emplace item
                //     - move item to back
                //     - update some 'endOfRange' value
                //   - remove from 'endOfRange' to 'end()' ?

                // weird
                toAdd.id = instanceIndex;

            }

            std::ranges::sort(m_pendingAdditions, [](const auto& lhs, const auto& rhs){
                return lhs.id > rhs.id;
            });

            struct IndexOffsets
            {
                uint32_t insertIndex;
                uint32_t shiftCount;
            };

            auto indexOffsets = std::vector<IndexOffsets>{};
            indexOffsets.reserve(m_pendingAdditions.size());

            // todo: atm, this doesn't account for capacity, will grow bigger...
            m_buffer.resize(m_buffer.size() + m_pendingAdditions.size());

            auto shiftCount = m_pendingAdditions.size();
            auto rngEnd = m_buffer.end();
            for (auto& toAdd : m_pendingAdditions)
            {
                const auto i = toAdd.id;
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

            m_pendingAdditions.clear();
        }

        // todo: remove this
        void AddInstance(uint32_t entityId,
                         uint32_t transformIndex,
                         MaterialInstanceHandle materialIndex,
                         MaterialPasses passes,
                         const asset::MeshView& mesh)
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
            m_indexLookup.emplace(entityId, instanceIndex);
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
        }

        // remove this
        void RemoveInstance(uint32_t entityId,
                            uint64_t meshId,
                            MaterialPasses passes)
        {
            const auto instanceIndex = m_indexLookup.at(entityId);
            m_indexLookup.erase(entityId);
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
        sparse_map<uint32_t> m_indexLookup; // maps entityId -> instanceIndex
        std::vector<StagedBatch> m_pendingBatches;
        std::vector<StagedInstance> m_pendingAdditions;
        std::vector<StagedInstance> m_pendingRemovals;

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
