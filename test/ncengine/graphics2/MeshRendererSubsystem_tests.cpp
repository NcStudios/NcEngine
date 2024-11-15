#include "gtest/gtest.h"
#include "../EcsFixture.inl"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "graphics2/frontend/subsystem/MeshRendererSubsystem.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include <array>
#include <ranges>

constexpr auto g_materialPasses = std::array{
    nc::MaterialPass::Toon
};

const auto g_materialDesc = nc::MaterialDesc{
    .passes = nc::MaterialPass::Toon
};

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc&){}
MaterialInstance::~MaterialInstance() = default;
auto MaterialInstance::GetPasses()     const -> MaterialPasses            { return g_materialDesc.passes;     }
auto MaterialInstance::GetProperties() const -> const MaterialProperties& { return g_materialDesc.properties; }
} // namespace nc

class MeshRendererSubsystemTest : public testing::Test,
                                  public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;

        nc::graphics::MeshRendererSubsystem uut;

        void AddEntity(nc::ecs::Ecs& world) 
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::MeshRenderer2>(
                entity,
                nc::asset::MeshView{},
                g_materialDesc
            );
        }

        MeshRendererSubsystemTest()
            : EcsFixture{MaxEntities},
              uut{MaxEntities, g_materialPasses}
        {
            GetTestComponentRegistry().RegisterType<nc::MeshRenderer2>(MaxEntities);
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


    auto actualRenderState = uut.BuildState(world);
    EXPECT_EQ(actualRenderState.instanceData.instances.size(), 5);


    EXPECT_EQ(1, actualRenderState.passData.size());
    auto& actualToonState = actualRenderState.passData.at(0);
    EXPECT_EQ(5, actualToonState.targets.size());

    registry.Clear();

}
