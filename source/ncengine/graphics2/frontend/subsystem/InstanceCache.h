#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncengine/asset/AssetViews.h"
#include "ncengine/utility/SparseMap.h"

#include <ranges>
#include <vector>

namespace nc::graphics
{
struct InstanceData
{
    uint32_t transformIndex = UINT32_MAX;
    uint32_t materialIndex = UINT32_MAX;
};

struct InstanceAddResult
{
    uint32_t instanceIndex = UINT32_MAX;
    bool modifiedIndices = false;
};

// instead of batch, should just make attribs...
struct Batch
{
    uint32_t instanceOffset = UINT32_MAX;
    uint32_t instanceCount = UINT32_MAX;
    uint32_t indexOffset = UINT32_MAX;
    uint32_t indexCount = UINT32_MAX;
    uint32_t vertexOffset = UINT32_MAX;
};

class InstanceCache2
{
    using BatchKey = uint32_t;
    static constexpr auto MakeKey(uint64_t meshId, MaterialPasses passes) -> BatchKey
    {
        // mock, somehow need to hash combine into uint32_t
        return static_cast<uint32_t>(meshId + passes); // mock
    }

    // not really sure best way to manage
    struct BatchRegion
    {
        uint32_t offset;
        uint32_t count;
        uint32_t indexOffset;
        uint32_t indexCount;
        uint32_t vertexOffset;

        uint32_t capacity;
        MaterialPasses passes;
    };

    public:
        explicit InstanceCache2()
            : m_batches{100}, // todo: don't know
              m_indexLookup{100}
        {
        }

        auto AddInstance(uint32_t transformIndex,
                         MaterialInstanceHandle materialIndex,
                         MaterialPasses passes,
                         const asset::MeshView& mesh) -> InstanceAddResult
        {
            const auto id = m_nextId = 0;
            const auto key = MakeKey(mesh.id, passes);
            // ...assuming always exists
            if (!m_batches.contains(key))
            {
                // prob need to add an item to buffer as well
                //   -- maybe not.. insert() below might handle it well
                m_batches.emplace(key, BatchRegion{
                    static_cast<uint32_t>(m_buffer.size()),
                    0,
                    mesh.firstIndex,
                    mesh.indexCount,
                    mesh.firstVertex,
                    0,
                    passes
                });
            }

            auto& batch = m_batches.at(key);
            const auto instanceIndex = batch.offset + batch.count;
            ++batch.count;
            m_indexLookup.at(id) = instanceIndex;
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

                return {id, true};
            }
            else
            {
                m_buffer.at(instanceIndex) = InstanceData{transformIndex, materialIndex};
                return {id, false};
            }
        }

        void RemoveInstance(uint32_t id,
                            uint64_t meshId,
                            MaterialPasses passes)
        {
            const auto instanceIndex = m_indexLookup.at(id);
            m_indexLookup.erase(id);
            // todo: ...reclaim id
            const auto key = MakeKey(meshId, passes);
            auto& batch = m_batches.at(key);
            ++batch.capacity;
            if (instanceIndex == batch.offset + batch.count)
            {
                return;
            }

            for (auto i = instanceIndex + 1; i < batch.offset + batch.count; ++i)
            {
                --m_indexLookup.at(i);
                m_buffer.at(i - 1) = m_buffer.at(i);
            }
        }

        // todo: we prob want to just make Diligent::XXXAttribs here... but can't be exactly here b/c its in frontend
        auto BuildBatches(std::span<const MaterialPass::type> passes) -> std::vector<std::vector<Batch>>
        {
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
        sparse_map<BatchRegion> m_batches; // maps BatchKey -> BatchRegion
        sparse_map<uint32_t> m_indexLookup; // maps instanceId -> instanceIndex
        uint32_t m_nextId = 0;
};
} // namespace nc::graphics
