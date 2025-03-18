#include "JoltApiFixture.inl"
#include "ncengine/physics/CompoundShape.h"
#include "ncengine/physics/CookedShape.h"
#include "physics/jolt/Conversion.h"
#include "physics/jolt/CookedShapeUtility.h"
#include "physics/jolt/ShapeFactory.h"

#include "ncjolt/JoltApi.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"

class CookedShapeTest : public JoltApiFixture
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

TEST_F(CookedShapeTest, CookedShapeRTTI)
{
    using uut = nc::ShapeStorageRTTI;
    const auto box = nc::Shape::MakeBox();
    const auto sphere = nc::Shape::MakeSphere();

    {
        auto storage = nc::CookedShape::ShapeStorage{};
        uut::Construct(storage, shapeFactory.MakeShape(box, JPH::Vec3::sOne()));

        auto& actual = uut::ToShape(storage);
        ASSERT_NE(nullptr, actual.GetPtr());
        EXPECT_EQ(JPH::EShapeSubType::Box, actual->GetSubType());

        uut::Destruct(storage);
    }

    {
        auto storageSrc = nc::CookedShape::ShapeStorage{};
        auto storageDst = nc::CookedShape::ShapeStorage{};
        uut::Construct(storageSrc, shapeFactory.MakeShape(box, JPH::Vec3::sOne()));
        uut::Construct(storageDst, shapeFactory.MakeShape(sphere, JPH::Vec3::sOne()));
        uut::Move(storageDst, std::move(storageSrc));

        const auto& actualSrc = uut::ToShape(storageSrc);
        const auto& actualDst = uut::ToShape(storageDst);
        ASSERT_EQ(nullptr, actualSrc.GetPtr());
        ASSERT_NE(nullptr, actualDst.GetPtr());
        EXPECT_EQ(JPH::EShapeSubType::Box, actualDst->GetSubType());

        uut::Destruct(storageSrc);
        uut::Destruct(storageDst);
    }
}

TEST_F(CookedShapeTest, ShapeConstructor_constructsShape)
{
    const auto expectedExtents = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto box = nc::Shape::MakeBox(expectedExtents);
    const auto uut = nc::CookedShape{box};
    ASSERT_TRUE(uut.HasShape());
    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(uut.GetShapeData());
    ASSERT_NE(nullptr, actualShape.GetPtr());

    ASSERT_EQ(JPH::EShapeSubType::Box, actualShape->GetSubType());
    const auto* actualBox = static_cast<const JPH::BoxShape*>(actualShape.GetPtr());
    const auto actualExtents = nc::physics::ToVector3(actualBox->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedExtents, actualExtents);
}

TEST_F(CookedShapeTest, TransformationConstructor_constructsShape)
{
    const auto expectedExtents = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto expectedPosition = nc::Vector3{5.0f, 6.0f, 7.0f};
    const auto expectedRotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f);
    const auto box = nc::Shape::MakeBox(expectedExtents);
    const auto uut = nc::CookedShape{box, expectedPosition, expectedRotation};
    ASSERT_TRUE(uut.HasShape());
    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(uut.GetShapeData());
    ASSERT_NE(nullptr, actualShape.GetPtr());

    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, actualShape->GetSubType());
    const auto* actualWrapped = static_cast<const JPH::RotatedTranslatedShape*>(actualShape.GetPtr());
    const auto actualPosition = nc::physics::ToVector3(actualWrapped->GetPosition());
    const auto actualRotation = nc::physics::ToQuaternion(actualWrapped->GetRotation());
    EXPECT_EQ(expectedPosition, actualPosition);
    EXPECT_EQ(expectedRotation, actualRotation);

    const auto* innerShape = actualWrapped->GetInnerShape();
    ASSERT_EQ(JPH::EShapeSubType::Box, innerShape->GetSubType());
    const auto* actualBox = static_cast<const JPH::BoxShape*>(innerShape);
    const auto actualExtents = nc::physics::ToVector3(actualBox->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedExtents, actualExtents);
}

