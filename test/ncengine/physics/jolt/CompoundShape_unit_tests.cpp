#include "JoltApiFixture.inl"
#include "ncengine/physics/CompoundShape.h"
#include "physics/jolt/Conversion.h"
#include "physics/jolt/CookedShapeUtility.h"
#include "physics/jolt/ShapeFactory.h"

#include "ncjolt/JoltApi.h"

#include "Jolt/Physics/Collision/Shape/MutableCompoundShape.h"
#include "Jolt/Physics/Collision/Shape/StaticCompoundShape.h"

class CompoundShapeTest : public JoltApiFixture
{
    public:
        nc::jolt::JoltApi joltApi;
        nc::Signal<const nc::asset::ConvexHullUpdateEventData&> convexHullSignal;
        nc::Signal<const nc::asset::MeshColliderUpdateEventData&> meshColliderSignal;
        nc::physics::ShapeFactory shapeFactory{convexHullSignal, meshColliderSignal};
};

TEST_F(CompoundShapeTest, CreateMutableCompoundShape_constructsSubShapes)
{
    auto subshapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f}),
            nc::Vector3{0.0f, 1.0f, 0.0f},
            nc::Quaternion::FromEulerAngles(1.57f, 0.0f, 0.0f),
            42
        },
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(5.0f),
            nc::Vector3{10.0f, 0.0f, 0.0f},
            nc::Quaternion::Identity(),
            142
        }
    };

    const auto cooked = nc::CreateMutableCompoundShape(subshapes);
    ASSERT_TRUE(cooked.HasShape());
    const auto properties = cooked.GetProperties();
    EXPECT_EQ(nc::ShapeType::Compound, properties.type);
    EXPECT_EQ(nc::ShapeDecorationFlags::None, properties.decorations);

    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(cooked.GetShapeData());
    ASSERT_EQ(JPH::EShapeType::Compound, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::MutableCompound, actualShape->GetSubType());
    const auto* mutableShape = static_cast<const JPH::MutableCompoundShape*>(actualShape.GetPtr());

    ASSERT_EQ(2, mutableShape->GetNumSubShapes());
    const auto& basePosition = mutableShape->GetCenterOfMass();

    const auto& expectedSubShape0 = subshapes.at(0);
    const auto& actualSubShape0 = mutableShape->GetSubShape(0);
    EXPECT_EQ(JPH::EShapeSubType::Box, actualSubShape0.mShape->GetSubType());
    EXPECT_EQ(expectedSubShape0.userData, actualSubShape0.mUserData);
    EXPECT_EQ(expectedSubShape0.position, nc::physics::ToVector3(actualSubShape0.GetPositionCOM() + basePosition));
    EXPECT_EQ(expectedSubShape0.rotation, nc::physics::ToQuaternion(actualSubShape0.GetRotation()));

    const auto& expectedSubShape1 = subshapes.at(1);
    const auto& actualSubShape1 = mutableShape->GetSubShape(1);
    EXPECT_EQ(JPH::EShapeSubType::Sphere, actualSubShape1.mShape->GetSubType());
    EXPECT_EQ(expectedSubShape1.userData, actualSubShape1.mUserData);
    EXPECT_EQ(expectedSubShape1.position, nc::physics::ToVector3(actualSubShape1.GetPositionCOM() + basePosition));
    EXPECT_EQ(expectedSubShape1.rotation, nc::physics::ToQuaternion(actualSubShape1.GetRotation()));
}

TEST_F(CompoundShapeTest, CreateStaticCompoundShape_constructsSubShapes)
{
    const auto subshapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f}),
            nc::Vector3{0.0f, 1.0f, 0.0f},
            nc::Quaternion::FromEulerAngles(1.57f, 0.0f, 0.0f),
            42
        },
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(5.0f),
            nc::Vector3{10.0f, 0.0f, 0.0f},
            nc::Quaternion::Identity(),
            142
        }
    };

    const auto cooked = nc::CreateStaticCompoundShape(subshapes);
    ASSERT_TRUE(cooked.HasShape());
    const auto properties = cooked.GetProperties();
    EXPECT_EQ(nc::ShapeType::Compound, properties.type);
    EXPECT_EQ(nc::ShapeDecorationFlags::IsStaticCompound, properties.decorations);

    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(cooked.GetShapeData());
    ASSERT_EQ(JPH::EShapeType::Compound, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::StaticCompound, actualShape->GetSubType());
    const auto* staticShape = static_cast<const JPH::StaticCompoundShape*>(actualShape.GetPtr());

    ASSERT_EQ(2, staticShape->GetNumSubShapes());
    const auto& basePosition = staticShape->GetCenterOfMass();

    const auto& expectedSubShape0 = subshapes.at(0);
    const auto& actualSubShape0 = staticShape->GetSubShape(0);
    EXPECT_EQ(JPH::EShapeSubType::Box, actualSubShape0.mShape->GetSubType());
    EXPECT_EQ(expectedSubShape0.userData, actualSubShape0.mUserData);
    EXPECT_EQ(expectedSubShape0.position, nc::physics::ToVector3(actualSubShape0.GetPositionCOM() + basePosition));
    EXPECT_EQ(expectedSubShape0.rotation, nc::physics::ToQuaternion(actualSubShape0.GetRotation()));

    const auto& expectedSubShape1 = subshapes.at(1);
    const auto& actualSubShape1 = staticShape->GetSubShape(1);
    EXPECT_EQ(JPH::EShapeSubType::Sphere, actualSubShape1.mShape->GetSubType());
    EXPECT_EQ(expectedSubShape1.userData, actualSubShape1.mUserData);
    EXPECT_EQ(expectedSubShape1.position, nc::physics::ToVector3(actualSubShape1.GetPositionCOM() + basePosition));
    EXPECT_EQ(expectedSubShape1.rotation, nc::physics::ToQuaternion(actualSubShape1.GetRotation()));
}

