#include "JoltApiFixture.inl"
#include "physics2/jolt/BodyManager.h"
#include "physics2/jolt/ShapeFactory.h"

#include "physics2/jolt/ConstraintManager.h"
// todo, do we want to move this stub to its own file
namespace nc::physics
{
auto ConstraintManager::AddConstraint(const ConstraintInfo&,
                                      Entity,
                                      JPH::Body*,
                                      Entity,
                                      JPH::Body*) -> ConstraintId
{
    return 0;
}

void ConstraintManager::RemoveConstraint(ConstraintId)
{
}

void ConstraintManager::RemoveConstraints(Entity)
{
}

auto ConstraintManager::GetConstraints(Entity) const -> std::span<const ConstraintView>
{
    return {};
}
} // namespace nc::physics


class BodyManagerTest : public JoltApiFixture
{
    private:
        struct RegistryInitialzer
        {
            RegistryInitialzer(nc::ecs::ComponentRegistry& registry)
            {
                registry.RegisterType<nc::Transform>(10u);
            }
        };

    protected:
        nc::ecs::ComponentRegistry registry;
        nc::physics::ShapeFactory shapeFactory;
        nc::physics::ConstraintManager constraintManager;
        nc::physics::BodyManager uut;

        BodyManagerTest()
            : registry{10u},
              constraintManager{joltApi.physicsSystem, 10u},
              uut{
                  nc::ecs::Ecs{registry},
                  10u,
                  joltApi.physicsSystem,
                  shapeFactory,
                  constraintManager
            }
        {
            // registry.RegisterType<nc::Transform>(10u);
            // registry.RegisterType<
        }
};

// constexpr auto g_entity1 = nc::Entity{1, 0, 0};
// constexpr auto g_entity2 = nc::Entity{2, 0, 0};
// constexpr auto g_entity3 = nc::Entity{3, 0, 0};


// TEST_F(BodyManagerTest, foo)
// {
    
// }
