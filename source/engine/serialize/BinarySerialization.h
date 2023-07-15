#pragma once

#include "BinaryBuffer.h"

namespace nc::serialize
{
/**
 * @brief Serialize an object to a BinaryBuffer.
 * @param buffer The BinaryBuffer to write to.
 * @param in The object to serialize.
 * @note Specializations of Seserialize() may be provided for handling
 *       user types. Overloads for Seserialize() are provided for:
 *       - trivially copyable types
 *       - std array, vector, and string
 *       - aggregates types with <= 10 members, each satisfying at least
 *         one of these requirements
 */
template<class T>
void Serialize(BinaryBuffer& buffer, const T& in);

/**
 * @brief Deserialize an object from a BinaryBuffer.
 * @param buffer The BinaryBuffer to read from.
 * @param out The object to deserialize to.
 * @note Specializations of Deserialize() may be provided for handling
 *       user types. Overloads for Deserialize() are provided for:
 *       - trivially copyable types
 *       - std array, vector, and string
 *       - aggregates types with <= 10 members, each satisfying at least
 *         one of these requirements
 */
template<class T>
void Deserialize(BinaryBuffer& buffer, T& out);

/** @brief The maximum number of members an aggregate may have for
 *         default serialization. */
constexpr auto g_maxMemberCount = 10ull;

namespace detail
{
template<class Container>
void SerializeTrivialContainer(BinaryBuffer& buffer, const Container& container)
{
    buffer.Write(container.size());
    buffer.Write(container.data(), sizeof(Container::value_type) * container.size());
}

template<class Container>
void SerializeNonTrivialContainer(BinaryBuffer& buffer, const Container& container)
{
    buffer.Write(container.size());
    for (const auto& item : container)
    {
        Serialize(buffer, item);
    }
}

template<class Container>
void DeserializeTrivialContainer(BinaryBuffer& buffer, Container& container)
{
    auto size = size_t{};
    buffer.Read(size);
    container.resize(size);
    buffer.Read(container.data(), sizeof(Container::value_type) * size);
}

template<class Container>
void DeserializeNonTrivialContainer(BinaryBuffer& buffer, Container& container)
{
    auto count = size_t{};
    buffer.Read(count);
    container.reserve(count);
    std::generate_n(std::back_inserter(container), count, [&buffer]()
    {
        auto out = typename Container::value_type{};
        Deserialize(buffer, out);
        return out;
    });
}
} // namespace detail

template<type::TriviallyCopyable T>
void Serialize(BinaryBuffer& buffer, const T& in)
{
    buffer.Write(in);
}

template<>
inline void Serialize(BinaryBuffer& buffer, const std::string& in)
{
    detail::SerializeTrivialContainer(buffer, in);
}

template<type::TriviallyCopyable T>
void Serialize(BinaryBuffer& buffer, const std::vector<T>& in)
{
    detail::SerializeTrivialContainer(buffer, in);
}

template<class T>
void Serialize(BinaryBuffer& buffer, const std::vector<T>& in)
{
    detail::SerializeNonTrivialContainer(buffer, in);
}

template<type::TriviallyCopyable T, size_t I>
void Serialize(BinaryBuffer& buffer, const std::array<T, I>& in)
{
    detail::SerializeTrivialContainer(buffer, in);
}

template<class T, size_t I>
void Serialize(BinaryBuffer& buffer, const std::array<T, I>& in)
{
    detail::SerializeNonTrivialContainer(buffer, in);
}

template<class... Args>
void SerializeMultiple(BinaryBuffer& buffer, Args&&... args)
{
    (Serialize(buffer, args), ...);
}

template<class T>
void Serialize(BinaryBuffer& buffer, const T& in)
{
    // We want a concept like this here:
    //   requires Aggregate<T> && !std::is_trivially_copyable<T>
    // but MSVC doesn't like it. Using asserts instead.
    static_assert(!std::is_trivially_copyable_v<T>);
    static_assert(type::Aggregate<T>, "Cannot automatically serialize non-aggregate types. "
                                      "Add a specialization of Serialize<T>().");

    constexpr auto memberCount = type::CountAggregateMembers<T>();
    static_assert(memberCount <= g_maxMemberCount);
    static_assert(memberCount > 0); // maybe we don't care?

    if constexpr (memberCount == 10)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
    }
    else if constexpr (memberCount == 9)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9);
    }
    else if constexpr (memberCount == 8)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7, m8] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8);
    }
    else if constexpr (memberCount == 7)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7);
    }
    else if constexpr (memberCount == 6)
    {
        auto&& [m1, m2, m3, m4, m5, m6] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6);
    }
    else if constexpr (memberCount == 5)
    {
        auto&& [m1, m2, m3, m4, m5] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4, m5);
    }
    else if constexpr (memberCount == 4)
    {
        auto&& [m1, m2, m3, m4] = in;
        SerializeMultiple(buffer, m1, m2, m3, m4);
    }
    else if constexpr (memberCount == 3)
    {
        auto&& [m1, m2, m3] = in;
        SerializeMultiple(buffer, m1, m2, m3);
    }
    else if constexpr (memberCount == 2)
    {
        auto&& [m1, m2] = in;
        SerializeMultiple(buffer, m1, m2);
    }
    else if constexpr (memberCount == 1)
    {
        auto&& [m1] = in;
        SerializeMultiple(buffer, m1);
    }
}

