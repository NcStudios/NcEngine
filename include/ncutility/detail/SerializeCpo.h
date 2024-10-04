#pragma once

#include "BinarySerializationDetail.h"

/** @cond internal */
namespace nc::serialize::cpo
{
// Helper to allow static_assert(false) prior to c++23.
template <class>
inline constexpr bool g_alwaysFalse = false;

// Indicates how a (de)serialize call will be resolved.
enum class Dispatch { None, Member, Adl, Default };

// Satisfied for types that have a Serialize member function.
template <class T>
concept HasSerializeMember = requires(std::ostream& stream, const T& obj)
{
    { obj.Serialize(stream) } -> std::same_as<void>;
};

// Satisfied for types that have a Serialize function in their namespace.
template <class T>
concept HasSerializeAdl = requires(std::ostream& stream, const T& obj)
{
    { Serialize(stream, obj) } -> std::same_as<void>;
};

// Satisfied for types that have a compatible Serialize function internally.
template <class T>
concept HasSerializeDefault = requires(std::ostream& stream, const T& obj)
{
    { nc::serialize::binary::Serialize(stream, obj) } -> std::same_as<void>;
};

// CPO for nc::serialize::Serialize - dispatches to a `Serialize()` function that is either
// a member of T, non-member found via adl, or internal non- member depending on what is
// available. Resolution is attempted in that order.
struct SerializeFn
{
    private:
        template<class T>
        static consteval auto GetDispatch() -> Dispatch
        {
            if constexpr(HasSerializeMember<T>)
                return Dispatch::Member;
            else if constexpr(HasSerializeAdl<T>)
                return Dispatch::Adl;
            else if constexpr(HasSerializeDefault<T>)
                return Dispatch::Default;
            else
                return Dispatch::None;
        }

        template<class T>
        static constexpr auto Strategy = GetDispatch<T>();

    public:
        template<class T>
            requires (Strategy<T> != Dispatch::None)
        auto operator()(std::ostream& stream, const T& obj) const
        {
            constexpr auto dispatch = Strategy<T>;
            if constexpr(dispatch == Dispatch::Member)
                obj.Serialize(stream);
            else if constexpr(dispatch == Dispatch::Adl)
                Serialize(stream, obj);
            else if constexpr(dispatch == Dispatch::Default)
                nc::serialize::binary::Serialize(stream, obj);
            else
                static_assert(g_alwaysFalse<T>, "Unreachable");
        }
};

// Satisfied for types that have a Deserialize member function.
template <class T>
concept HasDeserializeMember = requires(std::istream& stream, T& obj)
{
    { obj.Deserialize(stream) } -> std::same_as<void>;
};

// Satisfied for types that have a Deserialize function in their namespace.
template <class T>
concept HasDeserializeAdl = requires(std::istream& stream, T& obj)
{
    { Deserialize(stream, obj) } -> std::same_as<void>; // intentional ADL
};

// Satisfied for types that have a compatible Deserialize function internally.
template <class T>
concept HasDeserializeDefault = requires(std::istream& stream, T& obj)
{
    { nc::serialize::binary::Deserialize(stream, obj) } -> std::same_as<void>;
};

// CPO for nc::serialize::Deserialize - dispatches to a `Deserialize()` function that is either
// a member of T, non-member found via adl, or internal non- member depending on what is
// available. Resolution is attempted in that order.
struct DeserializeFn
{
    private:
        template<class T>
        static consteval auto GetDispatch() -> Dispatch
        {
            if constexpr(HasDeserializeMember<T>)
                return Dispatch::Member;
            else if constexpr(HasDeserializeAdl<T>)
                return Dispatch::Adl;
            else if constexpr(HasDeserializeDefault<T>)
                return Dispatch::Default;
            else
                return Dispatch::None;
        }

        template<class T>
        static constexpr auto Strategy = GetDispatch<T>();

    public:
        template<class T>
            requires (Strategy<T> != Dispatch::None)
        auto operator()(std::istream& stream, T& obj) const
        {
            constexpr auto dispatch = Strategy<T>;
            if constexpr(dispatch == Dispatch::Member)
                obj.Deserialize(stream);
            else if constexpr(dispatch == Dispatch::Adl)
                Deserialize(stream, obj);
            else if constexpr(dispatch == Dispatch::Default)
                nc::serialize::binary::Deserialize(stream, obj);
            else
                static_assert(g_alwaysFalse<T>, "Unreachable");
        }
};
} // namespace nc::serialize::cpo
/** @endcond internal */
