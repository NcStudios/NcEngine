#include <iostream>
#include <span>
#include <tuple>
#include <type_traits>
#include <concepts>
#include <memory>

template<class T>
concept SoAProperty = std::is_default_constructible_v<T> && std::is_trivially_destructible_v<T>;

struct SoALayoutBase {};

// template<SoAProperty Member>
// struct SoAArray
// {
//     using type = std::unique_ptr<Member[]>;
// };

template<SoAProperty... Members>
struct SoALayout : public SoALayoutBase
{
    static constexpr auto member_count = sizeof...(Members);

    using container_type = std::tuple<std::unique_ptr<Members[]>...>;

    using tuple_type = std::tuple<Members...>;

    static constexpr container_type CreateContainer(size_t size)
    {
        return std::tuple(std::make_unique<Members[]>(size)...);
    }
};


struct LayoutPolicy
{
    
};


template<class Layout, size_t Index>
struct SoAMember
{
    using type = std::tuple_element<Index, typename Layout::tuple_type>::type;
};


template<class Layout, size_t Index>
struct SoASpan
{
    using type = std::span<typename SoAMember<Layout, Index>::type>;

    static constexpr type Get(const Layout::container_type& container, size_t size)
    {
        return type(std::get<Index>(container).get(), size);
    }
};

template<std::derived_from<SoALayoutBase> Layout>
class SoA
{
    public:
        SoA(size_t maxCount)
            : m_tuple{Layout::CreateContainer(maxCount)},
              m_currentCount{0u},
              m_maxCount{maxCount}
        {
        }

        template<size_t I>
        auto GetSpan()
        {
            return SoASpan<Layout, I>::Get(m_tuple, m_currentCount);
        }

    private:
        Layout::container_type m_tuple;
        size_t m_currentCount;
        size_t m_maxCount;

};

struct TestObj : public SoALayout<float, int>
{
    float f;
    int i;
};

int main()
{
    SoA<TestObj> test{10};

    test.GetSpan<1>();
    test.GetPtr<1>();

    return 0;
}