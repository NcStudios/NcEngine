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
