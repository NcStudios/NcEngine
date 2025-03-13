#include "JoltApiFixture.inl"
#include "physics/jolt/ShapeFactory.h"
#include "physics/jolt/Conversion.h"
#include "physics/jolt/CookedShapeUtility.h"
#include "ncengine/asset/AssetData.h"
#include "ncengine/physics/Shape.h"
#include "ncengine/physics/CompoundShape.h"

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

const auto g_meshTriangles = std::vector<nc::Triangle>{
    nc::Triangle{nc::Vector3{0, 0, 0}, nc::Vector3{1, 0, 0}, nc::Vector3{0, 1, 0}},
    nc::Triangle{nc::Vector3{1, 1, 1}, nc::Vector3{2, 1, 1}, nc::Vector3{1, 2, 1}}
};

class ShapeFactoryTest : public JoltApiFixture
{
    public:
        nc::jolt::JoltApi joltApi;
        nc::Signal<const nc::asset::ConvexHullUpdateEventData&> convexHullSignal;
        nc::Signal<const nc::asset::MeshColliderUpdateEventData&> meshColliderSignal;
        nc::physics::ShapeFactory uut{convexHullSignal, meshColliderSignal};

        void LoadMockConvexHull()
        {
            using namespace nc::asset;
            const auto shape = nc::jolt::BuildConvexHull(g_hullVertices);
            const auto asset = ConvexHull{nc::Vector3{}, 0.0f, nc::jolt::SerializeShape(*shape)};
            convexHullSignal.Emit(ConvexHullUpdateEventData{std::span{&asset, 1}, std::span{&g_assetId, 1}, UpdateAction::Load});
        }

        void LoadMockMeshCollider()
        {
            using namespace nc::asset;
            const auto shape = nc::jolt::BuildMeshShape(g_meshTriangles);
            const auto asset = MeshCollider{nc::Vector3{}, 0.0f, nc::jolt::SerializeShape(*shape)};
            meshColliderSignal.Emit(MeshColliderUpdateEventData{std::span{&asset, 1}, std::span{&g_assetId, 1}, UpdateAction::Load});
        }

        void UnloadMockConvexHull()
        {
            using namespace nc::asset;
            convexHullSignal.Emit(ConvexHullUpdateEventData{{}, {&g_assetId, 1}, UpdateAction::Unload});
        }

        void UnloadMockMeshCollider()
        {
            using namespace nc::asset;
            meshColliderSignal.Emit(MeshColliderUpdateEventData{{}, {&g_assetId, 1}, UpdateAction::Unload});
        }

        void UnloadAllMockConvexHulls()
        {
            using namespace nc::asset;
            convexHullSignal.Emit(ConvexHullUpdateEventData{{}, {}, UpdateAction::UnloadAll});
        }

        void UnloadAllMockMeshColliders()
        {
            using namespace nc::asset;
            meshColliderSignal.Emit(MeshColliderUpdateEventData{{}, {}, UpdateAction::UnloadAll});
        }
};

TEST_F(ShapeFactoryTest, MakeDecoratedShape_offsetOverload_wrapsShape)
{
    const auto expectedExtents = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto expectedPosition = nc::Vector3{5.0f, 4.0f, 3.0f};
    const auto expectedRotation = nc::Quaternion::FromEulerAngles(0.0f, 5.0f, 1.0f);
    const auto inShape = nc::Shape::MakeBox(expectedExtents);
    const auto unwrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));
    const auto wrappedShape = uut.MakeDecoratedShape(
        unwrappedShape,
        nc::physics::ToJoltVec3(expectedPosition),
        nc::physics::ToJoltQuaternion(expectedRotation)
    );

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto actualDecoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());
    const auto actualInnerShape = actualDecoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, actualInnerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, actualInnerShape->GetSubType());
    const auto actualBox = static_cast<const JPH::BoxShape*>(actualInnerShape);

    const auto actualExtents = nc::physics::ToVector3(actualBox->GetHalfExtent() * 2.0f);
    const auto actualPosition = nc::physics::ToVector3(actualDecoratedShape->GetPosition());
    const auto actualRotation = nc::physics::ToQuaternion(actualDecoratedShape->GetRotation());
    EXPECT_EQ(expectedExtents, actualExtents);
    EXPECT_EQ(expectedPosition, actualPosition);
    EXPECT_EQ(expectedRotation, actualRotation);
}

