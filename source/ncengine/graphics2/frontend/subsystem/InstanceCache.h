#pragma once

#include "graphics2/ShaderTypes.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include "ncengine/asset/AssetViews.h"
#include "ncengine/utility/SparseMap.h"

#include <limits>
#include <ranges>
#include <vector>

namespace nc::graphics
{
constexpr auto NullBatchIndex = std::numeric_limits<uint32_t>::max();

// A batch is uniquely identified by a mesh and set of material passes.
struct BatchKey
{
    MaterialPasses passes = NullBatchIndex;
    uint64_t meshId = std::numeric_limits<uint64_t>::max();

    friend auto operator==(const BatchKey& lhs, const BatchKey& rhs) -> bool
    {
        return lhs.passes == rhs.passes && lhs.meshId == rhs.meshId;
    }
};

// Batch state describing a contiguous region in the instance buffer as well as draw attributes.
struct BatchRegion
{
    BatchKey key = BatchKey{};
    Batch batch;
    uint32_t capacity = 0;
};

// A new batch waiting to be added to the instance buffer.
struct StagedBatchRegion
{
    MaterialPasses passes = MaterialPasses{};
    asset::MeshView mesh = asset::MeshView{};
};

// Instance data for a renderer component waiting to added to a batch.
template<class T>
struct StagedInstance
{
    uint32_t entityId = NullBatchIndex;
    T instanceData = T{};
};

// Per-batch staging area.
template<class T>
struct StagedBatchInstances
{
    BatchKey key = BatchKey{};
    std::vector<StagedInstance<T>> instances = {};
};

// Renderer component instance waiting to be removed from a batch.
struct StagedRemoval
{
    BatchKey key = BatchKey{};
    uint32_t entityId = NullBatchIndex;
};

// InstanceCache interface for adding, removing, and updating renderer component instances. Events
// originating from game logic/API should only be routed to here to avoid race conditions with the
// actual InstanceCache buffer.
template<class T>
class InstanceCacheStaging
{
    public:
        // API-Facing Functions
        void AddInstance(uint32_t entityId,
                         MaterialPasses passes,
                         const asset::MeshView& mesh,
                         const T& instanceData);

        void RemoveInstance(uint32_t entityId,
                            MaterialPasses passes,
                            uint64_t meshId);

        void UpdateInstance(uint32_t entityId,
                            MaterialPasses oldPasses,
                            MaterialPasses newPasses,
                            uint64_t oldMeshId,
                            const asset::MeshView& newMesh,
                            const T& instanceData);

        // For Use By InstanceCache
        auto HasStagedState()        const -> bool                                        { return m_hasStagedState; }
        auto GetStagedBatchRegions() const -> const std::vector<StagedBatchRegion>&       { return m_stagedRegions; }
        auto GetStagedInstances()    const -> const std::vector<StagedBatchInstances<T>>& { return m_stagedInstances; }
        auto GetStagedRemovals()     const -> const std::vector<StagedRemoval>&           { return m_stagedRemovals; }
        void ClearStaged();
        void Purge(std::span<const BatchRegion> toKeep) noexcept;

    private:
        std::vector<StagedBatchRegion> m_stagedRegions;
        std::vector<StagedBatchInstances<T>> m_stagedInstances;
        std::vector<StagedRemoval> m_stagedRemovals;
        bool m_hasStagedState = false;
};

// CPU-side buffer for instance data types. Elements are sorted by Batch, where a Batch corresponds to an instanced
// draw call for all objects that share a mesh and material pass combination. Buffer modifications are held
// in a staging area to improve performance when adding many elements and to allow concurrent reads and writes
// from the graphics backend and game logic, respectively.
template<class T>
class InstanceCache
{
    public:
        explicit InstanceCache(uint32_t maxEntities,
                               uint32_t initialBatchSize = 1u);

        // Get the staging area for enqueueing buffer writes.
        auto GetStagingArea() -> InstanceCacheStaging<T>&
        {
            return m_stagingArea;
        }

        // Build info specifying modified buffer ranges.
        auto BuildState() -> BufferUpdateInfo<T>;

        // Generate draw call information for batches. The return value contains a collection of batches
        // to be drawn per material pass provided in the input 'passes'.
        auto BuildBatches(std::span<const MaterialPass::type> passes) -> std::vector<std::vector<Batch>>;

        // Merge staging area into the buffer.
        // IMPORTANT: Staging area cannot be modified while this is running (game logic cannot be running).
        void CommitPendingChanges();

        // Clean and compress unused state. Intended for scene changes but could also be called periodically, if needed.
        // - Removes all empty batches
        // - Shrinks excess batch capacity
        // - Compacts buffer by shifting instances/batches towards the front
        // IMPORTANT: Staging area cannot be modified while this is running (game logic cannot be running).
        void Purge() noexcept;

        auto IsValidInstance(uint32_t entityId) const -> bool
        {
            return m_entityToIndex.contains(entityId);
        }

        auto GetInstanceIndex(uint32_t entityId) const -> uint32_t
        {
            return m_entityToIndex.at(entityId);
        }

        auto GetEntityId(uint32_t instanceIndex) const -> uint32_t
        {
            return m_indexToEntity.at(instanceIndex);
        }

        auto GetInstanceData(uint32_t instanceIndex) const -> const T&
        {
            return m_buffer.at(instanceIndex);
        }

        auto HasBatchFor(const BatchKey& key) -> bool
        {
            return std::ranges::contains(m_batches, key, &BatchRegion::key);
        }

        auto GetRegion(const BatchKey& key) -> BatchRegion&
        {
            auto pos = std::ranges::find(m_batches, key, &BatchRegion::key);
            NC_ASSERT(pos != m_batches.end(), "Batch not found");
            return *pos;
        }

        auto GetInstanceCountUpperBound() const -> size_t
        {
            return m_buffer.size();
        }

        auto GetBatchCount() const -> size_t
        {
            return m_batches.size();
        }

    private:
        std::vector<T> m_buffer;
        std::vector<BatchRegion> m_batches;
        sparse_map<uint32_t> m_entityToIndex;
        std::vector<uint32_t> m_indexToEntity;
        InstanceCacheStaging<T> m_stagingArea;
        uint32_t m_dirtyBegin = NullBatchIndex;
        uint32_t m_initialBatchSize;

        void InsertInstance(uint32_t index, const StagedInstance<T>& instance);
        void MoveInstance(uint32_t from, uint32_t to);
        auto EraseInstance(uint32_t entityId) -> uint32_t;
        void MarkDirtyStartingFrom(uint32_t instanceIndex);
        void ResetDirtyRange() noexcept;
        void CommitBatchRegions(const std::vector<StagedBatchRegion>& stagedRegions);
        void CommitRemovals(const std::vector<StagedRemoval>& stagedRemovals);
        void CommitAdditions(const std::vector<StagedBatchInstances<T>>& stagedInstances, size_t newBatchCount);
        auto EvaluateAdditions(const std::vector<StagedBatchInstances<T>>& stagedInstances, size_t newBatchCount) -> std::vector<uint32_t>;
};
} // namespace nc::graphics

#include "InstanceCache.inl"