TEST_F(CookedShapeTest, DataConstructor_constructsShape)
{
    const auto box = nc::Shape::MakeBox();
    const auto src = nc::CookedShape{box};
    const auto uut = nc::CookedShape{src.GetShapeData()};
    ASSERT_TRUE(src.HasShape());
    ASSERT_TRUE(uut.HasShape());
    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(uut.GetShapeData());
    ASSERT_NE(nullptr, actualShape.GetPtr());
    ASSERT_EQ(JPH::EShapeSubType::Box, actualShape->GetSubType());
}

TEST_F(CookedShapeTest, MoveOperations_transferState)
{
    const auto box = nc::Shape::MakeBox();

    {
        auto movedFrom = nc::CookedShape{box};
        auto movedTo = std::move(movedFrom);
        EXPECT_FALSE(movedFrom.HasShape());
        EXPECT_TRUE(movedTo.HasShape());
    }

    {
        auto movedFrom = nc::CookedShape{box};
        auto movedTo = nc::CookedShape(box);
        movedTo = std::move(movedFrom);
        EXPECT_FALSE(movedFrom.HasShape());
        EXPECT_TRUE(movedTo.HasShape());
    }
}

TEST_F(CookedShapeTest, GetProperties_findsInnerShapeAndDecorations)
{
    {
        const auto sphere = nc::Shape::MakeSphere(0.5);
        const auto uut = nc::CookedShape{sphere};
        const auto properties = uut.GetProperties();
        EXPECT_EQ(nc::ShapeType::Sphere, properties.type);
        EXPECT_EQ(nc::ShapeDecorationFlags::None, properties.decorations);
    }

    {
        const auto expectedExtents = nc::Vector3{1.0f, 2.0f, 3.0f};
        const auto expectedPosition = nc::Vector3{5.0f, 6.0f, 7.0f};
        const auto expectedRotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f);
        const auto box = nc::Shape::MakeBox(expectedExtents);
        const auto uut = nc::CookedShape{box, expectedPosition, expectedRotation};
        const auto properties = uut.GetProperties();
        EXPECT_EQ(nc::ShapeType::Box, properties.type);
        EXPECT_EQ(nc::ShapeDecorationFlags::HasIsometricTransformation, properties.decorations);
    }
}

TEST_F(CookedShapeTest, GetPositionAndRotation_transformedShape_accountsForTransformation)
{
    const auto expectedPosition = nc::Vector3{5.0f, 6.0f, 7.0f};
    const auto expectedRotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f);
    const auto box = nc::Shape::MakeBox();
    const auto uut = nc::CookedShape{box, expectedPosition, expectedRotation};
    const auto actualPosition = uut.GetPosition();
    const auto actualRotation = uut.GetRotation();
    EXPECT_EQ(expectedPosition, actualPosition);
    EXPECT_EQ(expectedRotation, actualRotation);
}

TEST_F(CookedShapeTest, SetPositionAndRotation_untransformedShape_decoratesShape)
{
    const auto expectedPosition = nc::Vector3{5.0f, 6.0f, 7.0f};
    const auto expectedRotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f);
    const auto box = nc::Shape::MakeBox();
    auto uut = nc::CookedShape{box};
    uut.SetPositionAndRotation(expectedPosition, expectedRotation);
    const auto actualPosition = uut.GetPosition();
    const auto actualRotation = uut.GetRotation();
    EXPECT_EQ(expectedPosition, actualPosition);
    EXPECT_EQ(expectedRotation, actualRotation);

    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(uut.GetShapeData());
    EXPECT_EQ(JPH::EShapeSubType::RotatedTranslated, actualShape->GetSubType());
    const auto* wrapped = static_cast<const JPH::RotatedTranslatedShape*>(actualShape.GetPtr());
    EXPECT_EQ(JPH::EShapeSubType::Box, wrapped->GetInnerShape()->GetSubType());
}

