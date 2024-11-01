#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "graphics2/frontend/subsystem/ObjectSubsystem.h"
#include "graphics2/frontend/subsystem/ObjectRenderState.h"

#include <ranges>

DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);
DEFINE_ASSET_SERVICE_STUB(textureAssetManager, nc::asset::AssetType::Texture, nc::asset::TextureView, std::string);

class ObjectSubsystemTest : public testing::Test,
                            public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;

        nc::Signal<nc::graphics::ToonRenderer&> onAddToonRenderer;
        nc::Signal<nc::Entity> onRemoveToonRenderer;

        nc::graphics::ToonMaterial dummyMaterial;

        nc::graphics::ObjectSubsystem uutNoStaticSort; // No Static Sorting Unit Under Test
        nc::graphics::ObjectSubsystem uutStaticSort;   // Static Sorting Unit Under Test

        ObjectSubsystemTest()
            : EcsFixture{MaxEntities},
              onAddToonRenderer{},
              onRemoveToonRenderer{},
              dummyMaterial{"base", 2, "hatch", 2},
              uutNoStaticSort{},
              uutStaticSort{onAddToonRenderer, onRemoveToonRenderer}
        {
        }
};


TEST_F(ObjectSubsystemTest, BuildState_NoStaticSorting_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        const auto entity = world.Emplace<nc::Entity>({});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    const auto actualState = uutNoStaticSort.BuildState(world);


    // const auto& actualViewProjection = actualState.viewProjection;

    // // Should give back something other than an identity matrix
    // EXPECT_FALSE(
    //     DirectX::XMVector4Equal(DirectX::g_XMIdentityR0, actualViewProjection.r[0]) &&
    //     DirectX::XMVector4Equal(DirectX::g_XMIdentityR1, actualViewProjection.r[1]) &&
    //     DirectX::XMVector4Equal(DirectX::g_XMIdentityR2, actualViewProjection.r[2]) &&
    //     DirectX::XMVector4Equal(DirectX::g_XMIdentityR3, actualViewProjection.r[3])
    // );

    // // Don't care about exact values, but matrix should project in a reasonable way
    // const auto nearPoint = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 1.0f);
    // const auto farPoint = DirectX::XMVectorSet(0.0f, 0.0f, 50.0f, 1.0f);
    // const auto projectedNearPoint = DirectX::XMVector4Transform(nearPoint, actualViewProjection);
    // const auto projectedFarPoint = DirectX::XMVector4Transform(farPoint, actualViewProjection);
    // EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetX(projectedNearPoint));
    // EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetX(projectedFarPoint));
    // EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetY(projectedNearPoint));
    // EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetY(projectedFarPoint));
    // EXPECT_LT(DirectX::XMVectorGetZ(projectedNearPoint), DirectX::XMVectorGetZ(projectedFarPoint));
}
