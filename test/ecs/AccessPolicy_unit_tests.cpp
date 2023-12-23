#include "gtest/gtest.h"
#include "ncengine/ecs/AccessPolicy.h"

struct S1 {};
struct S2 {};

using explicitPolicy = nc::ecs::AccessPolicy<nc::ecs::FilterBase::None, S1, S2>;
using basicPolicy = nc::ecs::AccessPolicy<nc::ecs::FilterBase::Basic, S1, S2>;
using allPolicy = nc::ecs::AccessPolicy<nc::ecs::FilterBase::All>;

TEST(AccessPolicyTests, ConvertibleTo_identity_isTrue)
{
    static_assert(explicitPolicy::ConvertibleTo<nc::ecs::FilterBase::None, S1, S2>);
    static_assert(basicPolicy::ConvertibleTo<nc::ecs::FilterBase::Basic, S1, S2>);
    static_assert(allPolicy::ConvertibleTo<nc::ecs::FilterBase::All>);
}

TEST(AccessPolicyTests, ConvertibleTo_moreRestrictiveBase_isTrue)
{
    static_assert(basicPolicy::ConvertibleTo<nc::ecs::FilterBase::None, S1, S2>);
    static_assert(allPolicy::ConvertibleTo<nc::ecs::FilterBase::Basic, S1>);
}

TEST(AccessPolicyTests, ConvertibleTo_lessRestrictiveBase_isFalse)
{
    static_assert(!explicitPolicy::ConvertibleTo<nc::ecs::FilterBase::Basic, S1, S2>);
    static_assert(!basicPolicy::ConvertibleTo<nc::ecs::FilterBase::All, S1, S2>); // tparams are redundant but allowed
}

TEST(AccessPolicyTests, ConvertibleTo_moreRestrictiveTypeList_isTrue)
{
    static_assert(explicitPolicy::ConvertibleTo<nc::ecs::FilterBase::None>);
    static_assert(basicPolicy::ConvertibleTo<nc::ecs::FilterBase::Basic>);
}

TEST(AccessPolicyTests, ConvertibleTo_lessRestrictiveTypeList_isFalse)
{
    static_assert(!explicitPolicy::ConvertibleTo<nc::ecs::FilterBase::None, S1, S2, int>);
    static_assert(!basicPolicy::ConvertibleTo<nc::ecs::FilterBase::Basic, S1, S2, int>);
}

TEST(AccessPolicyTests, HasAccess_allAllowed_isTrue)
{
    static_assert(explicitPolicy::HasAccess<S1, S2>);
    static_assert(basicPolicy::HasAccess<S1, S2, nc::Entity, nc::Transform, nc::Tag, nc::ecs::detail::FreeComponentGroup>);
    static_assert(allPolicy::HasAccess<S1, S2, nc::Entity, nc::Transform, nc::Tag, nc::ecs::detail::FreeComponentGroup, int>);
}

TEST(AccessPolicyTests, HasAccess_anyNotAllowed_isFalse)
{
    static_assert(!explicitPolicy::HasAccess<nc::Entity, nc::Transform, nc::Tag, nc::ecs::detail::FreeComponentGroup, int>);
    static_assert(!basicPolicy::HasAccess<int>);
}

TEST(AccessPolicyTests, BaseContains_sameBase_isTrue)
{
    static_assert(explicitPolicy::BaseContains<nc::ecs::FilterBase::None>);
    static_assert(basicPolicy::BaseContains<nc::ecs::FilterBase::Basic>);
    static_assert(allPolicy::BaseContains<nc::ecs::FilterBase::All>);
}

TEST(AccessPolicyTests, BaseContains_moreRestrictive_isTrue)
{
    static_assert(basicPolicy::BaseContains<nc::ecs::FilterBase::Basic>);
    static_assert(basicPolicy::BaseContains<nc::ecs::FilterBase::None>);
    static_assert(allPolicy::BaseContains<nc::ecs::FilterBase::All>);
    static_assert(allPolicy::BaseContains<nc::ecs::FilterBase::Basic>);
    static_assert(allPolicy::BaseContains<nc::ecs::FilterBase::None>);
}

TEST(AccessPolicyTests, BaseContains_lessRestrictive_isFalse)
{
    static_assert(!explicitPolicy::BaseContains<nc::ecs::FilterBase::Basic>);
    static_assert(!explicitPolicy::BaseContains<nc::ecs::FilterBase::All>);
    static_assert(!basicPolicy::BaseContains<nc::ecs::FilterBase::All>);
}

TEST(AccessPolicyTests, OnPool_returnsReference_preservesValueCategory)
{
    using op = decltype(
        [](auto& pool) -> decltype(auto) { return pool.Handler(); }
    );

    using returned = decltype(
        std::declval<basicPolicy&>().OnPool<S1>(op{})
    );

    static_assert(std::is_lvalue_reference_v<returned>);
}

template<class T>
concept CanGetPools = requires (T t)
{
    t.GetComponentPools();
};

TEST(AccessPolicyTests, GetComponentPools_xxxxx)
{
    static_assert(CanGetPools<allPolicy>);
    static_assert(!CanGetPools<basicPolicy>);
    static_assert(!CanGetPools<explicitPolicy>);
}
