#include "JoltApiFixture.inl"
#include "ncengine/physics/CompoundShape.h"
#include "ncengine/physics/CookedShape.h"
#include "ncengine/physics/SupportFunctions.h"
#include "physics/jolt/Conversion.h"
#include "physics/jolt/CookedShapeUtility.h"
#include "physics/jolt/ShapeFactory.h"

#include "ncjolt/JoltApi.h"

class SupportFunctionsTest : public JoltApiFixture
{
    public:
        nc::jolt::JoltApi joltApi;
        nc::Signal<const nc::asset::ConvexHullUpdateEventData&> convexHullSignal;
        nc::Signal<const nc::asset::MeshColliderUpdateEventData&> meshColliderSignal;
        nc::physics::ShapeFactory shapeFactory{convexHullSignal, meshColliderSignal};
};


TEST_F(SupportFunctionsTest, SupportBuffer_constructor_succeeds)
{
    auto actual = nc::SupportBuffer{};
    EXPECT_NO_THROW(actual.GetImpl());
}

TEST_F(SupportFunctionsTest, SupportBuffer_GetDefault_succeeds)
{
    EXPECT_NO_THROW(nc::SupportBuffer::GetDefault());
}

TEST_F(SupportFunctionsTest, Sphere_nonTransformed)
{
    constexpr auto radius = 1.0f;
    const auto shape = nc::CookedShape{nc::Shape::MakeSphere(radius)};

    EXPECT_EQ(nc::Vector3::Right() * radius, nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius, nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * radius, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * radius, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius, nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius, nc::GetWorldSupport(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, Sphere_transformed)
{
    constexpr auto radius = 1.0f;
    constexpr auto offset = nc::Vector3::Up();
    const auto shape = nc::CookedShape{
        nc::Shape::MakeSphere(radius),
        offset,
        nc::Quaternion::Identity()
    };

    EXPECT_EQ(nc::Vector3::Right() * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, Box_nonTransformed)
{
    constexpr auto extents = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto shape = nc::CookedShape{nc::Shape::MakeBox(extents)};

    // for box, multiple vertices are valid here; just testting component of query direction
    EXPECT_FLOAT_EQ( 0.5f, nc::GetWorldSupport(shape, nc::Vector3::Right()).x);
    EXPECT_FLOAT_EQ(-0.5f, nc::GetWorldSupport(shape, nc::Vector3::Left()).x);
    EXPECT_FLOAT_EQ( 1.0f, nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(-1.0f, nc::GetWorldSupport(shape, nc::Vector3::Down()).y);
    EXPECT_FLOAT_EQ( 1.5f, nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(-1.5f, nc::GetWorldSupport(shape, nc::Vector3::Back()).z);

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, Box_transformed)
{
    constexpr auto extents = nc::Vector3{1.0f, 2.0f, 3.0f};
    constexpr auto offset = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto shape = nc::CookedShape{
        nc::Shape::MakeBox(extents),
        offset,
        nc::Quaternion::Identity()
    };

    // for box, multiple vertices are valid here; just testting component of query direction
    EXPECT_FLOAT_EQ( 0.5f + offset.x, nc::GetWorldSupport(shape, nc::Vector3::Right()).x);
    EXPECT_FLOAT_EQ(-0.5f + offset.x, nc::GetWorldSupport(shape, nc::Vector3::Left()).x);
    EXPECT_FLOAT_EQ( 1.0f + offset.y, nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(-1.0f + offset.y, nc::GetWorldSupport(shape, nc::Vector3::Down()).y);
    EXPECT_FLOAT_EQ( 1.5f + offset.z, nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(-1.5f + offset.z, nc::GetWorldSupport(shape, nc::Vector3::Back()).z);

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
}

TEST_F(SupportFunctionsTest, Capsule_nonTransformed)
{
    constexpr auto height = 2.0f;
    constexpr auto halfHeight = height / 2.0f;
    constexpr auto radius = 0.5f;
    const auto shape = nc::CookedShape{nc::Shape::MakeCapsule(2.0f, 0.5f)};

    EXPECT_EQ(nc::Vector3::Right() * radius,     nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius,     nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * halfHeight, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * halfHeight, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius,     nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius,     nc::GetWorldSupport(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, Capsule_transformed)
{
    constexpr auto height = 2.0f;
    constexpr auto halfHeight = height / 2.0f;
    constexpr auto radius = 0.5f;
    constexpr auto offset = nc::Vector3::Left();
    const auto shape = nc::CookedShape{
        nc::Shape::MakeCapsule(height, radius),
        offset,
        nc::Quaternion::Identity()
    };

    EXPECT_EQ(nc::Vector3::Right() * radius     + offset, nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius     + offset, nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * halfHeight + offset, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * halfHeight + offset, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius     + offset, nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius     + offset, nc::GetWorldSupport(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, CompoundShape_nonTransformed)
{
    const auto subShapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(0.5f),
            nc::Vector3::Up(),
            nc::Quaternion::Identity()
        },
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(0.4f),
            nc::Vector3::Down(),
            nc::Quaternion::Identity()
        }
    };

    const auto shape = nc::CreateStaticCompoundShape(subShapes);

    EXPECT_EQ(nc::Vector3( 0.5f,  1.0f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3(-0.5f,  1.0f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3( 0.0f,  1.5f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3( 0.0f, -1.4f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3( 0.0f,  1.0f,  0.5f), nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3( 0.0f,  1.0f, -0.5f), nc::GetWorldSupport(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.4f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, CompoundShape_nonTransformed_offsetCenterOfMass)
{
    const auto subShapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{0.5f, 1.0f, 4.0f})
        },
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(0.5f),
            nc::Vector3::Down()
        }
    };

    auto shape = nc::CreateStaticCompoundShape(subShapes);

    EXPECT_EQ(nc::Vector3( 0.5f, -1.0f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3(-0.5f, -1.0f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3( 0.0f, -1.5f,  0.0f), nc::GetWorldSupport(shape, nc::Vector3::Down()));
    // vertices selected from box could be any on reference face
    EXPECT_FLOAT_EQ(0.5f,  nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(2.0f,  nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(-2.0f, nc::GetWorldSupport(shape, nc::Vector3::Back()).z);

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, CompoundShape_transformed)
{
    constexpr auto subshapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{0.5f, 1.0f, 4.0f})
        },
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{0.5f, 0.5f, 0.5f}),
            nc::Vector3::Down()
        }
    };

    const auto rotation = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, nc::DegreesToRadians(90.0f));
    auto shape = nc::CreateMutableCompoundShape(subshapes);
    shape.SetPositionAndRotation(nc::Vector3::Zero(), rotation);

    EXPECT_FLOAT_EQ( 1.25f, nc::GetWorldSupport(shape, nc::Vector3::Right()).x);
    EXPECT_FLOAT_EQ(-0.5f,  nc::GetWorldSupport(shape, nc::Vector3::Left()).x);
    EXPECT_FLOAT_EQ( 0.25f, nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(-0.25f, nc::GetWorldSupport(shape, nc::Vector3::Down()).y);
    EXPECT_FLOAT_EQ( 2.0f,  nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(-2.0f,  nc::GetWorldSupport(shape, nc::Vector3::Back()).z);

    EXPECT_FLOAT_EQ(1.25f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f,  nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.25f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.25f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f,  nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f,  nc::GetHalfExtent(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, GetDistanceFromOrigin_CompoundShape_translatedRotatedCompoundShape)
{
    constexpr auto extents = nc::Vector3{1.0f, 2.0f, 3.0f};
    constexpr auto offset = nc::Vector3{1.0f, 2.0f, 3.0f};

    // dx/jolt quats out of sync
    // There are slight differences in quaternion calculations between DX/Jolt. They diverge enough
    // in this test to exceed gtest's epsilon (error roughly 0.00000005). Forcing initialization from
    // Jolt's values to keep the comparison happy.
    const auto rotationSource = JPH::Quat::sEulerAngles(JPH::Vec3{0.0f, 0.0f, nc::DegreesToRadians(90.0f)});
    const auto rotation = nc::physics::ToQuaternion(rotationSource);
    const auto shape = nc::CookedShape{
        nc::Shape::MakeBox(extents),
        offset,
        rotation
    };

    EXPECT_FLOAT_EQ(2.0f, nc::GetWorldSupport(shape, nc::Vector3::Right()).x);
    EXPECT_FLOAT_EQ(0.0f, nc::GetWorldSupport(shape, nc::Vector3::Left()).x);
    EXPECT_FLOAT_EQ(2.5f, nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(1.5f, nc::GetWorldSupport(shape, nc::Vector3::Down()).y);
    EXPECT_FLOAT_EQ(4.5f, nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(1.5f, nc::GetWorldSupport(shape, nc::Vector3::Back()).z);

    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}
