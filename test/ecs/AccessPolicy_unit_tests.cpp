#include "gtest/gtest.h"
#include "ncengine/ecs/AccessPolicy.h"

struct S1 {};
struct S2 {};

using explicitPolicy = nc::ecs::AccessPolicy<nc::ecs::FilterBase::None, S1, S2>;
using basicPolicy = nc::ecs::AccessPolicy<nc::ecs::FilterBase::Basic, S1, S2>;
using allPolicy = nc::ecs::AccessPolicy<nc::ecs::FilterBase::All>;

TEST(AccessPolicyTests, PolicyConvertibleTo_identity_isTrue)
{
    static_assert(nc::ecs::PolicyConvertibleTo<explicitPolicy, nc::ecs::FilterBase::None, S1, S2>);
    static_assert(nc::ecs::PolicyConvertibleTo<basicPolicy, nc::ecs::FilterBase::Basic, S1, S2>);
    static_assert(nc::ecs::PolicyConvertibleTo<allPolicy, nc::ecs::FilterBase::All>);
}

TEST(AccessPolicyTests, PolicyConvertibleTo_moreRestrictiveBase_isTrue)
{
    static_assert(nc::ecs::PolicyConvertibleTo<basicPolicy, nc::ecs::FilterBase::None, S1, S2>);
    static_assert(nc::ecs::PolicyConvertibleTo<allPolicy, nc::ecs::FilterBase::Basic, S1>);
}

TEST(AccessPolicyTests, PolicyConvertibleTo_lessRestrictiveBase_isFalse)
{
    static_assert(!nc::ecs::PolicyConvertibleTo<explicitPolicy, nc::ecs::FilterBase::Basic, S1, S2>);
    static_assert(!nc::ecs::PolicyConvertibleTo<basicPolicy, nc::ecs::FilterBase::All, S1, S2>); // tparams are redundant but allowed
}

TEST(AccessPolicyTests, PolicyConvertibleTo_moreRestrictiveTypeList_isTrue)
{
    static_assert(nc::ecs::PolicyConvertibleTo<explicitPolicy, nc::ecs::FilterBase::None>);
    static_assert(nc::ecs::PolicyConvertibleTo<basicPolicy, nc::ecs::FilterBase::Basic>);
}

TEST(AccessPolicyTests, PolicyConvertibleTo_lessRestrictiveTypeList_isFalse)
{
    static_assert(!nc::ecs::PolicyConvertibleTo<explicitPolicy, nc::ecs::FilterBase::None, S1, S2, int>);
    static_assert(!nc::ecs::PolicyConvertibleTo<basicPolicy, nc::ecs::FilterBase::Basic, S1, S2, int>);
}

TEST(AccessPolicyTests, HasAccess_allAllowed_isTrue)
{
    static_assert(nc::ecs::HasAccess<explicitPolicy, S1, S2>);
    static_assert(nc::ecs::HasAccess<basicPolicy, S1, S2, nc::Entity, nc::Transform, nc::Tag, nc::ecs::detail::FreeComponentGroup>);
    static_assert(nc::ecs::HasAccess<allPolicy, S1, S2, nc::Entity, nc::Transform, nc::Tag, nc::ecs::detail::FreeComponentGroup, int>);
}

TEST(AccessPolicyTests, HasAccess_anyNotAllowed_isFalse)
{
    static_assert(!nc::ecs::HasAccess<explicitPolicy, nc::Entity, nc::Transform, nc::Tag, nc::ecs::detail::FreeComponentGroup, int>);
    static_assert(!nc::ecs::HasAccess<basicPolicy, int>);
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
