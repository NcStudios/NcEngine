#pragma once

#include "ncutility/NcError.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

/** @cond internal */
namespace nc::serialize::binary
{
template<class T>
concept TriviallyCopyable = requires { requires std::is_trivially_copyable_v<T>; };

template<class T>
concept Aggregate = requires { requires std::is_aggregate_v<T>; };

// Max supported member count for automatic aggregate serialization
inline constexpr auto g_aggregateMaxMemberCount = 16ull;

// Arbitrarily large value returned from MemberCount() when a type has too many members.
inline constexpr size_t g_failedMemberCount = 0xFFFFFFFFFFFFFFFF;

// A type which is implicitly convertable to all other types
struct UniversalType{ template<class T> operator T() const; };

// Count the number of members in an aggregate, returns g_failedMemberCount for types with > 16 members
template<class T>
    requires requires { std::is_aggregate_v<T>; }
consteval auto MemberCount() -> size_t
{
    using U = UniversalType;
    if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return g_failedMemberCount;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 16ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 15ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 14ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 13ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 12ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 11ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 10ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 9ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 8ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 7ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}, U{}}; })
        return 6ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}, U{}}; })
        return 5ull;
    else if constexpr (requires { T{U{}, U{}, U{}, U{}}; })
        return 4ull;
    else if constexpr (requires { T{U{}, U{}, U{}}; })
        return 3ull;
    else if constexpr (requires { T{U{}, U{}}; })
        return 2ull;
    else if constexpr (requires { T{U{}}; })
        return 1ull;
    else
        return 0ull;
}

// Concept for aggregate types that have automatic serialization support
template<class T>
concept UnpackableAggregate = Aggregate<T>
                          && !TriviallyCopyable<T>
                          && (MemberCount<T>() <= g_aggregateMaxMemberCount);

template<class T>
void Serialize(std::ostream& stream, const T& in);

template<TriviallyCopyable T>
void Serialize(std::ostream& stream, const T& in);

template<UnpackableAggregate T>
void Serialize(std::ostream& stream, const T& in);

template<class T>
void Serialize(std::ostream& stream, const std::vector<T>& in);

template<class T, size_t I>
void Serialize(std::ostream& stream, const std::array<T, I>& in);

template<class T, class U>
void Serialize(std::ostream& stream, const std::pair<T, U>& in);

template<class K, class V>
void Serialize(std::ostream& stream, const std::unordered_map<K, V>& in);

template<class T>
void Serialize(std::ostream& stream, const std::optional<T>& in);

template<class T>
void Deserialize(std::istream& stream, T& out);

template<TriviallyCopyable T>
void Deserialize(std::istream& stream, T& in);

template<UnpackableAggregate T>
void Deserialize(std::istream& stream, T& out);

template<class T>
void Deserialize(std::istream& stream, std::vector<T>& out);

template<class T, size_t I>
void Deserialize(std::istream& stream, std::array<T, I>& out);

template<class T, class U>
void Deserialize(std::istream& stream, std::pair<T, U>& out);

template<class K, class V>
void Deserialize(std::istream& stream, std::unordered_map<K, V>& out);

template<class T>
void Deserialize(std::istream& stream, std::optional<T>& out);

template<class... Args>
void SerializeMultiple(std::ostream& stream, Args&&... args)
{
    (Serialize(stream, args), ...);
}

template<class... Args>
void DeserializeMultiple(std::istream& stream, Args&&... args)
{
    (Deserialize(stream, args), ...);
}

template<class C>
void SerializeTrivialContainer(std::ostream& stream, const C& container)
{
    Serialize(stream, container.size());
    stream.write(reinterpret_cast<const char*>(container.data()), sizeof(typename C::value_type) * container.size());
}

template<class C>
void DeserializeTrivialContainer(std::istream& stream, C& container)
{
    auto size = size_t{};
    Deserialize(stream, size);
    container.resize(size);
    stream.read(reinterpret_cast<char*>(container.data()), sizeof(typename C::value_type) * size);
}

template<class C>
void SerializeNonTrivialContainer(std::ostream& stream, const C& container)
{
    auto size = container.size();
    stream.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (const auto& obj : container) Serialize(stream, obj);
}

template<class C>
void DeserializeNonTrivialContainer(std::istream& stream, C& container)
{
    auto count = size_t{};
    Deserialize(stream, count);
    container.reserve(count);
    std::generate_n(std::back_inserter(container), count, [&stream]()
    {
        auto out = typename C::value_type{};
        Deserialize(stream, out);
        return out;
    });
}

template<TriviallyCopyable T>
void Serialize(std::ostream& stream, const T& in)
{
    stream.write(reinterpret_cast<const char*>(&in), sizeof(T));
}

template<TriviallyCopyable T>
void Deserialize(std::istream& stream, T& out)
{
    stream.read(reinterpret_cast<char*>(&out), sizeof(T));
}

template<>
inline void Serialize(std::ostream& stream, const std::string& in)
{
    SerializeTrivialContainer(stream, in);
}

template<>
inline void Deserialize(std::istream& stream, std::string& out)
{
    DeserializeTrivialContainer(stream, out);
}

template<class T>
void Serialize(std::ostream& stream, const std::vector<T>& in)
{
    if constexpr (std::is_trivially_copyable_v<T>)
        SerializeTrivialContainer(stream, in);
    else
        SerializeNonTrivialContainer(stream, in);
}

template<class T>
void Deserialize(std::istream& stream, std::vector<T>& out)
{
    if constexpr (std::is_trivially_copyable_v<T>)
        DeserializeTrivialContainer(stream, out);
    else
        DeserializeNonTrivialContainer(stream, out);
}

