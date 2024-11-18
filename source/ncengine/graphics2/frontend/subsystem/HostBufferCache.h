#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <vector>

namespace nc::graphics
{
// todo: handle SOA semantics somehow
// todo: handle deferred writes for parallelism

using HostBufferHandle = uint32_t;

// Buffer type for managing the cpu-side of a structured buffer.
// - returned handles are stable indices
// - buffer may be 'sparse' - indices to erased elements remain valid and are backfilled on the next emplace
// - unless otherwise specified, functions track modified elements, which can be querried for later updates as
//   indices or a BufferUpdateInfo<T>.
template<class T>
class HostBuffer
{
    public:
        explicit HostBuffer(uint32_t maxInstances)
            : m_maxIndex{maxInstances}
        {
        }

        // Add a new element
        template<class... Args>
        auto emplace(Args&&... args) -> HostBufferHandle
        {
            if (m_freeList.empty())
            {
                NC_ASSERT(m_nextIndex < m_maxIndex, "Max instances exceeded");
                m_data.emplace_back(std::forward<Args>(args)...);
                m_dirty.push_back(m_nextIndex);
                return m_nextIndex++;
            }

            const auto index = m_freeList.back();
            m_freeList.pop_back();
            m_data[index] = T(std::forward<Args>(args)...);
            m_dirty.push_back(index);
            return index;
        }

        // Remove an element (does not modify element data or mark as dirty)
        void erase(HostBufferHandle handle)
        {
            NC_ASSERT(handle < m_data.size(), "Instance out of bounds");
            m_freeList.push_back(handle);
        }

        // Remove an element, overwriting the current value (does not mark as dirty)
        void erase(HostBufferHandle handle, const T& tombstone)
        {
            NC_ASSERT(handle < m_data.size(), "Instance out of bounds");
            m_data[handle] = tombstone;
            m_freeList.push_back(handle);
        }

        // Mutable element access
        auto access_for_write(HostBufferHandle handle) -> T&
        {
            mark_dirty(handle);
            return m_data[handle];
        }

        // Immutable element access
        auto access_for_read(HostBufferHandle handle) const -> const T&
        {
            return m_data[handle];
        }

        // Explicitly track a modification to an element
        void mark_dirty(HostBufferHandle handle)
        {
            m_dirty.push_back(handle);
        }

        // Check if any elements have been modified
        auto has_dirty_indices() const -> bool
        {
            return !m_dirty.empty();
        }

        // View indices of modified elements (indices are unsorted and may contain duplicates)
        auto get_dirty_indices() const -> std::span<const uint32_t>
        {
            return m_dirty;
        }

        // Sort and dedupe indices of modified elements
        void sort_dirty_indices()
        {
            const auto maxIndex = m_data.size();
            auto visited = std::vector<uint8_t>(maxIndex, 0);
            for (const auto i : m_dirty)
            {
                visited[i] = true;
            }

            m_dirty.clear();
            for(auto i = 0u; i < maxIndex; ++i)
            {
                if (visited[i])
                    m_dirty.push_back(i);
            }
        }

        // Mark all modified elements as up-to-date
        void reset_dirty_indices()
        {
            m_dirty.clear();
        }

        // Build a BufferUpdateInfo<T> for all modified elements (calls sort_dirty_indices() and reset_dirty_indices())
        auto build_update_info() -> BufferUpdateInfo<T>
        {
            return has_dirty_indices()
                ? BufferUpdateInfo<T>{m_data, collect_dirty_ranges()}
                : BufferUpdateInfo<T>{};
        }

        // STL Functions
        // note: Any elements modified via these functions must be explicitly marked dirty!
        auto size()     const { return m_data.size();                   }
        auto max_size() const { return static_cast<size_t>(m_maxIndex); }
        auto capacity() const { return m_data.capacity();               }
        auto data()           { return m_data.data();                   }
        auto data()     const { return m_data.data();                   }
        auto begin()          { return m_data.begin();                  }
        auto begin()    const { return m_data.begin();                  }
        auto end()            { return m_data.end();                    }
        auto end()      const { return m_data.end();                    }

        void reserve(size_t capacity)
        {
            m_data.reserve(capacity);
        }

        void clear() noexcept
        {
            m_data.clear();
            m_dirty.clear();
            m_freeList.clear();
            m_nextIndex = 0;
        }

        void shrink_to_fit()
        {
            m_data.shrink_to_fit();
            m_dirty.shrink_to_fit();
            m_freeList.shrink_to_fit();
        }

    private:
        std::vector<T> m_data;
        std::vector<uint32_t> m_dirty;
        std::vector<uint32_t> m_freeList;
        uint32_t m_nextIndex = 0;
        uint32_t m_maxIndex;

        auto collect_dirty_ranges() -> std::vector<BufferSlice>
        {
            sort_dirty_indices();
            const auto end = static_cast<uint32_t>(m_dirty.size());
            auto curStart = m_dirty[0];
            auto curEnd = curStart + 1;
            auto out = std::vector<BufferSlice>{};
            for (auto nextEnd = 1u; nextEnd < end; ++nextEnd)
            {
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
            reset_dirty_indices();
            return out;
        }
};
} // namespace nc::graphics