TEST_F(ShapeFactoryTest, MakeDecoratedShape_scaleOverload_wrapsShape)
{
    const auto expectedExtents = nc::Vector3{1.0f, 1.0f, 1.0f};
    const auto inShape = nc::Shape::MakeBox(expectedExtents);
    const auto unwrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));
    const auto expectedScale = nc::Vector3{5.0f, 4.0f, 3.0f};
    const auto wrappedShape = uut.MakeDecoratedShape(
        unwrappedShape,
        nc::physics::ToJoltVec3(expectedScale)
    );

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Scaled, wrappedShape->GetSubType());
    const auto actualDecoratedShape = static_cast<const JPH::ScaledShape*>(wrappedShape.GetPtr());
    const auto actualInnerShape = actualDecoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, actualInnerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, actualInnerShape->GetSubType());
    const auto actualBox = static_cast<const JPH::BoxShape*>(actualInnerShape);

    const auto actualExtents = nc::physics::ToVector3(actualBox->GetHalfExtent() * 2.0f);
    const auto actualScale = nc::physics::ToVector3(actualDecoratedShape->GetScale());
    EXPECT_EQ(expectedExtents, actualExtents);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_box_returnsBoxShape)
{
    const auto inShape = nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f});
    const auto actualShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));
    ASSERT_EQ(JPH::EShapeType::Convex, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, actualShape->GetSubType());
    const auto box = static_cast<const JPH::BoxShape*>(actualShape.GetPtr());

    const auto expectedScale = inShape.GetLocalScale();
    const auto actualScale = nc::physics::ToVector3(box->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_box_withTransformScaling_returnsBoxShape)
{
    const auto transformScale = nc::Vector3{2.0f, 2.0f, 2.0f};
    const auto inShape = nc::Shape::MakeBox(nc::Vector3{2.0f, 2.0f, 2.0f});
    const auto actualShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));
    ASSERT_EQ(JPH::EShapeType::Convex, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, actualShape->GetSubType());
    const auto box = static_cast<const JPH::BoxShape*>(actualShape.GetPtr());

    const auto expectedScale = nc::HadamardProduct(inShape.GetLocalScale(), transformScale);
    const auto actualScale = nc::physics::ToVector3(box->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_returnsSphereShape)
{
    const auto inShape = nc::Shape::MakeSphere(0.75);
    const auto actualShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));
    ASSERT_EQ(JPH::EShapeType::Convex, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, actualShape->GetSubType());
    const auto sphere = static_cast<const JPH::SphereShape*>(actualShape.GetPtr());

    const auto expectedRadius = inShape.GetLocalScale().x * 0.5f;
    const auto actualRadius = sphere->GetRadius();
    EXPECT_FLOAT_EQ(expectedRadius, actualRadius);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_withTransformScaling_returnsSphereShape)
{
    const auto transformScale = nc::Vector3{2.0f, 2.0f, 2.0f};
    const auto inShape = nc::Shape::MakeSphere(0.75);
    const auto actualShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));
    ASSERT_EQ(JPH::EShapeType::Convex, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, actualShape->GetSubType());
    const auto sphere = static_cast<const JPH::SphereShape*>(actualShape.GetPtr());

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

TEST_F(ShapeFactoryTest, MakeShape_mesh_returnsMeshShape)
{
    LoadMockMeshCollider();

    const auto inShape = nc::Shape::MakeMesh(g_assetId, nc::Vector3::One());
    const auto wrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Scaled, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::ScaledShape*>(wrappedShape.GetPtr());

    const auto& expectedScale = inShape.GetLocalScale();
    const auto actualScale = nc::physics::ToVector3(decoratedShape->GetScale());
    EXPECT_EQ(expectedScale, actualScale);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Mesh, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Mesh, innerShape->GetSubType());
    // intentionally not checking triangles because its involved
}

TEST_F(ShapeFactoryTest, MakeShape_mesh_withTransformScaling_returnsScaledMeshShape)
{
    LoadMockMeshCollider();

    constexpr auto shapeScale = nc::Vector3{3.0f, 4.0f, 5.0f};
    constexpr auto transformScale = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto inShape = nc::Shape::MakeMesh(g_assetId, shapeScale);
    const auto wrappedShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Scaled, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::ScaledShape*>(wrappedShape.GetPtr());

    const auto expectedScale = nc::HadamardProduct(shapeScale, transformScale);
    const auto actualScale = nc::physics::ToVector3(decoratedShape->GetScale());
    EXPECT_EQ(expectedScale, actualScale);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Mesh, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Mesh, innerShape->GetSubType());
}

TEST_F(ShapeFactoryTest, MakeShape_mesh_notLoaded_throws)
{
    const auto inShape = nc::Shape::MakeMesh(g_assetId);
    EXPECT_THROW(uut.MakeShape(inShape, JPH::Vec3{}), nc::NcError);

    LoadMockMeshCollider();
    UnloadMockMeshCollider();
    EXPECT_THROW(uut.MakeShape(inShape, JPH::Vec3{}), nc::NcError);

    LoadMockMeshCollider();
    UnloadAllMockMeshColliders();
    EXPECT_THROW(uut.MakeShape(inShape, JPH::Vec3{}), nc::NcError);
}

TEST_F(ShapeFactoryTest, AddRuntimeAsset_compoundShape_addsAsset)
{
    const auto id = nc::asset::AssetId{42};
    const auto shapes = std::array{
        nc::SubShapeInfo{ nc::Shape::MakeBox()    },
        nc::SubShapeInfo{ nc::Shape::MakeSphere() }
    };

    auto cooked = nc::CreateStaticCompoundShape(shapes);
    uut.AddRuntimeAsset(nc::ShapeStorageRTTI::ToShape(cooked.GetShapeData()), id);
    const auto actual = uut.GetRuntimeAsset(id, nc::ShapeType::Compound);
    EXPECT_EQ(JPH::EShapeSubType::StaticCompound, actual->GetSubType());
}

TEST_F(ShapeFactoryTest, GetRuntimeAsset_badInputs_throws)
{
    const auto id = nc::asset::AssetId{42};
    const auto shapes = std::array{
        nc::SubShapeInfo{ nc::Shape::MakeBox()    },
        nc::SubShapeInfo{ nc::Shape::MakeSphere() }
    };

    auto cooked = nc::CreateStaticCompoundShape(shapes);
    auto shape = nc::ShapeStorageRTTI::ToShape(cooked.GetShapeData());
    uut.AddRuntimeAsset(shape, id);
    EXPECT_THROW(uut.AddRuntimeAsset(shape, id), nc::NcError);
    EXPECT_THROW(uut.GetRuntimeAsset(id, nc::ShapeType::Mesh), nc::NcError);
    EXPECT_THROW(uut.GetRuntimeAsset(id + 1, nc::ShapeType::Compound), nc::NcError);
}
