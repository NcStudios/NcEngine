#pragma once

#include "ecs/Entity.h"

#include <algorithm>
#include <limits>
#include <span>
#include <vector>

namespace nc
{
    /** Container for data associated with a pair of objects, typically Entities. This is a
     *  CRTP-based container and requires some member functions to be present in derived types.
     *  The behavior is similar to a flat map with the key (pair) embedded in the value.
     *  Based on Pierre Terdiman's pair manager: http://www.codercorner.com. */
    template<class Derived, class DataType, class IdType = Entity, uint32_t InitialSize = 1024u>
    class PairCache
    {
        /** Size must be a power of 2. */
        static_assert(InitialSize && (InitialSize & (InitialSize - 1)) == 0u);

        static constexpr uint32_t NullIndex = std::numeric_limits<uint32_t>::max();

        public:
            PairCache();

            /** Add an entry associated with a and b. Calls one of the following Derived methods
             *  depending on whether ab is a new pair or not:
             *    void AddToExisting(DataType* existing, Args&& ... args);
             *    void ConstructNew(IdType a, IdType b, Args&& ... args); */
            template<class ... Args>
            void Add(IdType a, IdType b, Args&& ... args);

            /** Remove the entry associated with a and b, if it exists. A hash must be computed from 
             *  an entry by calling the following from Derived:
             *    uint32_t Hash(const DataType& data); */
            void Remove(IdType a, IdType b);

            /** Returns pointer to the entry associated with a and b or nullptr on failure. Calls the
             *  following method from Derived:
             *    bool PairEqual(IdType a, IdType b, const DataType& data); */
            auto Find(IdType a, IdType b, uint32_t hash);

            /** Removes all entries and resizes table to the default size. */
            void Clear();

            /** Computes a 32 bit hash for a pair. If IdType != Entity, this calls the following
             *  method from Derived:
             *    uint32_t Hash(IdType a, IdType b); */
            auto Hash(IdType a, IdType b) -> uint32_t;

            /** Contiguous access to entries. */
            auto GetData() -> std::span<DataType> { return m_data; }
            auto GetData() const -> std::span<const DataType> { return m_data; }

        private:
            std::vector<DataType> m_data;
            std::vector<uint32_t> m_hashTable;
            std::vector<uint32_t> m_next;
            uint32_t m_hashMask;

