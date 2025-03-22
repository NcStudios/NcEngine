#include "JoltApiFixture.inl"
#include "ncengine/physics/CompoundShape.h"
#include "ncengine/physics/CookedShape.h"
#include "ncengine/physics/SupportFunctions.h"
#include "physics/jolt/Conversion.h"
#include "physics/jolt/CookedShapeUtility.h"
#include "physics/jolt/ShapeFactory.h"

#include "ncjolt/JoltApi.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"

class SupportFunctionsTest : public JoltApiFixture
{
    public:
        nc::jolt::JoltApi joltApi;
        nc::Signal<const nc::asset::ConvexHullUpdateEventData&> convexHullSignal;
        nc::Signal<const nc::asset::MeshColliderUpdateEventData&> meshColliderSignal;
        nc::physics::ShapeFactory shapeFactory{convexHullSignal, meshColliderSignal};

        void CookAndAddRuntimeAsset(nc::asset::AssetId id, std::span<const nc::SubShapeInfo> subShapes)
        {
            auto cooked = nc::CreateStaticCompoundShape(subShapes);
            auto& apiShape = nc::ShapeStorageRTTI::ToShape(cooked.GetShapeData());
            shapeFactory.AddRuntimeAsset(std::move(apiShape), id);
        }
};

struct TestDirectionQuery
{
    static auto GetDistanceFromOrigin(const nc::CookedShape& shape) -> TestDirectionQuery
    {
        return TestDirectionQuery{
            nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()),
            nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()),
            nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()),
            nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()),
            nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()),
            nc::GetDistanceFromOrigin(shape, nc::Vector3::Back())
        };
    }

    static auto GetHalfExtent(const nc::CookedShape& shape) -> TestDirectionQuery
    {
        return TestDirectionQuery{
            nc::GetHalfExtent(shape, nc::Vector3::Right()),
            nc::GetHalfExtent(shape, nc::Vector3::Left()),
            nc::GetHalfExtent(shape, nc::Vector3::Up()),
            nc::GetHalfExtent(shape, nc::Vector3::Down()),
            nc::GetHalfExtent(shape, nc::Vector3::Front()),
            nc::GetHalfExtent(shape, nc::Vector3::Back())
        };
    }

    float right;
    float left;
    float up;
    float down;
    float front;
    float back;
};

////////
#define TEST_VECS(expected, direction) \
{ \
    const auto actual = uut.GetFurthestVertex(direction); \
    EXPECT_EQ(expected, actual) \
        << "expected: "  << expected.x << ", " << expected.y << ", " << expected.z \
        << " | actual: " << actual.x   << ", " << actual.y   << ", " << actual.z; \
}
/////////

