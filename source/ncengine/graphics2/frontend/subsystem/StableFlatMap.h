// #pragma once

// #include "ncutility/NcError.h"

// #include <vector>

// namespace nc
// {
// template<class Key, class Value>
// class stable_cache
// {
//     public:

//         template<class... Args>
//         auto emplace(Key key, Args&&... args) -> uint32_t
//         {
//             if (m_freeList.empty())
//             {
//                 NC_ASSERT(m_nextIndex < m_maxIndex, "Max instances exceeded");
//                 m_keys.push_back(key);
//                 m_values.emplace_back(std::forward<Args>(args)...);
//                 m_dirty.push_back(m_nextIndex);
//                 return m_nextIndex++;
//             }

//             const auto index = m_freeList.back();
//             m_freeList.pop_back();
//             m_keys[index] = key;
//             m_data[index] = Value{std::forward<Args>(args)...};
//             m_dirty.push_back(index);
//             return index;
//         }

//         void remove(Key key, Key tombstone = Key{})
//         {
//             const auto pos = std::ranges::find(m_keys, key);
//             NC_ASSERT(pos != m_keys.end(), "Instance not found");
//             const auto index = static_cast<uint32_t>(std::distance(m_keys.begin(), pos));
//             m_key[index] = tombstone;
//             m_freeList.push_back(index);
//         }

//         auto keys()                -> std::span<Key>            { return m_keys;   }
//         auto values()              -> std::span<Values>         { return m_values; }
//         auto dirty_indices() const -> std::span<const uint32_t> { return m_dirty;  }

//     private:
//         std::vector<Value> m_values;
//         std::vector<Key> m_keys;
//         std::vector<uint32_t> m_dirty;
//         std::vector<uint32_t> m_freeList;
//         uint32_t m_nextIndex = 0;
//         uint32_t m_maxIndex;
// };
// } // namespace nc
