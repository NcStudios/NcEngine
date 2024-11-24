#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <vector>

namespace nc::graphics
{
// Identifier for an element in a HostBuffer.
using HostBufferHandle = uint32_t;

// A HostBuffer element waiting to be merged into the buffer.
template<class T>
struct StagedBufferItem
{
    T item;
    HostBufferHandle index;
};

// HostBuffer interface for adding, removing and updating elements. Events originating
// from game logic/API should only be routed here to avoid race conditions with the
// actual buffer.
template<class T>
class HostBufferStaging
{
    public:
        explicit HostBufferStaging(uint32_t maxInstances)
            : m_maxIndex{maxInstances}
        {
        }

        // API-Facing Functions
        template<class... Args>
        auto Emplace(Args&&... args) -> HostBufferHandle
        {
            return m_staged.emplace_back(T(std::forward<Args>(args)...), AssignHandle()).index;
        }

        void Update(HostBufferHandle handle, const T& value)
        {
            m_staged.emplace_back(value, TrackWrite(handle));
        }

        void Erase(HostBufferHandle handle)
        {
            m_freeList.push_back(handle);
        }

        // Functions For Use By HostBuffer
        auto MaxSize()         const -> HostBufferHandle                     { return m_maxIndex; }
        auto HasStagedState()  const -> bool                                 { return !m_staged.empty(); }
        auto GetStaged()       const -> std::span<const StagedBufferItem<T>> { return m_staged; }
        auto GetStagedBounds() const -> HostBufferHandle                     { return m_largestStagedIndex + 1; }

        void ClearStaged() noexcept
        {
            m_staged.clear();
            m_largestStagedIndex = 0;
        }

        void Clear() noexcept
        {
            m_staged.clear();
            m_staged.shrink_to_fit();
            m_freeList.clear();
            m_freeList.shrink_to_fit();
            m_nextIndex = 0;
            m_largestStagedIndex = 0;
        }

    private:
        std::vector<StagedBufferItem<T>> m_staged;
        std::vector<HostBufferHandle> m_freeList;
        HostBufferHandle m_nextIndex = 0;
        HostBufferHandle m_largestStagedIndex = 0;
        HostBufferHandle m_maxIndex;

        auto TrackWrite(HostBufferHandle handle) -> HostBufferHandle
        {
            m_largestStagedIndex = std::max(m_largestStagedIndex, handle);
            return handle;
        }

        auto AssignHandle() -> HostBufferHandle
        {
            return TrackWrite([this]()
            {
                if (m_freeList.empty())
                {
                    NC_ASSERT(m_nextIndex < m_maxIndex, "Max instances exceeded");
                    return m_nextIndex++;
                }

                const auto index = m_freeList.back();
                m_freeList.pop_back();
                return index;
            }());
        }
};

// Buffer type for managing the cpu-side of a structured buffer.
// - Returned handles are stable indices.
// - Buffer may be 'sparse' - indices to erased elements are reused on subsequent insertions.
// - Unless otherwise specified, functions track modified elements, which can be querried for later updates as indices
//   or a BufferUpdateInfo<T>.
//
// The simplest usage looks like: Events from update tasks are routed by a subsystem to the staging area. When the
// frontend render state is built (no update tasks are running), the subsystem calls CommitPendingChanges() to merge
// the staging area. Once this returns, update tasks are free to run again. BuildUpdateInfo() is called at any point
// to produce the state to be copied to associated StructuredBuffer<T>.
template<class T>
class HostBuffer
{
    public:
        explicit HostBuffer(uint32_t maxInstances)
            : m_stagingArea{maxInstances}
        {
        }

        // Get an interface for adding/removing elements.
        // IMPORTANT: All API-level events are to go through the staging area. The graphics backend shall decide
        // when it is safe to commit writes to the buffer.
        auto GetStagingArea() -> HostBufferStaging<T>&
        {
            return m_stagingArea;
        }

