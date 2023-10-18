#include "gtest/gtest.h"
#include "ncengine/physics/Collider.h"
#include "physics/ColliderUtility.h"

constexpr auto g_boxProperties = nc::physics::BoxProperties
{
    .center = nc::Vector3::One(),
    .extents = nc::Vector3::Splat(2.0f)
};

const auto g_box = nc::Box
{
    .center = g_boxProperties.center,
    .extents = g_boxProperties.extents,
    .maxExtent = nc::Magnitude(g_boxProperties.extents * 0.5f)
};

constexpr auto g_sphereProperties = nc::physics::SphereProperties
{
    .center = nc::Vector3::Splat(42.0f),
    .radius = 11.2f
};

const auto g_sphere = nc::Sphere
{
    .center = g_sphereProperties.center,
    .radius = g_sphereProperties.radius
};

constexpr auto g_capsuleProperties = nc::physics::CapsuleProperties
{
    .center = nc::Vector3::Zero(),
    .height = 3.0f,
    .radius = 0.5f
};

const auto g_capsule = nc::Capsule
{
    .pointA = nc::Vector3::Up(),
    .pointB = nc::Vector3::Down(),
    .radius = 0.5f,
    .maxExtent = 1.5f
};

const auto g_hullProperties = nc::physics::HullProperties
{
    .assetPath = ""
};

const auto g_hull = nc::ConvexHull
{
    .vertices = {},
    .extents = {},
    .maxExtent = 0.5f
};

TEST(ColliderUtilityTests, ToVolumeInfo_BoxProperties)
{
    const auto actual = nc::physics::ToVolumeInfo(g_boxProperties);
    EXPECT_EQ(nc::physics::ColliderType::Box, actual.type);
    EXPECT_EQ(g_boxProperties.center, actual.offset);
    EXPECT_EQ(g_boxProperties.extents, actual.scale);
}

TEST(ColliderUtilityTests, ToVolumeInfo_SphereProperties)
{
    const auto actual = nc::physics::ToVolumeInfo(g_sphereProperties);
    EXPECT_EQ(nc::physics::ColliderType::Sphere, actual.type);
    EXPECT_EQ(g_sphereProperties.center, actual.offset);
    EXPECT_EQ(g_sphereProperties.radius * 2.0f, actual.scale.x);
}

TEST(ColliderUtilityTests, ToVolumeInfo_CapsuleProperties)
{
    const auto actual = nc::physics::ToVolumeInfo(g_capsuleProperties);
    EXPECT_EQ(nc::physics::ColliderType::Capsule, actual.type);
    EXPECT_EQ(g_capsuleProperties.center, actual.offset);
    EXPECT_EQ(g_capsuleProperties.radius * 2.0f, actual.scale.x);
    EXPECT_EQ(g_capsuleProperties.height * 0.5f, actual.scale.y);
}

TEST(ColliderUtilityTests, ToVolumeInfo_HullProperties)
{
    const auto actual = nc::physics::ToVolumeInfo(g_hullProperties);
    EXPECT_EQ(nc::physics::ColliderType::Hull, actual.type);
    EXPECT_EQ(nc::Vector3::Zero(), actual.offset);
    EXPECT_EQ(nc::Vector3::One(), actual.scale);
}

TEST(ColliderUtilityTests, MakeBoundingVolume_Box)
{
    const auto volume = nc::physics::MakeBoundingVolume(g_boxProperties);
    const auto& actual = std::get<nc::Box>(volume);
    EXPECT_EQ(g_box.center, actual.center);
    EXPECT_EQ(g_box.extents, actual.extents);
}

TEST(ColliderUtilityTests, MakeBoundingVolume_Sphere)
{
    const auto volume = nc::physics::MakeBoundingVolume(g_sphereProperties);
    const auto& actual = std::get<nc::Sphere>(volume);
    EXPECT_EQ(g_sphere.center, actual.center);
    EXPECT_EQ(g_sphere.radius, actual.radius);
}

TEST(ColliderUtilityTests, MakeBoundingVolume_Capsule)
{
    const auto volume = nc::physics::MakeBoundingVolume(g_capsuleProperties);
    const auto& actual = std::get<nc::Capsule>(volume);
    EXPECT_EQ(g_capsule.pointA, actual.pointA);
    EXPECT_EQ(g_capsule.pointB, actual.pointB);
    EXPECT_EQ(g_capsule.radius, actual.radius);
    EXPECT_EQ(g_capsule.maxExtent, actual.maxExtent);
}

// No test as this requires an asset. We'd mostly be testing a mock any way.
// TEST(ColliderUtilityTests, MakeBoundingVolume_Hull)

TEST(ColliderUtilityTests, MakeBoundingSphere_Box)
{
    const auto actual = nc::physics::MakeBoundingSphere(g_box, nc::Vector3{}, 1.0f);
    EXPECT_EQ(g_box.center, actual.center);
    EXPECT_EQ(g_box.maxExtent, actual.radius);

}

TEST(ColliderUtilityTests, MakeBoundingSphere_Sphere)
{
    const auto actual = nc::physics::MakeBoundingSphere(g_sphere, nc::Vector3{}, 1.0f);
    EXPECT_EQ(g_sphere.center, actual.center);
    EXPECT_EQ(g_sphere.radius, actual.radius);
}

TEST(ColliderUtilityTests, MakeBoundingSphere_Capsule)
{
    const auto actual = nc::physics::MakeBoundingSphere(g_capsule, nc::Vector3{}, 1.0f);
    EXPECT_EQ(nc::Vector3::Zero(), actual.center);
    EXPECT_EQ(g_capsule.maxExtent, actual.radius);
}

