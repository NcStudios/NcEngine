#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace nc
{
    struct SerializationFlags
    {
        static constexpr uint32_t None        = 0x0000;
        static constexpr uint32_t Nonnegative = 0x0001;
        static constexpr uint32_t Position    = 0x0010;
        static constexpr uint32_t Scale       = 0x0100;
        static constexpr uint32_t Angles      = 0x1000;
    };

    struct PropertyBase
    {
        constexpr PropertyBase()
            : name{"Null"}, flags{SerializationFlags::None}
        {
        }

        constexpr PropertyBase(const char* tag, uint32_t serializationFlags)
            : name{tag}, flags{serializationFlags}
        {
        }

        const char* name;
        uint32_t flags = SerializationFlags::None;
    };

    template<class T, class P>
    struct Property : public PropertyBase
    {
        using parent_type = T;
        using property_type = P;

        constexpr Property(P T::*ptr, const char* tag, uint32_t serializationFlags)
            : PropertyBase{tag, serializationFlags}, member{ptr}
        {
        }

        P T::*member;
    };

    template<class T>
    struct Meta
    {
        static constexpr auto IsPrimitive = false;
        static constexpr auto name = "Null";
        static constexpr auto properties = std::make_tuple();
    };

    template<class T, class P>
    auto GetProperty(const T& obj) -> P
    {
        auto property = std::get<Property<T, P>>(Meta<T>::properties);
        return obj.*(property.member);
    }

    template<class T, class P>
    constexpr auto MakeProperty(P T::*member, const char* name, uint32_t flags = SerializationFlags::None)
    {
        return Property<T, P>{member, name, flags};
    }

    template<class T, class Func>
    constexpr void ForEachMember(Func&& func)
    {
        std::apply([&func](auto&& ... args) { (func(args), ...); }, Meta<T>::properties);
    }

    #define PROPERTY(Class, Member)                                      \
    MakeProperty(&Class::Member, #Member, SerializationFlags::None)

    #define PROPERTY_F(Class, Member, Flags)                             \
    MakeProperty(&Class::Member, #Member, Flags)

    #define REGISTER_TYPE(Class, ...)                                    \
    template<> struct Meta<Class>                                        \
    {                                                                    \
        static constexpr bool IsPrimitive = false;                       \
        static constexpr const char* name = #Class;                      \
        static constexpr auto properties = std::make_tuple(__VA_ARGS__); \
    };

    #define REGISTER_PRIMITIVE_TYPE(Class)                               \
    template<> struct Meta<Class>                                        \
    {                                                                    \
        static constexpr bool IsPrimitive = true;                        \
        static constexpr const char* name = #Class;                      \
        static constexpr auto properties = std::make_tuple();            \
        static auto Get(Class obj) -> Class  { return obj; }             \
        static auto Set(Class* ptr, Class v) { *ptr = v;  }              \
    };

    REGISTER_PRIMITIVE_TYPE(float);
    REGISTER_PRIMITIVE_TYPE(int);
    REGISTER_PRIMITIVE_TYPE(char);
    REGISTER_PRIMITIVE_TYPE(const char*);
}