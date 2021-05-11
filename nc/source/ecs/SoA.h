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
        using GapIterator = std::set<size_t>::const_iterator;
        
        public:
            SoAIndex(size_t begin, size_t end, GapIterator gapBegin, GapIterator gapEnd);

            SoAIndex& operator++();
            operator size_t() const;
            bool Valid() const;

        private:
            size_t m_current;
            const size_t m_end;
            GapIterator m_gapCurrent;
            const GapIterator m_gapEnd;
    };

    template<class T>
    concept SoAMember = std::is_default_constructible_v<T> && std::is_trivially_destructible_v<T>;

    /** Struct of arrays representation for any number of trivially destructible properties. */
    template<SoAMember... Members>
    class SoA
    {
        template<size_t Index>
        using member_type = std::tuple_element<Index, std::tuple<Members...>>::type;

        template<size_t Index>
        using span_type = std::span<member_type<Index>>;

        template<size_t... Indices>
        using view_type = std::tuple<SoAIndex, span_type<Indices>...>;

        public:
            SoA(size_t maxCount);

            void Add(Members... members);

            void RemoveAtIndex(size_t index);

            template<SoAMember Member>
                requires std::equality_comparable<Member>
            void Remove(Member toRemove);

            template<class Member>
            auto GetSpan() const -> std::span<Member>;
            
            template<size_t Index>
            auto GetSpanFromIndex() const -> std::span<member_type<Index>>;

            template<size_t... Indices>
            auto View() const -> view_type<Indices...>;

            void Clear();
            bool IsFull() const;
            size_t GetCapacity() const;
            size_t GetSize() const;
            size_t GetRemainingSpace() const;
            SoAIndex SmartIndex() const;

        private:
            template<size_t TupleIndex = 0u, SoAMember First, SoAMember... Rest>
            void Assign(size_t index, First&& first, Rest&&... rest);

            std::tuple<std::unique_ptr<Members[]>...> m_members;
            std::set<size_t> m_gaps;
            size_t m_nextFree;
            size_t m_capacity;
    };

    template<SoAMember... Members>
    SoA<Members...>::SoA(size_t maxCount)
        : m_members{std::make_unique<Members[]>(maxCount)...},
          m_gaps{},
          m_nextFree{0u},
          m_capacity{maxCount}
    {
    }

    template<SoAMember... Members>
    void SoA<Members...>::Add(Members... members)
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

        Assign(index, std::forward<Members>(members)...);
    }

    template<SoAMember... Members>
    void SoA<Members...>::RemoveAtIndex(size_t index)
    {
        // throw if doesn't exist? return false? (then other remove should be modified)
        m_gaps.insert(index);
    }

    template<SoAMember... Members>
    template<SoAMember Member>
        requires std::equality_comparable<Member>
    void SoA<Members...>::Remove(Member toRemove)
    {
        auto targetMembers = GetSpan<Member>();
        auto index = SmartIndex();
        while(index.Valid())
        {
            if(targetMembers[index] == toRemove)
                break;

            ++index;
        }

        if(!index.Valid())
            throw std::runtime_error("SoA::Remove - member doesn't exists");
        
        m_gaps.insert(index);
    }

    template<SoAMember... Members>
    void SoA<Members...>::Clear()
    {
        m_gaps.clear();
        m_nextFree = 0u;
    }

    template<SoAMember... Members>
    bool SoA<Members...>::IsFull() const
    {
        return m_nextFree >= m_capacity && m_gaps.empty();
    }

    template<SoAMember... Members>
    template<class Member>
    auto SoA<Members...>::GetSpan() const -> std::span<Member>
    {
        return std::span{std::get<std::unique_ptr<Member[]>>(m_members).get(), m_nextFree};
    }

    template<SoAMember... Members>
    template<size_t Index>
    auto SoA<Members...>::GetSpanFromIndex() const -> std::span<member_type<Index>>
    {
        return std::span{std::get<Index>(m_members).get(), m_nextFree};
    }

    template<SoAMember... Members>
    template<size_t... I>
    auto SoA<Members...>::View() const -> view_type<I...>
    {
        return std::tuple{SmartIndex(), GetSpanFromIndex<I>()...};
    }

    template<SoAMember... Members>
    size_t SoA<Members...>::GetCapacity() const
    {
        return m_capacity;
    }
    
    template<SoAMember... Members>
    size_t SoA<Members...>::GetSize() const
    {
        //IF_THROW(m_gaps.size() > m_nextFree, "SoA::GetSize - Free/gaps mismatch resulting in underflow");
        return m_nextFree - m_gaps.size();
    }
    
    template<SoAMember... Members>
    size_t SoA<Members...>::GetRemainingSpace() const
    {
        return m_capacity - GetSize();
    }

    template<SoAMember... Members>
    SoAIndex SoA<Members...>::SmartIndex() const
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

    template<SoAMember... Members>
    template<size_t TupleIndex, SoAMember First, SoAMember... Rest>
    void SoA<Members...>::Assign(size_t index, First&& first, Rest&&... rest)
    {
        std::get<TupleIndex>(m_members)[index] = first;
        if constexpr(sizeof...(rest) > 0u)
            Assign<TupleIndex + 1u>(index, std::forward<Rest>(rest)...);
    }
} // namespace nc::ecs