TEST_F(CookedShapeTest, SetPositionAndRotation_transformedShape_updatesValues)
{
    const auto originalPosition = nc::Vector3{0.0f, 1.0f, 0.0f};
    const auto originalRotation = nc::Quaternion::Identity();
    const auto expectedPosition = nc::Vector3{5.0f, 6.0f, 7.0f};
    const auto expectedRotation = nc::Quaternion::FromEulerAngles(1.0f, 2.0f, 3.0f);
    const auto box = nc::Shape::MakeBox();
    auto uut = nc::CookedShape{box, originalPosition, originalRotation};
    uut.SetPositionAndRotation(expectedPosition, expectedRotation);
    const auto actualPosition = uut.GetPosition();
    const auto actualRotation = uut.GetRotation();
    EXPECT_EQ(expectedPosition, actualPosition);
    EXPECT_EQ(expectedRotation, actualRotation);

    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(uut.GetShapeData());
    EXPECT_EQ(JPH::EShapeSubType::RotatedTranslated, actualShape->GetSubType());
    const auto* wrapped = static_cast<const JPH::RotatedTranslatedShape*>(actualShape.GetPtr());
    EXPECT_EQ(JPH::EShapeSubType::Box, wrapped->GetInnerShape()->GetSubType()); // make sure we didn't decorate twice
}


/////////

struct TestDirectionQuery
{
    static auto GetDistanceFromOrigin(const nc::CookedShape& shape) -> TestDirectionQuery
    {
        return TestDirectionQuery{
            shape.GetDistanceFromOrigin(nc::Vector3::Right()),
            shape.GetDistanceFromOrigin(nc::Vector3::Left()),
            shape.GetDistanceFromOrigin(nc::Vector3::Up()),
            shape.GetDistanceFromOrigin(nc::Vector3::Down()),
            shape.GetDistanceFromOrigin(nc::Vector3::Front()),
            shape.GetDistanceFromOrigin(nc::Vector3::Back())
        };
    }

    static auto GetHalfExtent(const nc::CookedShape& shape) -> TestDirectionQuery
    {
        return TestDirectionQuery{
            shape.GetHalfExtent(nc::Vector3::Right()),
            shape.GetHalfExtent(nc::Vector3::Left()),
            shape.GetHalfExtent(nc::Vector3::Up()),
            shape.GetHalfExtent(nc::Vector3::Down()),
            shape.GetHalfExtent(nc::Vector3::Front()),
            shape.GetHalfExtent(nc::Vector3::Back())
        };
    }

    float right;
    float left;
    float up;
    float down;
    float front;
    float back;
};


TEST_F(CookedShapeTest, GetFurthesetVertex_sphere)
{
    {
        auto shape = nc::Shape::MakeSphere(1.0f);
        auto uut = nc::CookedShape{shape};

        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));

        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Back()));
    }

    {
        constexpr auto offset = nc::Vector3::Up();
        auto shape = nc::Shape::MakeSphere(1.0f);
        auto uut = nc::CookedShape{shape, offset, nc::Quaternion::Identity()};

        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(0.0f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));

        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Back()));
    }
}

TEST_F(CookedShapeTest, GetFurthestVertex_box)
{
    {
        constexpr auto extents = nc::Vector3{1.0f, 2.0f, 3.0f};
        auto shape = nc::Shape::MakeBox(extents);
        auto uut = nc::CookedShape{shape};

        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));

        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetHalfExtent(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetHalfExtent(nc::Vector3::Back()));
    }

    {
        auto shape = nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f});
        auto uut = nc::CookedShape{shape, nc::Vector3{1.0f, 2.0f, 3.0f}, nc::Quaternion::Identity()};

        EXPECT_FLOAT_EQ( 1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(-0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ( 3.0f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(-1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ( 4.5f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(-1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));

        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetHalfExtent(nc::Vector3::Back()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetHalfExtent(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetHalfExtent(nc::Vector3::Back()));
    }
}

TEST_F(CookedShapeTest, GetFurthestVertex_capsule)
{
    {
        auto shape = nc::Shape::MakeCapsule(2.0f, 0.5f);
        auto uut = nc::CookedShape{shape};

        // const auto actualOne = uut.GetDistanceFromOrigin(nc::Normalize(nc::Vector3::One()));

        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));

        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetHalfExtent(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetHalfExtent(nc::Vector3::Back()));

        // EXPECT_FLOAT_EQ(0.8333f, actualOne);
    }

    {
        auto shape = nc::Shape::MakeCapsule(2.0f, 0.5f);
        auto uut = nc::CookedShape{shape, nc::Vector3::Left(), nc::Quaternion::Identity()};

        EXPECT_FLOAT_EQ(-0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
        EXPECT_FLOAT_EQ(1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
        EXPECT_FLOAT_EQ(1.0f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
        EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));
    }
}

TEST_F(CookedShapeTest, GetDistanceFromOrigin_CompoundShape)
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

    constexpr auto id = nc::asset::AssetId{1};
    CookAndAddRuntimeAsset(id, subShapes);

    auto shape = nc::Shape::MakeCompound(id);
    auto uut = nc::CookedShape{shape};

    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));
}

