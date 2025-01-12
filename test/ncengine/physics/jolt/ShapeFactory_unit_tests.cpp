#include "JoltApiFixture.inl"
#include "physics/jolt/ShapeFactory.h"
#include "physics/jolt/Conversion.h"
#include "ncengine/asset/AssetData.h"
#include "ncengine/physics/Shape.h"

#include "ncjolt/JoltApi.h"
#include "ncjolt/ShapeUtility.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

constexpr auto g_assetId = nc::asset::AssetId{42ull};
constexpr auto g_hullVertices = std::array{
    nc::Vector3::Front(), nc::Vector3::Back(),
    nc::Vector3::Up(),    nc::Vector3::Down()
};

class ShapeFactoryTest : public JoltApiFixture
{
    public:
        nc::jolt::JoltApi joltApi;
        nc::Signal<const nc::asset::ConvexHullUpdateEventData&> convexHullSignal;
        nc::physics::ShapeFactory uut{convexHullSignal};

        void LoadMockConvexHull()
        {
            const auto shape = nc::jolt::BuildConvexHull(g_hullVertices);
            const auto asset = nc::asset::ConvexHull{
                .extents = nc::Vector3{},
                .maxExtent = 0.0f,
                .blob = nc::jolt::SerializeShape(*shape)
            };

            convexHullSignal.Emit(nc::asset::ConvexHullUpdateEventData{
                std::span{&asset, 1},
                std::span{&g_assetId, 1},
                nc::asset::UpdateAction::Load
            });
        }

        void UnloadMockConvexHull()
        {
            convexHullSignal.Emit(nc::asset::ConvexHullUpdateEventData{
                {},
                {&g_assetId, 1},
                nc::asset::UpdateAction::Unload
            });
        }

        void UnloadAllMockConvexHulls()
        {
            convexHullSignal.Emit(nc::asset::ConvexHullUpdateEventData{
                {},
                {},
                nc::asset::UpdateAction::UnloadAll
            });
        }
};

TEST_F(ShapeFactoryTest, MakeShape_box_returnsBoxShape)
{
    const auto inShape = nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f}, nc::Vector3::Zero());
    const auto wrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());
    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, innerShape->GetSubType());
    const auto box = static_cast<const JPH::BoxShape*>(innerShape);

    const auto expectedScale = inShape.GetLocalScale();
    const auto actualScale = nc::physics::ToVector3(box->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedScale, actualScale);

    const auto expectedPosition = inShape.GetLocalPosition();
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);
}

TEST_F(ShapeFactoryTest, MakeShape_box_withTransformScaling_returnsBoxShape)
{
    const auto transformScale = nc::Vector3{2.0f, 2.0f, 2.0f};
    const auto inShape = nc::Shape::MakeBox(nc::Vector3{2.0f, 2.0f, 2.0f}, nc::Vector3{0.0f, 1.0f, 0.0f});
    const auto wrappedShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());

    const auto expectedPosition = nc::HadamardProduct(inShape.GetLocalPosition(), transformScale);
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, innerShape->GetSubType());
    const auto box = static_cast<const JPH::BoxShape*>(innerShape);

    const auto expectedScale = nc::HadamardProduct(inShape.GetLocalScale(), transformScale);
    const auto actualScale = nc::physics::ToVector3(box->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_returnsSphereShape)
{
    const auto inShape = nc::Shape::MakeSphere(0.75, nc::Vector3::Zero());
    const auto wrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());

    const auto expectedPosition = inShape.GetLocalPosition();
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, innerShape->GetSubType());
    const auto sphere = static_cast<const JPH::SphereShape*>(innerShape);

    const auto expectedRadius = inShape.GetLocalScale().x * 0.5f;
    const auto actualRadius = sphere->GetRadius();
    EXPECT_FLOAT_EQ(expectedRadius, actualRadius);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_withTransformScaling_returnsSphereShape)
{
    const auto transformScale = nc::Vector3{2.0f, 2.0f, 2.0f};
    const auto inShape = nc::Shape::MakeSphere(0.75, nc::Vector3{1.0f, 2.0f, 3.0f});
    const auto wrappedShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());

    const auto expectedPosition = nc::HadamardProduct(inShape.GetLocalPosition(), transformScale);
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, innerShape->GetSubType());
    const auto sphere = static_cast<const JPH::SphereShape*>(innerShape);

    const auto expectedRadius = inShape.GetLocalScale().x * transformScale.x * 0.5f;
    const auto actualRadius = sphere->GetRadius();
    EXPECT_FLOAT_EQ(expectedRadius, actualRadius);
}

