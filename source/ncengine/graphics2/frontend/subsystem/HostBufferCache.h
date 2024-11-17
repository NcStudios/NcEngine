#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <vector>

namespace nc::graphics
{
// todo: handle SOA semantics somehow
// todo: handle deferred writes for parallelism

template<class T>
class HostBufferCache
{
    public:
        explicit HostBufferCache(uint32_t maxInstances)
            : m_maxIndex{maxInstances}
        {
        }

        template<class... Args>
        // ctorabl w/
        auto emplace(Args&&... args) -> uint32_t
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

        void erase(uint32_t index)
        {
            NC_ASSERT(index < m_data.size(), "Instance out of bounds");
            m_freeList.push_back(index);
        }

        void erase(uint32_t index, const T& tombstone)
        {
            NC_ASSERT(index < m_data.size(), "Instance out of bounds");
            m_data[index] = tombstone;
            m_freeList.push_back(index);
        }

        auto build_update_info() -> BufferUpdateInfo<T>
        {
            return has_dirty_state()
                ? BufferUpdateInfo<T>{m_data, collect_dirty_ranges()}
                : BufferUpdateInfo<T>{};
        }

        auto has_dirty_state()  const -> bool                      { return !m_dirty.empty(); }
        auto dirty_indices()    const -> std::span<const uint32_t> { return m_dirty; }

        void mark_dirty(uint32_t index)
        {
            m_dirty.push_back(index);
        }

        void reset_dirty_state()
        {
            m_dirty.clear();
        }

        void clear() noexcept
        {
            m_data.clear();
            m_data.shrink_to_fit();
            m_dirty.clear();
            m_dirty.shrink_to_fit();
            m_freeList.clear();
            m_freeList.shrink_to_fit();
            m_nextIndex = 0;
        }

        auto operator[](uint32_t index)       -> T&       { return m_data[index];     }
        auto operator[](uint32_t index) const -> const T& { return m_data[index];     }
        auto at(uint32_t index)               -> T&       { return m_data.at(index);  }
        auto at(uint32_t index)         const -> const T& { return m_data.at(index);  }
        auto size()                     const -> size_t   { return m_data.size();     } // correct?
        auto max_size()                 const -> size_t   { return m_maxIndex;        }
        auto num_free_slots()           const -> size_t   { return m_freeList.size(); }
        auto capacity()                 const -> size_t   { return m_data.capacity(); }
        void reserve(uint32_t capacity)                   { m_data.reserve(capacity); }
        auto data()                           -> T*       { return m_data.data();     }
        auto data()                     const -> const T* { return m_data.data();     }
        auto begin()                                      { return m_data.begin();    }
        auto begin()                    const             { return m_data.begin();    }
        auto end()                                        { return m_data.end();      }
        auto end()                      const             { return m_data.end();      }

    private:
        std::vector<T> m_data;
        std::vector<uint32_t> m_dirty;
        std::vector<uint32_t> m_freeList;
        uint32_t m_nextIndex = 0;
        uint32_t m_maxIndex;

        auto collect_dirty_ranges() -> std::vector<BufferSlice>
        {
            auto out = std::vector<BufferSlice>{};
            if (m_dirty.empty())
            {
                return out;
            }

            std::ranges::sort(m_dirty);
            auto removed = std::ranges::unique(m_dirty);
            m_dirty.erase(removed.begin(), removed.end());
            auto start = m_dirty[0];
            auto end = start + 1;
            for (const auto nextEnd : std::views::drop(m_dirty, 1))
            {
                if (nextEnd == end)
                {
                    end += 1;
                }
                else
                {
                    out.emplace_back(start, end - start);
                    start = nextEnd;
                    end = start + 1;
                }
            }

            out.emplace_back(start, end - start);
            return out;
        }
};
} // namespace nc::graphics