TEST_F(SupportFunctionsTest, Sphere_nonTransformed)
{
    constexpr auto radius = 1.0f;
    const auto shape = nc::CookedShape{nc::Shape::MakeSphere(radius)};

    EXPECT_FLOAT_EQ(radius, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(radius, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(radius, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Back()));

    EXPECT_EQ(nc::Vector3::Right() * radius, nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius, nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * radius, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * radius, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius, nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius, nc::GetWorldSupport(shape, nc::Vector3::Back()));
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

    EXPECT_FLOAT_EQ(1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius, nc::GetHalfExtent(shape, nc::Vector3::Back()));

    EXPECT_EQ(nc::Vector3::Right() * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius + offset, nc::GetWorldSupport(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, Box_nonTransformed)
{
    constexpr auto extents = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto shape = nc::CookedShape{nc::Shape::MakeBox(extents)};

    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Back()));

    // for box, multiple vertices are valid here; just testting component of query direction
    EXPECT_FLOAT_EQ( 0.5f, nc::GetWorldSupport(shape, nc::Vector3::Right()).x);
    EXPECT_FLOAT_EQ(-0.5f, nc::GetWorldSupport(shape, nc::Vector3::Left()).x);
    EXPECT_FLOAT_EQ( 1.0f, nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(-1.0f, nc::GetWorldSupport(shape, nc::Vector3::Down()).y);
    EXPECT_FLOAT_EQ( 1.5f, nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(-1.5f, nc::GetWorldSupport(shape, nc::Vector3::Back()).z);
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

    EXPECT_FLOAT_EQ( 1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(-0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ( 3.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(-1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ( 4.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(-1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.0f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetHalfExtent(shape, nc::Vector3::Front()));

    // for box, multiple vertices are valid here; just testting component of query direction
    EXPECT_FLOAT_EQ( 0.5f + offset.x, nc::GetWorldSupport(shape, nc::Vector3::Right()).x);
    EXPECT_FLOAT_EQ(-0.5f + offset.x, nc::GetWorldSupport(shape, nc::Vector3::Left()).x);
    EXPECT_FLOAT_EQ( 1.0f + offset.y, nc::GetWorldSupport(shape, nc::Vector3::Up()).y);
    EXPECT_FLOAT_EQ(-1.0f + offset.y, nc::GetWorldSupport(shape, nc::Vector3::Down()).y);
    EXPECT_FLOAT_EQ( 1.5f + offset.z, nc::GetWorldSupport(shape, nc::Vector3::Front()).z);
    EXPECT_FLOAT_EQ(-1.5f + offset.z, nc::GetWorldSupport(shape, nc::Vector3::Back()).z);
}

TEST_F(SupportFunctionsTest, Capsule_nonTransformed)
{
    constexpr auto height = 2.0f;
    constexpr auto halfHeight = height / 2.0f;
    constexpr auto radius = 0.5f;
    const auto shape = nc::CookedShape{nc::Shape::MakeCapsule(2.0f, 0.5f)};

    EXPECT_FLOAT_EQ(radius,     nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius,     nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius,     nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius,     nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Back()));

    EXPECT_EQ(nc::Vector3::Right() * radius,     nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(nc::Vector3::Left()  * radius,     nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(nc::Vector3::Up()    * halfHeight, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(nc::Vector3::Down()  * halfHeight, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(nc::Vector3::Front() * radius,     nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(nc::Vector3::Back()  * radius,     nc::GetWorldSupport(shape, nc::Vector3::Back()));
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

    /// TODO: what are these values? function name is wrong
    EXPECT_FLOAT_EQ(-0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ( 1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ( 1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ( 1.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ( 0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ( 0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(halfHeight, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(radius,     nc::GetHalfExtent(shape, nc::Vector3::Back()));

    EXPECT_EQ(offset + nc::Vector3::Right() * radius,     nc::GetWorldSupport(shape, nc::Vector3::Right()));
    EXPECT_EQ(offset + nc::Vector3::Left()  * radius,     nc::GetWorldSupport(shape, nc::Vector3::Left()));
    EXPECT_EQ(offset + nc::Vector3::Up()    * halfHeight, nc::GetWorldSupport(shape, nc::Vector3::Up()));
    EXPECT_EQ(offset + nc::Vector3::Down()  * halfHeight, nc::GetWorldSupport(shape, nc::Vector3::Down()));
    EXPECT_EQ(offset + nc::Vector3::Front() * radius,     nc::GetWorldSupport(shape, nc::Vector3::Front()));
    EXPECT_EQ(offset + nc::Vector3::Back()  * radius,     nc::GetWorldSupport(shape, nc::Vector3::Back()));

}

TEST_F(SupportFunctionsTest, GetDistanceFromOrigin_CompoundShape)
{
    const auto subShapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(0.5f),
            nc::Vector3::Up(),
            nc::Quaternion::Identity()
        },
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(0.5f),
            nc::Vector3::Down(),
            nc::Quaternion::Identity()
        }
    };

    const auto shape = nc::CreateStaticCompoundShape(subShapes);

    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, GetDistanceFromOrigin_CompoundShape2)
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

    const auto shape = nc::CreateStaticCompoundShape(subShapes);

    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.5f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));

    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetHalfExtent(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.25f, nc::GetHalfExtent(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetHalfExtent(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetHalfExtent(shape, nc::Vector3::Back()));
}


TEST_F(SupportFunctionsTest, GetDistanceFromOrigin_CompoundShape_rotatedInnerShape)
{
    const auto r = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, nc::DegreesToRadians(90.0f));

    // THINK THIS IS THE SAME AS TEST BELOW
    
    const auto uut = nc::CookedShape{
        nc::Shape::MakeBox(nc::Vector3{0.5f, 1.0f, 4.0f}),
        nc::Vector3::Zero(),
        r
    };

    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(uut, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, nc::GetDistanceFromOrigin(uut, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.25f, nc::GetDistanceFromOrigin(uut, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.25f, nc::GetDistanceFromOrigin(uut, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetDistanceFromOrigin(uut, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, nc::GetDistanceFromOrigin(uut, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, GetDistanceFromOrigin_CompoundShape_rotatedCompoundShape)
{
    const auto rotation = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, nc::DegreesToRadians(90.0f));

    const auto shape = nc::CookedShape{
        nc::Shape::MakeBox(nc::Vector3{0.5f, 1.0f, 4.0f}),
        nc::Vector3::Zero(),
        rotation
    };

    EXPECT_FLOAT_EQ(0.5f,  nc::GetDistanceFromOrigin(shape, nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f,  nc::GetDistanceFromOrigin(shape, nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.25f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.25f, nc::GetDistanceFromOrigin(shape, nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f,  nc::GetDistanceFromOrigin(shape, nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f,  nc::GetDistanceFromOrigin(shape, nc::Vector3::Back()));
}

TEST_F(SupportFunctionsTest, GetDistanceFromOrigin_CompoundShape_translatedRotatedCompoundShape)
{
    const auto r2 = JPH::Quat::sEulerAngles(JPH::Vec3{0.0f, 0.0f, nc::DegreesToRadians(90.0f)});

    const auto r = nc::Quaternion{r2.GetX(), r2.GetY(), r2.GetZ(), r2.GetW()};

    const auto uut = nc::CookedShape{
        nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f}),
        nc::Vector3{1.0f, 2.0f, 3.0f},
        r
    };

    // after rotation:
    // 2.0, 1.0, 3.0

    const auto actualRight = nc::GetDistanceFromOrigin(uut, nc::Vector3::Right());
    const auto actualLeft = nc::GetDistanceFromOrigin(uut, nc::Vector3::Left());
    const auto actualUp = nc::GetDistanceFromOrigin(uut, nc::Vector3::Up());
    const auto actualDown = nc::GetDistanceFromOrigin(uut, nc::Vector3::Down());
    const auto actualFront = nc::GetDistanceFromOrigin(uut, nc::Vector3::Front());
    const auto actualBack = nc::GetDistanceFromOrigin(uut, nc::Vector3::Back());

    EXPECT_FLOAT_EQ(2.0f, actualRight);
    EXPECT_FLOAT_EQ(0.0f, actualLeft);
    EXPECT_FLOAT_EQ(2.5f, actualUp);
    EXPECT_FLOAT_EQ(-1.5f, actualDown); // these are busted, kind of...
    EXPECT_FLOAT_EQ(4.5f, actualFront);
    EXPECT_FLOAT_EQ(-1.5f, actualBack);
}

TEST_F(SupportFunctionsTest, DebugSandbox)
{
    /// !!! something weird here, DistanceFromorigin is a better result

    const auto r2 = JPH::Quat::sEulerAngles(JPH::Vec3{nc::DegreesToRadians(90.0f), 0.0f, 0.0f});
    const auto r = nc::Quaternion{r2.GetX(), r2.GetY(), r2.GetZ(), r2.GetW()};
    EXPECT_EQ(r.x, r2.GetX());
    EXPECT_EQ(r.y, r2.GetY());
    EXPECT_EQ(r.z, r2.GetZ());
    EXPECT_EQ(r.w, r2.GetW());
    // const auto r = nc::Quaternion::FromEulerAngles(nc::DegreesToRadians(90.0f), 0.0f, 0.0f);
    // const auto r = nc::Vector3::Zero();

    const auto subShapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{0.5f, 0.5f, 7.0f}),
        },
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{2.0f, 0.5f, 2.0f}),
            nc::Vector3{0.0f, 0.0f, 4.5f}
        }
    };

    constexpr auto id = nc::asset::AssetId{1};
    CookAndAddRuntimeAsset(id, subShapes);

    auto shape = nc::Shape::MakeCompound(id);
    auto uut = nc::CookedShape{shape, nc::Vector3::Zero(), r};

    // after rotation:
    // 2.0, 1.0, 3.0

    // const auto actualRight = uut.GetHalfExtent(nc::Vector3::Right());
    // const auto actualLeft  = uut.GetHalfExtent(nc::Vector3::Left());
    // const auto actualUp    = uut.GetHalfExtent(nc::Vector3::Up());
    // const auto actualDown  = uut.GetHalfExtent(nc::Vector3::Down());
    // const auto actualFront = uut.GetHalfExtent(nc::Vector3::Front());
    // const auto actualBack  = uut.GetHalfExtent(nc::Vector3::Back());

    const auto actualRight = nc::GetDistanceFromOrigin(uut, nc::Vector3::Right());
    const auto actualLeft  = nc::GetDistanceFromOrigin(uut, nc::Vector3::Left());
    const auto actualUp    = nc::GetDistanceFromOrigin(uut, nc::Vector3::Up());
    const auto actualDown  = nc::GetDistanceFromOrigin(uut, nc::Vector3::Down());
    const auto actualFront = nc::GetDistanceFromOrigin(uut, nc::Vector3::Front());
    const auto actualBack  = nc::GetDistanceFromOrigin(uut, nc::Vector3::Back());

    EXPECT_FLOAT_EQ(1.0f, actualRight);
    EXPECT_FLOAT_EQ(1.0f, actualLeft);
    EXPECT_FLOAT_EQ(3.5f, actualUp);
    EXPECT_FLOAT_EQ(5.5f, actualDown);
    EXPECT_NEAR(0.25f, actualFront, 0.000001f);
    EXPECT_NEAR(0.25f, actualBack, 0.000001f);

    // EXPECT_FLOAT_EQ(0.25f, actualFront);
    // EXPECT_FLOAT_EQ(0.25f, actualBack);
}