            auto ToDerived() -> Derived* { return static_cast<Derived*>(this); }
            void RemoveDataFromTable(uint32_t hash, uint32_t index);
            void GrowTable();
    };

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    PairCache<Derived, DataType, IdType, InitialSize>::PairCache()
        : m_data{},
          m_hashTable(InitialSize, NullIndex),
          m_next(InitialSize, NullIndex),
          m_hashMask{InitialSize - 1}
    {
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    auto PairCache<Derived, DataType, IdType, InitialSize>::Find(IdType a, IdType b, uint32_t hash)
    {
        /** Follow table indices until we find the pair or hit a null value. */
        auto* derived = ToDerived();
        auto index = m_hashTable[hash];
        while(index != NullIndex && !derived->PairEqual(a, b, m_data[index]))
        {
            index = m_next[index];
        }

        return (index == NullIndex) ? nullptr : &m_data[index];
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    template<class ... Args>
    void PairCache<Derived, DataType, IdType, InitialSize>::Add(IdType a, IdType b, Args&& ... args)
    {
        /** Check if there is already an entry for this pair. */
        auto hash = Hash(a, b);
        auto* existing = Find(a, b, hash);
        if(existing)
        {
            ToDerived()->AddToExisting(existing, std::forward<Args>(args)...);
            return;
        }

        auto nextIndex = m_data.size();

        /** Make sure the table is large enough to support an addition. */
        if(nextIndex >= m_hashTable.size())
        {
            GrowTable();
            hash = Hash(a, b);
        }

        /** Add a new entry. */
        m_data.push_back(ToDerived()->ConstructNew(a, b, std::forward<Args>(args)...));
        m_next[nextIndex] = m_hashTable[hash];
        m_hashTable[hash] = nextIndex;
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    void PairCache<Derived, DataType, IdType, InitialSize>::Remove(IdType a, IdType b)
    {
        /** Find the entry associated with the pair. */
        auto hash = Hash(a, b);
        auto* existing = Find(a, b, hash);

        if(!existing)
        {
            return;
        }

        /** Remove index from table */
        auto toRemoveIndex = static_cast<uint32_t>(std::distance(&m_data[0], existing));
        RemoveDataFromTable(hash, toRemoveIndex);

        /** If the entry is in the last position, just remove it. */
        auto toSwapIndex = m_data.size() - 1;
        if(toSwapIndex == toRemoveIndex)
        {
            m_data.pop_back();
            return;
        }

        /** The last entry will be swapped to the removed position. Remove its index from the table. */
        auto& toSwap = m_data.back();
        auto toSwapHash = ToDerived()->Hash(toSwap);
        RemoveDataFromTable(toSwapHash, toSwapIndex);

        /** Swap and add back the last entry. */
        m_data[toRemoveIndex] = std::move(toSwap);
        m_data.pop_back();
        m_next[toRemoveIndex] = m_hashTable[toSwapHash];
        m_hashTable.at(toSwapHash) = toRemoveIndex;
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    void PairCache<Derived, DataType, IdType, InitialSize>::Clear()
    {
        m_data.clear();
        m_data.shrink_to_fit();
        m_hashTable.resize(InitialSize, NullIndex);
        std::ranges::fill(m_hashTable, NullIndex);
        m_next.resize(InitialSize, NullIndex);
        std::ranges::fill(m_next, NullIndex);
        m_hashMask = InitialSize - 1;
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    void PairCache<Derived, DataType, IdType, InitialSize>::RemoveDataFromTable(uint32_t hash, uint32_t index)
    {
        /** Find where in the table the index exists. */
        auto current = m_hashTable[hash];
        auto previous = NullIndex;
        while(current != index)
        {
            previous = current;
            current = m_next[current];
        }

        /** Shift the table indices back to remove the target index. */
        if(previous == NullIndex)
        {
            m_hashTable[hash] = m_next[index];
        }
        else
        {
            m_next[previous] = m_next[index];
        }
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    void PairCache<Derived, DataType, IdType, InitialSize>::GrowTable()
    {
        /** Double table size and null all entries. The table size must remain a power of two. */
        std::ranges::fill(m_hashTable, NullIndex);
        std::ranges::fill(m_next, NullIndex);
        const auto oldSize = m_hashTable.size();
        const auto newSize = oldSize * 2u;
        m_hashTable.resize(newSize, NullIndex);
        m_next.resize(newSize, NullIndex);
        m_hashMask = newSize - 1;
        auto* derived = ToDerived();

        /** Compute new hashes for entries and add them back. */
        const auto dataCount = m_data.size();
        for(auto i = 0u; i < dataCount; ++i)
        {
            const auto& data = m_data[i];
            auto hash = derived->Hash(data);
            m_next[i] = m_hashTable[hash];
            m_hashTable[hash] = i;
        }
    }

    template<class Derived, class DataType, class IdType, uint32_t InitialSize>
    uint32_t PairCache<Derived, DataType, IdType, InitialSize>::Hash(IdType first, IdType second)
    {
        if constexpr(std::same_as<IdType, Entity>)
        {
            /** Order the pair so that Hash(a, b) == Hash(b, a) */
            auto a = first.Index();
            auto b = second.Index();
            if(a > b)
                std::swap(a, b);

            /** Thomas Wang 32bit hash */
            uint32_t k = a | (b << 16);
            k += ~(k << 15);
            k ^=  (k >> 10);
            k +=  (k << 3);
            k ^=  (k >> 6);
            k += ~(k << 11);
            k ^=  (k >> 16);

            /** Wrap to table size */
            return k & m_hashMask;
        }
        else
        {
            return ToDerived->Hash(first, second) & m_hashMask;
        }
    }
}