TEST_F(ShapeFactoryTest, MakeShape_convexHull_returnsHullShape)
{
    LoadMockConvexHull();

    const auto inShape = nc::Shape::MakeConvexHull(g_assetId, nc::Vector3::One());
    const auto wrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Scaled, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::ScaledShape*>(wrappedShape.GetPtr());

    const auto& expectedScale = inShape.GetLocalScale();
    const auto actualScale = nc::physics::ToVector3(decoratedShape->GetScale());
    EXPECT_EQ(expectedScale, actualScale);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::ConvexHull, innerShape->GetSubType());
    const auto actualHull = static_cast<const JPH::ConvexHullShape*>(innerShape);

    const auto actualVertexCount = actualHull->GetNumPoints();
    EXPECT_EQ(g_hullVertices.size(), actualVertexCount);
    for (auto i = 0u; i < actualVertexCount; ++i)
    {
        const auto vertex = actualHull->GetPoint(i);
        const auto pos = std::ranges::find(g_hullVertices, nc::physics::ToVector3(vertex));
        EXPECT_NE(pos, g_hullVertices.cend());
    }
}

TEST_F(ShapeFactoryTest, MakeShape_convexHull_withTransformScaling_returnsScaledHullShape)
{
    LoadMockConvexHull();

    constexpr auto shapeScale = nc::Vector3{3.0f, 4.0f, 5.0f};
    constexpr auto transformScale = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto inShape = nc::Shape::MakeConvexHull(g_assetId, shapeScale);
    const auto wrappedShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Scaled, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::ScaledShape*>(wrappedShape.GetPtr());

    const auto expectedScale = nc::HadamardProduct(shapeScale, transformScale);
    const auto actualScale = nc::physics::ToVector3(decoratedShape->GetScale());
    EXPECT_EQ(expectedScale, actualScale);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::ConvexHull, innerShape->GetSubType());
    const auto actualHull = static_cast<const JPH::ConvexHullShape*>(innerShape);

    const auto actualVertexCount = actualHull->GetNumPoints();
    EXPECT_EQ(g_hullVertices.size(), actualVertexCount);
    // Since scaling is applied on the decorated shape, vertex positions should match the input (are unscaled)
    for (auto i = 0u; i < actualVertexCount; ++i)
    {
        const auto vertex = actualHull->GetPoint(i);
        const auto pos = std::ranges::find(g_hullVertices, nc::physics::ToVector3(vertex));
        EXPECT_NE(pos, g_hullVertices.cend());
    }
}

TEST_F(ShapeFactoryTest, MakeShape_convexHull_notLoaded_throws)
{
    const auto inShape = nc::Shape::MakeConvexHull(g_assetId);
    EXPECT_THROW(uut.MakeShape(inShape, JPH::Vec3{}), nc::NcError);

    LoadMockConvexHull();
    UnloadMockConvexHull();
    EXPECT_THROW(uut.MakeShape(inShape, JPH::Vec3{}), nc::NcError);

    LoadMockConvexHull();
    UnloadAllMockConvexHulls();
    EXPECT_THROW(uut.MakeShape(inShape, JPH::Vec3{}), nc::NcError);
}
