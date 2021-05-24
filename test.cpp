#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <span>
#include <type_traits>
#include <concepts>
#include <cstdint>
#include <limits>
#include <algorithm>

#include "nc/include/entity/EntityHandle.h"

using namespace nc;

template<class T>
class UnorderedPool
{
    public:
        UnorderedPool()
            : m_data{}
        {}

        size_t Size() const { return m_data.size(); }

        template<class... Args>
        T* Emplace(Args&&... args)
        {
            return &m_data.emplace_back(std::forward<Args>(args)...);
        }

        EntityHandle Remove(HandleTraits::index_type i)
        {
            m_data.at(i) = std::move(m_data.back());
            m_data.pop_back();
            return m_data[i].GetParentHandle();
        }

        T* Get(HandleTraits::index_type i) { return &m_data.at(i); }
        std::span<T> View() { return std::span{m_data}; }
        void Clear() { m_data.clear(); }

    private:
        std::vector<T> m_data;
};

template<class... Ts>
class WorldRegistry
{
    template<class T>
    using storage_type = UnorderedPool<T>;
    using index_type = HandleTraits::index_type;
    using pools_type = std::tuple<storage_type<Ts>...>;

    static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();

    struct TypedSequence
    {
        template<class T>
        struct TypedIndex
        {
            index_type value = std::numeric_limits<index_type>::max();
        };

        std::tuple<TypedIndex<Ts>...> indices;
    };

    public:
        WorldRegistry(size_t maxEntities)
            : m_sparse{maxEntities},
              m_pools{storage_type<Ts>{}...}
        {}

        template<class T, class... Args>
        T* Emplace(EntityHandle handle, Args&&... args)
        {
            if(Contains<T>(handle))
                throw std::runtime_error("Registry::Emplace - Cannot add multiple components of the same type");
            
            auto& pool = std::get<storage_type<T>>(m_pools);
            auto* ptr = pool.Emplace(handle, std::forward<Args>(args)...);
            auto index = pool.Size() - 1;

            SetDenseIndex<T>(handle.Index(), index);
            return ptr;
        }

        template<class T>
        auto GetDenseIndex(HandleTraits::index_type sparseIndex) -> HandleTraits::index_type
        {
            auto& typeSequence = m_sparse.at(sparseIndex);
            return std::get<typename TypedSequence::TypedIndex<T>>(typeSequence.indices).value;
        }

        template<class T>
        void SetDenseIndex(HandleTraits::index_type sparseIndex, HandleTraits::index_type value)
        {
            auto& typeSequence = m_sparse.at(sparseIndex);
            std::get<typename TypedSequence::TypedIndex<T>>(typeSequence.indices).value = value;
        }

        template<class T>
        bool Remove(EntityHandle handle)
        {
            auto sparseIndex = handle.Index();
            auto denseIndex = GetDenseIndex<T>(sparseIndex);
            if(denseIndex == NullIndex)
                return false;
            
            auto& pool = std::get<storage_type<T>>(m_pools);
            auto movedHandle = pool.Remove(denseIndex);

            if(movedHandle != handle)
            {
                auto movedSparseIndex = movedHandle.Index();
                SetDenseIndex<T>(movedSparseIndex, denseIndex);
            }

            SetDenseIndex<T>(sparseIndex, NullIndex);
            return true;
        }

        template<class T>
        bool Contains(EntityHandle handle)
        {
            auto sparseIndex = handle.Index();
            auto denseIndex = GetDenseIndex<T>(sparseIndex);
            return denseIndex == NullIndex ? false : true;
        }

        template<class T>
        T* Get(EntityHandle handle)
        {
            auto denseIndex = GetDenseIndex<T>(handle.Index());
            return denseIndex == NullIndex ? nullptr : std::get<storage_type<T>>(m_pools).Get(denseIndex);
        }

        template<class T>
        std::span<T> View()
        {
            auto& pool = std::get<storage_type<T>>(m_pools);
            return pool.View();
        }

        void Clear()
        {
            std::apply([](auto&&... pools)
            {
                (pools.Clear(), ...);
            }, m_pools);

            std::ranges::fill(m_sparse, TypedSequence{});
        }

    private:
        std::vector<TypedSequence> m_sparse;
        pools_type m_pools;
};

struct C1
{
    C1(EntityHandle handle)
        : h{handle}
    {}

    EntityHandle GetParentHandle() { return h; }

    EntityHandle h;
};

struct C2
{
    C2(EntityHandle handle)
        : h{handle}
    {}

    EntityHandle GetParentHandle() { return h; }

    EntityHandle h;
};

int main()
{
    EntityHandle h1{0u, 0u, 0u, 0u};
    WorldRegistry<C1, C2> wr{10u};

    auto ptr = wr.Emplace<C1>(h1);
    std::cout << "ptr: " << ptr << '\n';

    auto has = wr.Contains<C1>(h1);
    std::cout << "has: " << has << '\n';

    auto rem = wr.Remove<C1>(h1);
    std::cout << "rem: " << rem << '\n';

    has = wr.Contains<C1>(h1);
    std::cout << "has: " << has << '\n';

    rem = wr.Remove<C1>(h1);
    std::cout << "rem: " << rem << '\n';

    ptr = wr.Emplace<C1>(h1);
    std::cout << "ptr: " << ptr << '\n';

    has = wr.Contains<C1>(h1);
    std::cout << "has: " << has << '\n';

    wr.Clear();
    std::cout << "clear\n";

    has = wr.Contains<C1>(h1);
    std::cout << "has: " << has << '\n';

    return 0;
}