TEST_F(CookedShapeTest, GetDistanceFromOrigin_CompoundShape2)
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

    const auto uut = nc::CreateStaticCompoundShape(subShapes);

    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(1.5f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));
}


TEST_F(CookedShapeTest, GetDistanceFromOrigin_CompoundShape_rotatedInnerShape)
{
    const auto r = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, nc::DegreesToRadians(90.0f));

    // THINK THIS IS THE SAME AS TEST BELOW
    
    const auto uut = nc::CookedShape{
        nc::Shape::MakeBox(nc::Vector3{0.5f, 1.0f, 4.0f}),
        nc::Vector3::Zero(),
        r
    };

    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.25f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.25f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));
}

TEST_F(CookedShapeTest, GetDistanceFromOrigin_CompoundShape_rotatedCompoundShape)
{
    const auto r = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, nc::DegreesToRadians(90.0f));

    const auto uut = nc::CookedShape{
        nc::Shape::MakeBox(nc::Vector3{0.5f, 1.0f, 4.0f}),
        nc::Vector3::Zero(),
        r
    };

    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Right()));
    EXPECT_FLOAT_EQ(0.5f, uut.GetDistanceFromOrigin(nc::Vector3::Left()));
    EXPECT_FLOAT_EQ(0.25f, uut.GetDistanceFromOrigin(nc::Vector3::Up()));
    EXPECT_FLOAT_EQ(0.25f, uut.GetDistanceFromOrigin(nc::Vector3::Down()));
    EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Front()));
    EXPECT_FLOAT_EQ(2.0f, uut.GetDistanceFromOrigin(nc::Vector3::Back()));
}

TEST_F(CookedShapeTest, GetDistanceFromOrigin_CompoundShape_translatedRotatedCompoundShape)
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

    const auto actualRight = uut.GetDistanceFromOrigin(nc::Vector3::Right());
    const auto actualLeft = uut.GetDistanceFromOrigin(nc::Vector3::Left());
    const auto actualUp = uut.GetDistanceFromOrigin(nc::Vector3::Up());
    const auto actualDown = uut.GetDistanceFromOrigin(nc::Vector3::Down());
    const auto actualFront = uut.GetDistanceFromOrigin(nc::Vector3::Front());
    const auto actualBack = uut.GetDistanceFromOrigin(nc::Vector3::Back());

    EXPECT_FLOAT_EQ(2.0f, actualRight);
    EXPECT_FLOAT_EQ(0.0f, actualLeft);
    EXPECT_FLOAT_EQ(2.5f, actualUp);
    EXPECT_FLOAT_EQ(-1.5f, actualDown); // these are busted, kind of...
    EXPECT_FLOAT_EQ(4.5f, actualFront);
    EXPECT_FLOAT_EQ(-1.5f, actualBack);
}

TEST_F(CookedShapeTest, DebugSandbox)
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

    const auto actualRight = uut.GetDistanceFromOrigin(nc::Vector3::Right());
    const auto actualLeft  = uut.GetDistanceFromOrigin(nc::Vector3::Left());
    const auto actualUp    = uut.GetDistanceFromOrigin(nc::Vector3::Up());
    const auto actualDown  = uut.GetDistanceFromOrigin(nc::Vector3::Down());
    const auto actualFront = uut.GetDistanceFromOrigin(nc::Vector3::Front());
    const auto actualBack  = uut.GetDistanceFromOrigin(nc::Vector3::Back());

    EXPECT_FLOAT_EQ(1.0f, actualRight);
    EXPECT_FLOAT_EQ(1.0f, actualLeft);
    EXPECT_FLOAT_EQ(3.5f, actualUp);
    EXPECT_FLOAT_EQ(5.5f, actualDown);
    EXPECT_NEAR(0.25f, actualFront, 0.000001f);
    EXPECT_NEAR(0.25f, actualBack, 0.000001f);

    // EXPECT_FLOAT_EQ(0.25f, actualFront);
    // EXPECT_FLOAT_EQ(0.25f, actualBack);
}
