#include "InstanceCache.h"

#include "ncengine/debug/Profile.h"

namespace
{
void IncrementBatchCount(nc::graphics::BatchRegion& region, uint32_t count)
{
    region.batch.instanceCount += count;
    if (region.batch.instanceCount > region.capacity)
    {
        region.capacity = region.batch.instanceCount;
    }
}

auto WalkBatch(const nc::graphics::Batch& batch)
{
    return std::views::iota(
        batch.instanceOffset,
        batch.instanceOffset + batch.instanceCount
    );
}

auto BuildBatch(uint32_t batchOffset, const nc::asset::MeshView& mesh)
{
    return nc::graphics::Batch{
        .instanceOffset = batchOffset,
        .instanceCount = 0,
        .indexOffset = mesh.firstIndex,
        .indexCount = mesh.indexCount,
        .vertexOffset = mesh.firstVertex
    };
}
} // anonymous namespace

namespace nc::graphics
{
void InstanceCacheStaging::AddInstance(uint32_t entityId,
                                       MaterialPasses passes,
                                       const asset::MeshView& mesh,
                                       const InstanceData& instanceData)
{
    m_hasStagedState = true;
    const auto key = BatchKey{passes, mesh.id};
    const auto staging = std::ranges::find(m_stagedInstances, key, &StagedBatchInstances::key);
    if (staging != m_stagedInstances.end())
    {
        staging->instances.emplace_back(entityId, instanceData);
        return;
    }

    m_stagedInstances.emplace_back(
        key,
        std::vector<StagedInstance>{
            StagedInstance{
                entityId,
                instanceData
            }
        }
    );

    m_stagedRegions.emplace_back(passes, mesh);
}

void InstanceCacheStaging::RemoveInstance(uint32_t entityId,
                                          MaterialPasses passes,
                                          uint64_t meshId)
{
    m_hasStagedState = true;
    m_stagedRemovals.emplace_back(BatchKey{passes, meshId}, entityId);
}

void InstanceCacheStaging::UpdateInstance(uint32_t entityId,
                                          MaterialPasses oldPasses,
                                          MaterialPasses newPasses,
                                          uint64_t oldMeshId,
                                          const asset::MeshView& newMesh,
                                          const InstanceData& instanceData)
{
    m_hasStagedState = true;
    RemoveInstance(entityId, oldPasses, oldMeshId);
    AddInstance(entityId, newPasses, newMesh, instanceData);
}

void InstanceCacheStaging::ClearStaged()
{
    m_hasStagedState = false;
    m_stagedRegions.clear();
    m_stagedRemovals.clear();
    for (auto& pending : m_stagedInstances)
    {
        pending.instances.clear();
    }
}

void InstanceCacheStaging::Purge(std::span<const BatchRegion> toKeep) noexcept
{
    m_stagedRegions.clear();
    m_stagedRegions.shrink_to_fit();
    m_stagedRemovals.clear();
    m_stagedRemovals.shrink_to_fit();
    std::erase_if(m_stagedInstances, [&toKeep](auto& staging)
    {
        if (!std::ranges::contains(toKeep, staging.key, &BatchRegion::key))
        {
            return true;
        }

        staging.instances.clear();
        staging.instances.shrink_to_fit();
        return false;
    });

    m_stagedInstances.shrink_to_fit();
}

InstanceCache::InstanceCache(uint32_t maxMeshRenderers,
                             uint32_t initialBatchSize)
    : m_batches{},
      m_entityToIndex{100, maxMeshRenderers},
      m_initialBatchSize{initialBatchSize}
{
    NC_ASSERT(initialBatchSize > 0, "Invalid initial batch size");
}

void InstanceCache::CommitPendingChanges()
{
    NC_PROFILE_SCOPE("InstanceCache::CommitPendingChanges()", ProfileCategory::Rendering);
    if (!m_stagingArea.HasStagedState())
    {
        return;
    }

    const auto& stagedRegions = m_stagingArea.GetStagedBatchRegions();
    const auto newBatchCount = stagedRegions.size();
    CommitBatchRegions(stagedRegions);
    CommitRemovals(m_stagingArea.GetStagedRemovals());
    CommitAdditions(m_stagingArea.GetStagedInstances(), newBatchCount);
    m_stagingArea.ClearStaged();
}

auto InstanceCache::BuildState() -> BufferUpdateInfo<InstanceData>
{
    // For simplicity, we're only tracking one dirty range from the first modified index to the buffer
    // end. This can result in more copies than necessary for non-shifting insertions and removals, but the
    // data is pretty cheap to copy.
    const auto begin = std::exchange(m_dirtyBegin, NullBatchIndex);
    if (begin == NullBatchIndex)
    {
        return BufferUpdateInfo<InstanceData>{};
    }

    const auto count = static_cast<uint32_t>(m_buffer.size()) - begin;
    if (count == 0)
    {
        return BufferUpdateInfo<InstanceData>{};
    }

    return BufferUpdateInfo<InstanceData>{
        .instances = m_buffer,
        .dirtyRanges = {{begin, count}}
    };
}

auto InstanceCache::BuildBatches(std::span<const MaterialPass::type> passes) -> std::vector<std::vector<Batch>>
{
    NC_PROFILE_SCOPE("InstanceCache::BuildBatches()", ProfileCategory::Rendering);
    auto out = std::vector<std::vector<Batch>>(passes.size(), {});
    for (const auto& region : m_batches)
    {
        if (region.batch.instanceCount == 0)
        {
            continue;
        }

        for (auto [passId, passOut] : std::views::zip(passes, out))
        {
            if (region.key.passes & passId)
            {
                passOut.push_back(region.batch);
            }
        }
    }

    return out;
}

void InstanceCache::Purge() noexcept
{
    CommitPendingChanges();
    m_dirtyBegin = 0u;
    auto bufferEnd = 0u;
    for (auto& region : m_batches)
    {
        if (region.batch.instanceCount > 0)
        {
            auto insertIndex = bufferEnd;
            for (const auto currentIndex : WalkBatch(region.batch))
            {
                MoveInstance(currentIndex, insertIndex++);
            }

            region.batch.instanceOffset = bufferEnd;
            region.capacity = std::max(region.batch.instanceCount, m_initialBatchSize);
            bufferEnd += region.capacity;
        }
    }

    std::erase_if(m_batches, [](const auto& region)
    {
        return region.batch.instanceCount == 0;
    });

    m_batches.shrink_to_fit();
    m_buffer.resize(bufferEnd);
    m_buffer.shrink_to_fit();
    m_indexToEntity.resize(bufferEnd);
    m_indexToEntity.shrink_to_fit();
    m_stagingArea.Purge(m_batches);
}

void InstanceCache::MarkDirtyStartingFrom(uint32_t instanceIndex)
{
    m_dirtyBegin = std::min(m_dirtyBegin, instanceIndex);
}

void InstanceCache::ResetDirtyRange() noexcept
{
    m_dirtyBegin = NullBatchIndex;
}

void InstanceCache::InsertInstance(uint32_t index, const StagedInstance& instance)
{
    m_buffer[index] = instance.instanceData;
    m_indexToEntity[index] = instance.entityId;
    m_entityToIndex.emplace(instance.entityId, index);
}

void InstanceCache::MoveInstance(uint32_t from, uint32_t to)
{
    m_buffer[to] = m_buffer[from];
    const auto entity = std::exchange(m_indexToEntity[from], NullBatchIndex);
    m_indexToEntity[to] = entity;
    m_entityToIndex.at(entity) = to;
}

auto InstanceCache::EraseInstance(uint32_t entityId) -> uint32_t
{
    const auto instanceIndex = m_entityToIndex.at(entityId);
    m_entityToIndex.erase(entityId);
    m_indexToEntity.at(instanceIndex) = NullBatchIndex;
    return instanceIndex;
}

void InstanceCache::CommitBatchRegions(const std::vector<StagedBatchRegion>& stagedRegions)
{
    auto batchIndex = static_cast<uint32_t>(m_buffer.size());
    for (const auto& [passes, mesh] : stagedRegions)
    {
        m_batches.emplace_back(
            BatchKey{passes, mesh.id},
            BuildBatch(batchIndex, mesh),
            m_initialBatchSize
        );

        batchIndex += m_initialBatchSize;
    }
}

void InstanceCache::CommitRemovals(const std::vector<StagedRemoval>& stagedRemovals)
{
    for (const auto& toRemove : stagedRemovals)
    {
        const auto instanceIndex = EraseInstance(toRemove.entityId);
        auto& batch = GetRegion(toRemove.key).batch;
        const auto batchEndIndex = batch.instanceOffset + batch.instanceCount - 1;
        --batch.instanceCount;

        // If this is the last batch instance, decrementing the count is sufficient, else swap with last.
        if (instanceIndex < batchEndIndex)
        {
            MoveInstance(batchEndIndex, instanceIndex);
            MarkDirtyStartingFrom(instanceIndex);
        }
    }
}

void InstanceCache::CommitAdditions(const std::vector<StagedBatchInstances>& stagedInstances, size_t newBatchCount)
{
    NC_ASSERT(m_batches.size() == stagedInstances.size(), "Batch size mismatch");
    const auto perBatchShift = EvaluateAdditions(stagedInstances, newBatchCount);

    // The range from the end of the last batch to buffer end is newly allocated space. Visiting in reverse lets
    // us freely shift each batch to the right. We can then write new instances into the free spaces between batches.
    auto reverseView = std::views::reverse(std::views::zip(m_batches, stagedInstances, perBatchShift));
    for (auto [current, pending, shiftBy] : reverseView)
    {
        if (shiftBy > 0)
        {
            for (const auto currentIndex : std::views::reverse(WalkBatch(current.batch)))
            {
                MoveInstance(currentIndex, currentIndex + shiftBy);
            }

            current.batch.instanceOffset += shiftBy;
        }

        const auto insertCount = static_cast<uint32_t>(pending.instances.size());
        if (insertCount == 0)
        {
            continue;
        }

        auto instanceIndex = current.batch.instanceOffset + current.batch.instanceCount;
        IncrementBatchCount(current, insertCount);
        for (auto& toAdd : pending.instances)
        {
            InsertInstance(instanceIndex, toAdd);
            ++instanceIndex;
        }
    }
}

auto InstanceCache::EvaluateAdditions(const std::vector<StagedBatchInstances>& stagedInstances, size_t newBatchCount) -> std::vector<uint32_t>
{
    // Want to minimize number of shifts required when inserting new items. Traverse new instances
    // once to calculate the required shift amount for each batch.
    auto perBatchShift = std::vector<uint32_t>{};
    perBatchShift.reserve(m_batches.size());
    auto accumulatedShifts = 0u;
    for (auto [current, pending] : std::views::zip(m_batches, stagedInstances))
    {
        if (pending.instances.empty())
        {
            perBatchShift.push_back(accumulatedShifts);
            continue;
        }

        const auto freeSlots = current.capacity - current.batch.instanceCount;
        const auto newInstances = static_cast<uint32_t>(pending.instances.size());
        const auto slotsNeeded = newInstances >= freeSlots ? newInstances - freeSlots : 0u;
        perBatchShift.push_back(accumulatedShifts);
        accumulatedShifts += slotsNeeded;
        MarkDirtyStartingFrom(current.batch.instanceOffset);
    }

    // Each batch starts with some capacity. This needs to be allocated in addition to the new instance/shift count.
    const auto additionalSpaceRequired = accumulatedShifts + newBatchCount * m_initialBatchSize;
    if (additionalSpaceRequired > 0u)
    {
        const auto newSize = m_buffer.size() + additionalSpaceRequired;
        m_buffer.resize(newSize);
        m_indexToEntity.resize(newSize, NullBatchIndex);
    }

    return perBatchShift;
}
} // namespace nc::graphics
