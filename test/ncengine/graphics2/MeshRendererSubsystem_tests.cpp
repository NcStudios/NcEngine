#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "graphics2/frontend/subsystem/MeshRendererSubsystem.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include <ranges>

DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);
DEFINE_ASSET_SERVICE_STUB(textureAssetManager, nc::asset::AssetType::Texture, nc::asset::TextureView, std::string);

class MeshRendererSubsystemTest : public testing::Test,
                            public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;

        nc::graphics::ToonMaterial dummyMaterial;
        nc::graphics::MeshRendererSubsystem uut;

        void AddEntity(nc::ecs::Ecs& world) 
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::graphics::ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        }

        MeshRendererSubsystemTest()
            : EcsFixture{MaxEntities},
              dummyMaterial{"base", 2, "hatch", 2},
              uut{}
        {
        }
};

TEST_F(MeshRendererSubsystemTest, BuildState_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    // Curious why this deduces to a && ref?
    auto actualRenderState = uut.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}