template<type::TriviallyCopyable T>
void Deserialize(BinaryBuffer& buffer, T& out)
{
    buffer.Read(out);
}

template<>
inline void Deserialize(BinaryBuffer& buffer, std::string& out)
{
    auto size = size_t{};
    buffer.Read(size);
    out.resize(size);
    buffer.Read(out.data(), size);
}

template<type::TriviallyCopyable T>
void Deserialize(BinaryBuffer& buffer, std::vector<T>& out)
{
    detail::DeserializeTrivialContainer(buffer, out);
}

template<class T>
void Deserialize(BinaryBuffer& buffer, std::vector<T>& out)
{
    detail::DeserializeNonTrivialContainer(buffer, out);
}

template<type::TriviallyCopyable T, size_t I>
void Deserialize(BinaryBuffer& buffer, std::array<T, I>& out)
{
    auto size = size_t{};
    buffer.Read(size);
    buffer.Read(out.data(), sizeof(T) * out.size());
}

template<class T, size_t I>
void Deserialize(BinaryBuffer& buffer, std::array<T, I>& out)
{
    auto count = size_t{};
    buffer.Read(count);
    for (auto& item : out)
    {
        Deserialize(buffer, item);
    }
}

template<class... Args>
void DeserializeMultiple(BinaryBuffer& buffer, Args&&... args)
{
    (Deserialize(buffer, args), ...);
}

template<class T>
void Deserialize(BinaryBuffer& buffer, T& out)
{
    static_assert(!std::is_trivially_copyable_v<T>);
    static_assert(type::Aggregate<T>, "Cannot automatically serialize non-aggregate types. "
                                      "Add a specialization of Serialize<T>().");

    constexpr auto memberCount = type::CountAggregateMembers<T>();
    static_assert(memberCount <= g_maxMemberCount);
    static_assert(memberCount > 0); // maybe we don't care?

    if constexpr (memberCount == 10)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
    }
    else if constexpr (memberCount == 9)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9);
    }
    else if constexpr (memberCount == 8)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7, m8] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8);
    }
    else if constexpr (memberCount == 7)
    {
        auto&& [m1, m2, m3, m4, m5, m6, m7] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7);
    }
    else if constexpr (memberCount == 6)
    {
        auto&& [m1, m2, m3, m4, m5, m6] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6);
    }
    else if constexpr (memberCount == 5)
    {
        auto&& [m1, m2, m3, m4, m5] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4, m5);
    }
    else if constexpr (memberCount == 4)
    {
        auto&& [m1, m2, m3, m4] = out;
        DeserializeMultiple(buffer, m1, m2, m3, m4);
    }
    else if constexpr (memberCount == 3)
    {
        auto&& [m1, m2, m3] = out;
        DeserializeMultiple(buffer, m1, m2, m3);
    }
    else if constexpr (memberCount == 2)
    {
        auto&& [m1, m2] = out;
        DeserializeMultiple(buffer, m1, m2);
    }
    else if constexpr (memberCount == 1)
    {
        auto&& [m1] = out;
        DeserializeMultiple(buffer, m1);
    }
}
} // namespace nc::serialize
