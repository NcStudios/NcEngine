#pragma once

#include "Type.h"
#include "EditorMeta.h"

namespace nc
{
    template<class T>
    void PrintMember(std::ostream& os, const T& obj, const char* name, size_t depth = 0u);

    template<class T>
    void PrintType(std::ostream& os, const T& obj, size_t depth = 0u);

    template<class T>
    void PrintMember(std::ostream& os, const T& obj, const char* name, size_t depth)
    {
        using meta_type = Meta<T>;
        constexpr auto propertyCount = std::tuple_size<decltype(meta_type::properties)>::value;

        auto Tab = [&os](size_t count)
        {
            for(auto i = 0u; i < count; ++i) { os << "    "; }
        };

        Tab(depth);
        os << "[member: " << name << ", type: " << meta_type::name;

        if constexpr(meta_type::IsPrimitive)
        {
            os << ", value: " << meta_type::Get(obj);
        }

        os << "]\n";

        if constexpr(propertyCount != 0)
        {
            for_sequence(std::make_index_sequence<propertyCount>{}, [&](auto i)
            {
                constexpr auto property = std::get<i>(meta_type::properties);
                PrintMember(os, obj.*(property.member), property.name, depth + 1);
            });
        }
    }

    template<class T>
    void PrintType(std::ostream& os, const T& obj, size_t depth)
    {
        using meta_type = Meta<T>;
        constexpr auto propertyCount = std::tuple_size<decltype(meta_type::properties)>::value;

        auto Tab = [&os](size_t count)
        {
            for(auto i = 0u; i < count; ++i) { os << "    "; }
        };

        Tab(depth);
        os << meta_type::name;

        if constexpr(meta_type::IsPrimitive)
        {
            Tab(depth + 1);
            os << ": " << meta_type::Get(obj);
        }

        os << '\n';

        if constexpr(propertyCount != 0)
        {
            for_sequence(std::make_index_sequence<propertyCount>{}, [&](auto i)
            {
                constexpr auto property = std::get<i>(meta_type::properties);
                PrintMember(os, obj.*(property.member), property.name, depth + 1);
            });
        }
    }
}