        // Merge staging area into the buffer.
        // IMPORTANT: The staging area must not be accessed while this is in progress.
        void CommitPendingChanges()
        {
            if (!m_stagingArea.HasStagedState())
            {
                return;
            }

            if (const auto stagingBounds = m_stagingArea.GetStagedBounds(); stagingBounds > m_data.size())
            {
                m_data.resize(stagingBounds);
            }

            for (const auto& [item, index] : m_stagingArea.GetStaged())
            {
                AccessForWrite(index) = item;
            }

            m_stagingArea.ClearStaged();
        }

        // Build info specifying modified buffer ranges.
        // NOTE: Dirty indices are reset by this call.
        auto BuildUpdateInfo() -> BufferUpdateInfo<T>
        {
            return HasDirtyIndices()
                ? BufferUpdateInfo<T>{m_data, CollectDirtyRanges()}
                : BufferUpdateInfo<T>{};
        }

        // Mutable element access
        auto AccessForWrite(HostBufferHandle handle) -> T&
        {
            MarkDirty(handle);
            return m_data[handle];
        }

        // Immutable element access
        auto AccessForRead(HostBufferHandle handle) const -> const T&
        {
            return m_data[handle];
        }

        // Explicitly track an element modification (if modified through data(), for example).
        void MarkDirty(HostBufferHandle handle)
        {
            m_dirty.push_back(handle);
        }

        // Check if any elements have been modified
        auto HasDirtyIndices() const -> bool
        {
            return !m_dirty.empty();
        }

        // View indices of modified elements (indices are unsorted and may contain duplicates)
        auto GetDirtyIndices() const -> std::span<const uint32_t>
        {
            return m_dirty;
        }

        // Sort and dedupe indices of modified elements
        void SortDirtyIndices()
        {
            const auto maxIndex = m_data.size();
            auto visited = std::vector<uint8_t>(maxIndex, 0);
            for (const auto i : m_dirty)
            {
                visited[i] = 1;
            }

            m_dirty.clear();
            for(auto i = 0u; i < maxIndex; ++i)
            {
                if (visited[i] == 1)
                    m_dirty.push_back(i);
            }
        }

        // Mark all modified elements as up-to-date
        void ClearDirtyIndices() noexcept
        {
            m_dirty.clear();
        }

        void Clear() noexcept
        {
            m_data.clear();
            m_data.shrink_to_fit();
            m_dirty.clear();
            m_dirty.shrink_to_fit();
            m_stagingArea.Clear();
        }

        // STL Functions
        // IMPORTANT: Any elements modified via these functions must be explicitly marked dirty!
        auto size()     const { return m_data.size(); }
        auto max_size() const { return m_stagingArea.MaxSize(); }
        auto capacity() const { return m_data.capacity(); }
        auto data()           { return m_data.data(); }
        auto data()     const { return m_data.data(); }
        auto begin()          { return m_data.begin(); }
        auto begin()    const { return m_data.begin(); }
        auto end()            { return m_data.end(); }
        auto end()      const { return m_data.end(); }

        void reserve(size_t capacity)
        {
            m_data.reserve(capacity);
        }

    private:
        std::vector<T> m_data;
        std::vector<uint32_t> m_dirty;
        HostBufferStaging<T> m_stagingArea;

        auto CollectDirtyRanges() -> std::vector<BufferSlice>
        {
            NC_ASSERT(!m_dirty.empty(), "Unexpected call");
            SortDirtyIndices();
            const auto end = static_cast<uint32_t>(m_dirty.size());
            auto curStart = m_dirty[0];
            auto curEnd = curStart + 1;
            auto out = std::vector<BufferSlice>{};
            // coalesce adjacent dirty indices into ranges
            for (auto i = 1u; i < end; ++i)
            {
                const auto nextEnd = m_dirty[i];
                if (nextEnd == curEnd)
                {
                    ++curEnd;
                }
                else
                {
                    out.emplace_back(curStart, curEnd - curStart);
                    curStart = nextEnd;
                    curEnd = nextEnd + 1;
                }
            }

            out.emplace_back(curStart, curEnd - curStart);
            ClearDirtyIndices();
            return out;
        }
};
} // namespace nc::graphics
