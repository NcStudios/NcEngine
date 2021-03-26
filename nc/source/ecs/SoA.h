#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <set>
#include <span>
#include <type_traits>

namespace nc::ecs
{
    /** An incrementable index for sequentially accessing each valid position in an SoA. */
    class SoAIndex
    {
        using GapIterator = std::set<uint32_t>::const_iterator;
        
        public:
            SoAIndex(uint32_t begin, uint32_t end, GapIterator gapBegin, GapIterator gapEnd);

            SoAIndex& operator++();
            operator unsigned() const;
            bool Valid() const;

        private:
            uint32_t m_current;
            const uint32_t m_end;
            GapIterator m_gapCurrent;
            const GapIterator m_gapEnd;
    };

    template<class T>
    concept TriviallyDestructible = std::is_trivially_destructible<T>::value;

    /** Struct of arrays representation for any number of trivially destructible properties. */
    template<TriviallyDestructible... Properties>
    class SoA
    {
        public:
            SoA(uint32_t maxCount);

            void Add(Properties&&... properties);
            template<std::equality_comparable Property> void Remove(Property toRemove);
            template<class Property> std::span<Property> View() const;
            void Clear();
            bool IsFull() const;
            SoAIndex SmartIndex() const;

        private:
            std::tuple<std::unique_ptr<Properties[]>...> m_properties;
            std::set<uint32_t> m_gaps;
            uint32_t m_nextFree;
            uint32_t m_size;
    };

    template<class... Properties>
    SoA<Properties...>::SoA(uint32_t maxCount)
        : m_properties{std::make_unique<Properties[]>(maxCount)...},
          m_gaps{},
          m_nextFree{0u},
          m_size{maxCount}
    {
    }

    template<class... Properties>
    void SoA<Properties...>::Add(Properties&&... properties)
    {
        if(IsFull())
            throw std::runtime_error("SoA::Add - capacity exceeded");
        
        auto index = [this]()
        {
            if(m_gaps.empty())
                return m_nextFree++;
            
            auto out = m_gaps.extract(m_gaps.begin());
            return out.value();
        }();

        ( (View<Properties>()[index] = properties), ... );
    }

    template<class... Properties>
    template<std::equality_comparable Property>
    void SoA<Properties...>::Remove(Property toRemove)
    {
        auto targetProperties = View<Property>();
        auto index = SmartIndex();
        while(index.Valid())
        {
            if(targetProperties[index] == toRemove)
                break;

            ++index;
        }

        if(!index.Valid())
            throw std::runtime_error("SoA::Remove - property doesn't exists");
        
        m_gaps.insert(index);
    }

    template<class... Properties>
    void SoA<Properties...>::Clear()
    {
        m_gaps.clear();
        m_nextFree = 0u;
    }

    template<class... Properties>
    bool SoA<Properties...>::IsFull() const
    {
        return m_nextFree >= m_size && m_gaps.empty();
    }

    template<class... Properties>
    template<class Property>
    std::span<Property> SoA<Properties...>::View() const
    {
        return std::span{std::get<std::unique_ptr<Property[]>>(m_properties).get(), m_nextFree};
    }

    template<class... Properties>
    SoAIndex SoA<Properties...>::SmartIndex() const
    {
        // if the first gap is 0, find the starting index
        // and make sure begin is greater than it
        auto index = 0u;
        auto begin = m_gaps.cbegin();
        auto end = m_gaps.cend();
        while(begin != end && index == *begin)
        {
            ++index; ++begin;
        }

        return SoAIndex{index, m_nextFree, begin, end};
    }
} // namespace nc::ecs