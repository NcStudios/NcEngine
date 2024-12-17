#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/animation/AnimationStateOrchestrator.h"
#include "graphics2/frontend/subsystem/animation/SkeletalAnimationStorage.h"
#include "ncutility/Hash.h"

auto g_nextTransition = nc::AnimationTransition{};
auto g_completedCount = 0u;

const auto g_testMeshPath = std::string{"mesh.nca"};
const auto g_testMeshId = nc::utility::Fnv1a(g_testMeshPath);
const auto g_testBonesData = nc::asset::BonesData{};

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc&) {}
void MaterialInstance::Release() noexcept {}

MeshBase::MeshBase(Entity self,
                   const asset::MeshView& mesh,
                   const MaterialDesc&,
                   MeshInstanceType type)
    : m_ctx{self, mesh.id, 0, 0, type}
{
}

void MeshBase::Release() noexcept {}
SkeletalAnimationController::SkeletalAnimationController(uint64_t, float) : m_states{0} {}

auto SkeletalAnimationController::CheckForTransition() -> AnimationTransition
{
    return g_nextTransition;
}

void SkeletalAnimationController::NotifyCompleteState()
{
    ++g_completedCount;
}

namespace graphics
{
void SkeletalAnimationStorage::LoadBones(std::span<const std::string> ids,
                                         std::span<const asset::BonesData> data)
{
    for (const auto [id, bones] : std::views::zip(ids, data))
    {
        m_rigs.emplace(utility::Fnv1a(id), Rig{bones});
    }
}
} // namespace graphics
} // namespace nc

class AnimationStateOrchestratorTest : public testing::Test
{
    protected:
        nc::ecs::ComponentPool<nc::SkinnedMesh> pool{10ull, nc::ComponentHandler<nc::SkinnedMesh>{}};
        nc::graphics::SkeletalAnimationStorage storage{};
        nc::graphics::AnimationStateOrchestrator uut{};

        AnimationStateOrchestratorTest()
        {
            g_nextTransition = nc::AnimationTransition{};
            g_completedCount = 0;
            storage.LoadBones(
                std::span{&g_testMeshPath, 1},
                std::span{&g_testBonesData, 1}
            );
        }
};

TEST_F(AnimationStateOrchestratorTest, Transition_notAnimatable_doesNotAddState)
{
    const auto e1 = nc::Entity{1, 0, 0};
    pool.Emplace(e1, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.CommitStagedComponents({});

    // Transition to null animation does not create state
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = nc::NullAnimationId,
        .type = nc::AnimationTransitionType::Loop
    };

    uut.Transition(pool, storage);
    EXPECT_TRUE(uut.GetEntities().empty());
    EXPECT_TRUE(uut.GetAnimations().empty());

    // Transition to mesh without a rig does not create state
    pool.Remove(e1);
    const auto e2 = nc::Entity{2, 0, 0};
    pool.Emplace(e2, nc::asset::MeshView{.id = 0}, nc::MaterialDesc{});
    pool.CommitStagedComponents({});
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = 2,
        .type = nc::AnimationTransitionType::Loop
    };

    uut.Transition(pool, storage);
    EXPECT_TRUE(uut.GetEntities().empty());
    EXPECT_TRUE(uut.GetAnimations().empty());
}

TEST_F(AnimationStateOrchestratorTest, Transition_animatable_handlesAllTransitionTypes)
{
    const auto e1 = nc::Entity{1, 0, 0};
    pool.Emplace(e1, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.CommitStagedComponents({});

    // Continue transition ignored
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = 2,
        .type = nc::AnimationTransitionType::Continue
    };

    uut.Transition(pool, storage);
    EXPECT_TRUE(uut.GetEntities().empty());
    EXPECT_TRUE(uut.GetAnimations().empty());

    // Valid transition creates new state
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = 2,
        .type = nc::AnimationTransitionType::Loop
    };

    uut.Transition(pool, storage);
    EXPECT_EQ(1, uut.GetEntities().size());
    EXPECT_EQ(1, uut.GetAnimations().size());
    EXPECT_EQ(2, uut.GetAnimations().at(0).animId);

    // Valid transition with existing state overwrites
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = 3,
        .type = nc::AnimationTransitionType::PlayOnce
    };

    uut.Transition(pool, storage);
    EXPECT_EQ(1, uut.GetEntities().size());
    EXPECT_EQ(1, uut.GetAnimations().size());
    EXPECT_EQ(3, uut.GetAnimations().at(0).animId);

    // Stop transition removes state
    g_nextTransition = nc::AnimationTransition{
        .type = nc::AnimationTransitionType::Stop
    };

    uut.Transition(pool, storage);
    EXPECT_TRUE(uut.GetEntities().empty());
    EXPECT_TRUE(uut.GetAnimations().empty());
}

TEST_F(AnimationStateOrchestratorTest, NotifyCompleted_notifiesControllers)
{
    const auto entities = std::array{
        nc::Entity{1, 0, 0},
        nc::Entity{2, 0, 0}
    };

    pool.Emplace(entities[0], nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.Emplace(entities[1], nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.CommitStagedComponents({});
    uut.NotifyCompleted(pool, entities);
    EXPECT_EQ(2, g_completedCount);
}

TEST_F(AnimationStateOrchestratorTest, Remove_clearsStates)
{
    const auto e1 = nc::Entity{1, 0, 0};
    const auto e2 = nc::Entity{2, 0, 0};
    pool.Emplace(e1, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.Emplace(e2, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.CommitStagedComponents({});
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = 1,
        .type = nc::AnimationTransitionType::PlayOnce
    };

    uut.Transition(pool, storage);
    ASSERT_EQ(2, uut.GetEntities().size());
    ASSERT_EQ(2, uut.GetAnimations().size());

    uut.Remove(std::span{&e1, 1});
    EXPECT_EQ(1, uut.GetEntities().size());
    EXPECT_EQ(1, uut.GetAnimations().size());

    uut.Remove(std::span{&e2, 1});
    EXPECT_EQ(0, uut.GetEntities().size());
    EXPECT_EQ(0, uut.GetAnimations().size());
}

TEST_F(AnimationStateOrchestratorTest, Purge_preservesPersistentEntities)
{
    const auto e1 = nc::Entity{1, 0, nc::Entity::Flags::Persistent};
    const auto e2 = nc::Entity{2, 0, 0};
    const auto e3 = nc::Entity{3, 0, nc::Entity::Flags::Persistent};
    const auto e4 = nc::Entity{4, 0, 0};
    pool.Emplace(e1, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.Emplace(e2, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.Emplace(e3, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.Emplace(e4, nc::asset::MeshView{.id = g_testMeshId}, nc::MaterialDesc{});
    pool.CommitStagedComponents({});
    g_nextTransition = nc::AnimationTransition{
        .toAnimId = 1,
        .type = nc::AnimationTransitionType::PlayOnce
    };

    uut.Transition(pool, storage);
    ASSERT_EQ(4, uut.GetEntities().size());
    ASSERT_EQ(4, uut.GetAnimations().size());

    uut.Purge();
    EXPECT_EQ(2, uut.GetEntities().size());
    EXPECT_EQ(2, uut.GetAnimations().size());
}
