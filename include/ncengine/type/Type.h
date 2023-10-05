#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>

namespace nc::type
{
template<class T>
struct Type
{
    static constexpr auto name = "Null";
    static constexpr auto properties = std::make_tuple();
    static constexpr auto propertyCount = 0u;
    static constexpr auto isPrimitive = false;
};

#define REGISTER_TYPE(Class, ...)                                                       \
template<> struct Type<Class>                                                           \
{                                                                                       \
    static constexpr const char* name = #Class;                                         \
    static constexpr auto properties = std::make_tuple(__VA_ARGS__);                    \
    static constexpr auto propertyCount = std::tuple_size<decltype(properties)>::value; \
    static constexpr bool isPrimitive = false;                                          \
};

#define REGISTER_PRIMITIVE_TYPE(Class)                                                  \
template<> struct Type<Class>                                                           \
{                                                                                       \
    static constexpr const char* name = #Class;                                         \
    static constexpr auto properties = std::make_tuple();                               \
    static constexpr auto propertyCount = 0u;                                           \
    static constexpr bool isPrimitive = true;                                           \
    static auto Get(Class obj) -> Class  { return obj; }                                \
    static auto Set(Class* ptr, Class v) { *ptr = v;  }                                 \
};

struct PropertyFlags
{
    static constexpr uint32_t None        = 0b0000;
    static constexpr uint32_t Nonnegative = 0b0001;
    static constexpr uint32_t Position    = 0b0010;
    static constexpr uint32_t Scale       = 0b0100;
    static constexpr uint32_t Angles      = 0b1000;
};

struct PropertyBase
{
    constexpr PropertyBase()
        : name{"Null"}, flags{PropertyFlags::None}
    {
    }

    constexpr PropertyBase(const char* tag, uint32_t flags_)
        : name{tag}, flags{flags_}
    {
    }

    const char* name;
    uint32_t flags;
};

template<class T, class P>
struct Property : public PropertyBase
{
    using parent_type = T;
    using property_type = P;

    constexpr Property(P T::*ptr, const char* tag, uint32_t propertyFlags)
        : PropertyBase{tag, propertyFlags}, member{ptr}
    {
    }

    P T::*member;
};

#define PROPERTY(Class, Member) \
Property<Class, decltype(Class::Member)>{&Class::Member, #Member, PropertyFlags::None}

#define PROPERTY_F(Class, Member, Flags) \
Property<Class, decltype(Class::Member)>{&Class::Member, #Member, Flags}

template<class T, class P>
auto GetProperty(const T& obj) -> P
{
    auto property = std::get<Property<T, P>>(Type<T>::properties);
    return obj.*(property.member);
}

template<class T, class Func>
constexpr void ForEachMember(Func&& func)
{
    std::apply([&func](auto&& ... args) { (func(args), ...); }, Type<T>::properties);
}

REGISTER_PRIMITIVE_TYPE(float);
REGISTER_PRIMITIVE_TYPE(int);
REGISTER_PRIMITIVE_TYPE(char);
REGISTER_PRIMITIVE_TYPE(const char*);
} // namespace nc::type
