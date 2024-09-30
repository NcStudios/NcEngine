/**
 * @file BinarySerialization.h
 * @copyright Copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#if defined(__APPLE__)
    /** @note While this is likely fine on apple/clang, tests can't build
     * in CI. Disabling consumption on macOS until there is test coverage. */
    #error "BinarySerialization.h is currently unsupported on macOS."
#endif

#include "ncutility/detail/SerializeCpo.h"

namespace nc::serialize
{
/**
 * @brief Serialize an object to a stream.
 * 
 * Serialize and Deserialize are function objects with call signatures:
 *     `void Serialize(std::ostream&, const T&)`
 *     `void Deserialize(std::istream&, T&)`
 * 
 * Calls to them are equivalent to the first matched valid expression among:
 *   1. A non-static member function with the signature:
 *        `void T::Serialize(std::ostream&) const`
 *        `void T::Deserialize(std::istream&)`
 *   2. A non-member function found via adl with the signature:
 *        `void Serialize(std::ostream&, const T&)`
 *        `void Deserialize(std::istream&, T&)`
 *   3. A non-member function in the `nc::serialize::binary` namespace with the signature:
 *        `void nc::serialize::binary::Serialize(std::ostream&, const T&)`
 *        `void nc::serialize::binary::Deserialize(std::istream&, T&)`
 * 
 * The following are supported by the overloads from number 3:
 *   - Trivially copyable types
 *   - Stl types: string, array, vector, unordered_map, pair, and optional
 *   - Aggregates with <= 16 members, each satisfying at least one
 *     of the above requirements
 */
inline constexpr nc::serialize::cpo::SerializeFn Serialize;

/**
 * @brief Deserialize an object from a stream.
 * @copydetails Serialize
 */
inline constexpr nc::serialize::cpo::DeserializeFn Deserialize;

/** @brief The maximum number of members an aggregate may have for default serialization. */
inline constexpr size_t g_aggregateMaxMemberCount = 16ull;
} // namespace nc::seriazlize