TEST(ColliderUtilityTests, MakeBoundingSphere_Hull)
{
    const auto actual = nc::physics::MakeBoundingSphere(g_hull, nc::Vector3{}, 1.0f);
    EXPECT_EQ(nc::Vector3::Zero(), actual.center);
    EXPECT_EQ(g_hull.maxExtent, actual.radius);
}

TEST(ColliderTests, Constructor_sphere_setsProperties)
{
    const auto expectedCenter = nc::Vector3{1.1f, 42.2f, 3.2f};
    const auto expectedRadius = 70.0f;
    const auto props = nc::physics::SphereProperties{expectedCenter, expectedRadius};
    const auto uut = nc::physics::Collider{nc::Entity{}, props, false};
    const auto actualVolume = std::get<nc::Sphere>(uut.GetVolume());
    EXPECT_EQ(expectedCenter, actualVolume.center);
    EXPECT_EQ(expectedRadius, actualVolume.radius);
    EXPECT_FALSE(uut.IsTrigger());
    EXPECT_TRUE(uut.IsAwake());
    EXPECT_EQ(nc::physics::ColliderType::Sphere, uut.GetType());
    EXPECT_EQ("", uut.GetAssetPath());
}

TEST(ColliderTests, Constructor_box_setsProperties)
{
    const auto expectedCenter = nc::Vector3{1.1f, 42.2f, 3.2f};
    const auto expectedExtents = nc::Vector3{70.0f, 71.0f, 72.0f};
    const auto props = nc::physics::BoxProperties{expectedCenter, expectedExtents};
    const auto uut = nc::physics::Collider{nc::Entity{}, props, false};
    const auto actualVolume = std::get<nc::Box>(uut.GetVolume());
    EXPECT_EQ(expectedCenter, actualVolume.center);
    EXPECT_EQ(expectedExtents, actualVolume.extents);
    EXPECT_FALSE(uut.IsTrigger());
    EXPECT_TRUE(uut.IsAwake());
    EXPECT_EQ(nc::physics::ColliderType::Box, uut.GetType());
    EXPECT_EQ("", uut.GetAssetPath());
}

TEST(ColliderTests, Constructor_capsule_setsProperties)
{
    const auto expectedCenter = nc::Vector3{1.1f, 42.2f, 3.2f};
    const auto expectedHeight = 2.0f;
    const auto expectedRadius = 0.25f;
    const auto props = nc::physics::CapsuleProperties{expectedCenter, expectedHeight, expectedRadius};
    const auto uut = nc::physics::Collider{nc::Entity{}, props, false};
    const auto actualVolume = std::get<nc::Capsule>(uut.GetVolume());
    const auto pointOffset = nc::Vector3::Up() * ( (expectedHeight / 2.0f) - expectedRadius );
    const auto expectedA = expectedCenter + pointOffset;
    const auto expectedB = expectedCenter - pointOffset;
    EXPECT_EQ(expectedA, actualVolume.pointA);
    EXPECT_EQ(expectedB, actualVolume.pointB);
    EXPECT_FLOAT_EQ(expectedRadius, actualVolume.radius);
    EXPECT_FLOAT_EQ(expectedHeight / 2.0f, actualVolume.maxExtent);
    EXPECT_FALSE(uut.IsTrigger());
    EXPECT_TRUE(uut.IsAwake());
    EXPECT_EQ(nc::physics::ColliderType::Capsule, uut.GetType());
    EXPECT_EQ("", uut.GetAssetPath());
}

TEST(ColliderTests, Constructor_zeroScale_throws)
{
    const auto sphereProps = nc::physics::SphereProperties{nc::Vector3::Zero(), 0.0f};
    const auto boxProps = nc::physics::BoxProperties{nc::Vector3::Zero(), nc::Vector3::Up()};
    const auto capsuleProps = nc::physics::CapsuleProperties{nc::Vector3::Zero(), 1.0f, 0.0f};
    EXPECT_THROW(nc::physics::Collider(nc::Entity{}, sphereProps), nc::NcError);
    EXPECT_THROW(nc::physics::Collider(nc::Entity{}, boxProps), nc::NcError);
    EXPECT_THROW(nc::physics::Collider(nc::Entity{}, capsuleProps), nc::NcError);
}

TEST(ColliderTests, ColliderType_ToString_converts)
{
    EXPECT_EQ("Box", nc::physics::ToString(nc::physics::ColliderType::Box));
    EXPECT_EQ("Capsule", nc::physics::ToString(nc::physics::ColliderType::Capsule));
    EXPECT_EQ("Hull", nc::physics::ToString(nc::physics::ColliderType::Hull));
    EXPECT_EQ("Sphere", nc::physics::ToString(nc::physics::ColliderType::Sphere));
    EXPECT_THROW(nc::physics::ToString(static_cast<nc::physics::ColliderType>(100)), nc::NcError);
}

TEST(ColliderTests, ColliderType_FromString_converts)
{
    EXPECT_EQ(nc::physics::ColliderType::Box, nc::physics::FromString("Box"));
    EXPECT_EQ(nc::physics::ColliderType::Capsule, nc::physics::FromString("Capsule"));
    EXPECT_EQ(nc::physics::ColliderType::Hull, nc::physics::FromString("Hull"));
    EXPECT_EQ(nc::physics::ColliderType::Sphere, nc::physics::FromString("Sphere"));
    EXPECT_THROW(nc::physics::FromString("Throw"), nc::NcError);
}