template<class T, size_t I>
void Serialize(std::ostream& stream, const std::array<T, I>& in)
{
    if constexpr(std::is_trivially_copyable_v<T>)
        SerializeTrivialContainer(stream, in);
    else
        SerializeNonTrivialContainer(stream, in);
}

template<class T, size_t I>
void Deserialize(std::istream& stream, std::array<T, I>& out)
{
    auto size = size_t{};
    Deserialize(stream, size);
    NC_ASSERT(size == out.size(), "Expected array size does not match stream contents");

    if constexpr(std::is_trivially_copyable_v<T>)
    {
        stream.read(reinterpret_cast<char*>(out.data()), sizeof(T) * size);
    }
    else
    {
        std::ranges::for_each(out, [&stream](auto&& obj)
        {
            Deserialize(stream, obj);
        });
    }
}

template<class T, class U>
void Serialize(std::ostream& stream, const std::pair<T, U>& in)
{
    SerializeMultiple(stream, in.first, in.second);
}

template<class T, class U>
void Deserialize(std::istream& stream, std::pair<T, U>& out)
{
    DeserializeMultiple(stream, out.first, out.second);
}

template<class K, class V>
void Serialize(std::ostream& stream, const std::unordered_map<K, V>& in)
{
    SerializeNonTrivialContainer(stream, in);
}

template<class K, class V>
void Deserialize(std::istream& stream, std::unordered_map<K, V>& out)
{
    auto count = size_t{};
    Deserialize(stream, count);
    out.reserve(count);
    std::generate_n(std::inserter(out, std::end(out)), count, [&stream]()
    {
        auto pair = std::pair<K, V>{};
        Deserialize(stream, pair);
        return pair;
    });
}

template<class T>
void Serialize(std::ostream& stream, const std::optional<T>& in)
{
    in.has_value()
        ? SerializeMultiple(stream, true, in.value())
        : Serialize(stream, false);
}

template<class T>
void Deserialize(std::istream& stream, std::optional<T>& out)
{
    auto hasValue = false;
    Deserialize(stream, hasValue);
    if (hasValue)
    {
        out = T{};
        Deserialize(stream, out.value());
    }
    else
    {
        out = std::nullopt;
    }
}

template<UnpackableAggregate T>
void Serialize(std::ostream& stream, const T& in)
{
    static constexpr auto memberCount = MemberCount<T>();
    static_assert(memberCount <= g_aggregateMaxMemberCount);

    if constexpr (memberCount == 16)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16);
    }
    else if constexpr (memberCount == 15)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15);
    }
    else if constexpr (memberCount == 14)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14);
    }
    else if constexpr (memberCount == 13)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13);
    }
    else if constexpr (memberCount == 12)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12);
    }
    else if constexpr (memberCount == 11)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11);
    }
    else if constexpr (memberCount == 10)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
    }
    else if constexpr (memberCount == 9)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9);
    }
    else if constexpr (memberCount == 8)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8);
    }
    else if constexpr (memberCount == 7)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7);
    }
    else if constexpr (memberCount == 6)
    {
        const auto& [m1, m2, m3, m4, m5, m6] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5, m6);
    }
    else if constexpr (memberCount == 5)
    {
        const auto& [m1, m2, m3, m4, m5] = in;
        SerializeMultiple(stream, m1, m2, m3, m4, m5);
    }
    else if constexpr (memberCount == 4)
    {
        const auto& [m1, m2, m3, m4] = in;
        SerializeMultiple(stream, m1, m2, m3, m4);
    }
    else if constexpr (memberCount == 3)
    {
        const auto& [m1, m2, m3] = in;
        SerializeMultiple(stream, m1, m2, m3);
    }
    else if constexpr (memberCount == 2)
    {
        const auto& [m1, m2] = in;
        SerializeMultiple(stream, m1, m2);
    }
    else if constexpr (memberCount == 1)
    {
        const auto& [m1] = in;
        SerializeMultiple(stream, m1);
    }
}

template<UnpackableAggregate T>
void Deserialize(std::istream& stream, T& out)
{
    static constexpr auto memberCount = MemberCount<T>();
    static_assert(memberCount <= g_aggregateMaxMemberCount);

    if constexpr (memberCount == 16)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16);
    }
    else if constexpr (memberCount == 15)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15);
    }
    else if constexpr (memberCount == 14)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14);
    }
    else if constexpr (memberCount == 13)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13);
    }
    else if constexpr (memberCount == 12)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12);
    }
    else if constexpr (memberCount == 11)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11);
    }
    else if constexpr (memberCount == 10)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
    }
    else if constexpr (memberCount == 9)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8, m9);
    }
    else if constexpr (memberCount == 8)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7, m8);
    }
    else if constexpr (memberCount == 7)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7] = out;
       DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6, m7);
    }
    else if constexpr (memberCount == 6)
    {
        auto& [m1, m2, m3, m4, m5, m6] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5, m6);
    }
    else if constexpr (memberCount == 5)
    {
        auto& [m1, m2, m3, m4, m5] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4, m5);
    }
    else if constexpr (memberCount == 4)
    {
        auto& [m1, m2, m3, m4] = out;
        DeserializeMultiple(stream, m1, m2, m3, m4);
    }
    else if constexpr (memberCount == 3)
    {
        auto& [m1, m2, m3] = out;
        DeserializeMultiple(stream, m1, m2, m3);
    }
    else if constexpr (memberCount == 2)
    {
        auto& [m1, m2] = out;
        DeserializeMultiple(stream, m1, m2);
    }
    else if constexpr (memberCount == 1)
    {
        auto& [m1] = out;
        DeserializeMultiple(stream, m1);
    }
}
} // namespace nc::serialize::binary
/** @endcond internal */
