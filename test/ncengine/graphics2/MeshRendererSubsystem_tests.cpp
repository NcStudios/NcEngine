#include "gtest/gtest.h"
#include "../EcsFixture.inl"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "graphics2/frontend/subsystem/MeshRendererSubsystem.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include <ranges>

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc&){}
MaterialInstance::~MaterialInstance() = default;
auto MaterialInstance::GetDesc() const -> const MaterialDesc&
{
    static MaterialDesc desc{};
    return desc;
}
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
                nc::MaterialDesc{}
            );
        }

        MeshRendererSubsystemTest()
            : EcsFixture{MaxEntities},
              uut{}
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

    // Curious why this deduces to a && ref?
    auto actualRenderState = uut.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);

    // todo: update this test to check for pass state
}