TEST_F(CompoundShapeTest, CreateCompoundShape_tooFewShapes_throws)
{
    const auto subshapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(5.0f),
        }
    };

    EXPECT_THROW(nc::CreateMutableCompoundShape(subshapes), nc::NcError);
    EXPECT_THROW(nc::CreateStaticCompoundShape(subshapes), nc::NcError);
}

TEST_F(CompoundShapeTest, CompoundShapeBuilder_GetSubShapeIndex_returnsIndex)
{
    const auto subshapes = std::array{
        nc::SubShapeInfo{
            .shape = nc::Shape::MakeBox(),
            .userData = 42
        },
        nc::SubShapeInfo{
            .shape = nc::Shape::MakeSphere(5.0f),
            .userData = 142
        }
    };

    auto cooked = nc::CreateMutableCompoundShape(subshapes);
    auto uut = nc::CompoundShapeBuilder{cooked};
    EXPECT_EQ(0, uut.GetSubShapeIndex(subshapes.at(0).userData));
    EXPECT_EQ(1, uut.GetSubShapeIndex(subshapes.at(1).userData));
}

TEST_F(CompoundShapeTest, CompoundShapeBuilder_SampleWorkflow)
{
    const auto subshapes = std::array{
        nc::SubShapeInfo{
            nc::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f}),
            nc::Vector3{0.0f, 1.0f, 0.0f},
            nc::Quaternion::FromEulerAngles(1.57f, 0.0f, 0.0f),
            42
        },
        nc::SubShapeInfo{
            nc::Shape::MakeSphere(5.0f),
            nc::Vector3{10.0f, 0.0f, 0.0f},
            nc::Quaternion::Identity(),
            142
        }
    };

    auto cooked = nc::CreateMutableCompoundShape(subshapes);
    auto uut = nc::CompoundShapeBuilder{cooked};
    const auto& actualShape = nc::ShapeStorageRTTI::ToShape(cooked.GetShapeData());
    ASSERT_EQ(JPH::EShapeType::Compound, actualShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::MutableCompound, actualShape->GetSubType());
    const auto* mutableShape = static_cast<const JPH::MutableCompoundShape*>(actualShape.GetPtr());

    EXPECT_EQ(subshapes.at(0).position, uut.GetSubShapePosition(0));
    EXPECT_EQ(subshapes.at(0).rotation, uut.GetSubShapeRotation(0));
    EXPECT_EQ(subshapes.at(1).position, uut.GetSubShapePosition(1));
    EXPECT_EQ(subshapes.at(1).rotation, uut.GetSubShapeRotation(1));

    const auto newCapsule = nc::SubShapeInfo{
        nc::Shape::MakeCapsule(1.0f, 0.5f),
        nc::Vector3{100.0f, 100.0f, 0.0f},
        nc::Quaternion::FromEulerAngles(1.57f, 0.0f, 0.0f),
        200
    };

    const auto newCapsuleIndex = uut.AddSubShape(newCapsule);
    EXPECT_EQ(3, mutableShape->GetNumSubShapes());
    const auto& newCapsuleSubShape = mutableShape->GetSubShape(newCapsuleIndex);
    ASSERT_EQ(JPH::EShapeSubType::Capsule, newCapsuleSubShape.mShape->GetSubType());
    EXPECT_EQ(newCapsule.position, uut.GetSubShapePosition(newCapsuleIndex));
    EXPECT_EQ(newCapsule.rotation, uut.GetSubShapeRotation(newCapsuleIndex));

    uut.RemoveSubShape(0);
    EXPECT_EQ(2, mutableShape->GetNumSubShapes());
    const auto& firstSubShape = mutableShape->GetSubShape(0);
    const auto& secondSubShape = mutableShape->GetSubShape(1);
    ASSERT_EQ(JPH::EShapeSubType::Sphere, firstSubShape.mShape->GetSubType());
    ASSERT_EQ(JPH::EShapeSubType::Capsule, secondSubShape.mShape->GetSubType());

    const auto& newBox = subshapes.at(0);
    uut.ReplaceSubShape(0, newBox);
    EXPECT_EQ(2, mutableShape->GetNumSubShapes());
    const auto& newBoxSubShape = mutableShape->GetSubShape(0);
    ASSERT_EQ(JPH::EShapeSubType::Box, newBoxSubShape.mShape->GetSubType());
    EXPECT_EQ(newBox.position, uut.GetSubShapePosition(0));
    EXPECT_EQ(newBox.rotation, uut.GetSubShapeRotation(0));

    const auto newPosition = nc::Vector3::Down();
    const auto newRotation = nc::Quaternion::FromEulerAngles(0.0f, 3.0f, 0.0f);
    uut.ModifySubShape(0, newPosition, newRotation);

    EXPECT_EQ(newPosition, uut.GetSubShapePosition(0));
    EXPECT_EQ(newRotation, uut.GetSubShapeRotation(0));

    const auto oldCom = mutableShape->GetCenterOfMass();
    uut.RecalculateCenterOfMass();
    const auto newCom = mutableShape->GetCenterOfMass();
    // Don't care exactly what COM is, but it should be different. Local transformations should not have changed.
    EXPECT_NE(nc::physics::ToVector3(oldCom), nc::physics::ToVector3(newCom));
    EXPECT_EQ(newPosition, uut.GetSubShapePosition(0));
    EXPECT_EQ(newRotation, uut.GetSubShapeRotation(0));
    EXPECT_EQ(newCapsule.position, uut.GetSubShapePosition(1));
    EXPECT_EQ(newCapsule.rotation, uut.GetSubShapeRotation(1));
}

// todo
// wrong type throws
// wrong id throws
