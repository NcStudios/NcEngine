#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/MaterialPassCache.h"

#include <array>

class MaterialPassCacheTest : public testing::Test
{
    protected:
        static constexpr auto allPasses = std::array{
            nc::MaterialPass::Shadow,
            nc::MaterialPass::Toon,
            nc::MaterialPass::Alpha
        };

        nc::graphics::MaterialPassCache uut;

        MaterialPassCacheTest()
            : uut{allPasses}
        {
        }
};

constexpr auto mesh1 = nc::asset::MeshView{
    .id = 0,
    .firstVertex = 0,
    .vertexCount = 1,
    .firstIndex = 0,
    .indexCount = 1
};

constexpr auto mesh2 = nc::asset::MeshView{
    .id = 1,
    .firstVertex = 10,
    .vertexCount = 11,
    .firstIndex = 10,
    .indexCount = 10
};

TEST_F(MaterialPassCacheTest, AddDynamicTarget_addsToEnabledPasses)
{
    constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
    constexpr auto instance = 42u;
    uut.AddDynamicTarget(passes, instance, mesh1);

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    ASSERT_EQ(1, actualToonTargets.dynamicTargets.size());
    EXPECT_EQ(instance, actualToonTargets.dynamicTargets.at(0).instance);
    EXPECT_TRUE(actualToonTargets.staticTargets.empty());

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    ASSERT_EQ(1, actualAlphaTargets.dynamicTargets.size());
    EXPECT_EQ(instance, actualAlphaTargets.dynamicTargets.at(0).instance);
    EXPECT_TRUE(actualAlphaTargets.staticTargets.empty());
}

TEST_F(MaterialPassCacheTest, AddDynamicTarget_noEnabledPasses_addsToNone)
{
    constexpr auto passes = nc::MaterialPass::type{0};
    constexpr auto instance = 42u;
    uut.AddDynamicTarget(passes, instance, mesh1);

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_TRUE(actualToonTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargets.empty());

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_TRUE(actualAlphaTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargets.empty());
}

TEST_F(MaterialPassCacheTest, AddStaticTarget_addsToEnabledPasses)
{
    constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
    constexpr auto id = 10u;
    constexpr auto instance = 42u;
    uut.AddStaticTarget(passes, id, instance, mesh1);

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargetEntities.empty());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_TRUE(actualToonTargets.dynamicTargets.empty());
    EXPECT_EQ(1, actualToonTargets.staticTargets.size());
    EXPECT_EQ(instance, actualToonTargets.staticTargets.at(0).instance);
    EXPECT_EQ(1, actualToonTargets.staticTargetEntities.size());
    EXPECT_EQ(id, actualToonTargets.staticTargetEntities.at(0));

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_TRUE(actualAlphaTargets.dynamicTargets.empty());
    EXPECT_EQ(1, actualAlphaTargets.staticTargets.size());
    EXPECT_EQ(instance, actualAlphaTargets.staticTargets.at(0).instance);
    EXPECT_EQ(1, actualAlphaTargets.staticTargetEntities.size());
    EXPECT_EQ(id, actualAlphaTargets.staticTargetEntities.at(0));
}

TEST_F(MaterialPassCacheTest, AddStaticTarget_noEnabledPasses_addsToNone)
{
    constexpr auto passes = nc::MaterialPass::type{0};
    constexpr auto id = 10u;
    constexpr auto instance = 42u;
    uut.AddStaticTarget(passes, id, instance, mesh1);

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargetEntities.empty());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_TRUE(actualToonTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargetEntities.empty());

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_TRUE(actualAlphaTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargetEntities.empty());
}

TEST_F(MaterialPassCacheTest, RemoveStaticTarget_removesFromPasses)
{
    constexpr auto passes = nc::MaterialPass::Shadow |
                            nc::MaterialPass::Toon |
                            nc::MaterialPass::Alpha;
    constexpr auto id = 10u;
    constexpr auto instance = 42u;
    uut.AddStaticTarget(passes, id, instance, mesh1);
    uut.RemoveStaticTarget(passes, id);

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargetEntities.empty());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_TRUE(actualToonTargets.staticTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargetEntities.empty());

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_TRUE(actualAlphaTargets.staticTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargetEntities.empty());
}

TEST_F(MaterialPassCacheTest, UpdateStaticTargetInstance_updatesInstanceAcrossPasses)
{
    constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
    constexpr auto id = 10u;
    constexpr auto initialInstance = 42u;
    constexpr auto updatedInstance = 10u;
    uut.AddStaticTarget(passes, id, initialInstance, mesh1);
    uut.UpdateStaticTargetInstance(passes, id, updatedInstance);

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_EQ(updatedInstance, actualToonTargets.staticTargets.at(0).instance);

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_EQ(updatedInstance, actualAlphaTargets.staticTargets.at(0).instance);
}

TEST_F(MaterialPassCacheTest, UpdateStaticTargetMesh_updatesMeshAcrossPasses)
{
    constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
    constexpr auto id = 10u;
    constexpr auto instance = 42u;
    uut.AddStaticTarget(passes, id, instance, mesh1);
    uut.UpdateStaticTargetMesh(passes, id, mesh2);

    const auto& actualToonTarget = uut.GetTargets(nc::MaterialPass::Toon).staticTargets.at(0);
    EXPECT_EQ(instance, actualToonTarget.instance);
    EXPECT_EQ(mesh2.firstVertex, actualToonTarget.firstVertex);
    EXPECT_EQ(mesh2.firstIndex, actualToonTarget.firstIndex);
    EXPECT_EQ(mesh2.indexCount, actualToonTarget.indexCount);

    const auto& actualAlphaTarget = uut.GetTargets(nc::MaterialPass::Alpha).staticTargets.at(0);
    EXPECT_EQ(instance, actualAlphaTarget.instance);
    EXPECT_EQ(mesh2.firstVertex, actualAlphaTarget.firstVertex);
    EXPECT_EQ(mesh2.firstIndex, actualAlphaTarget.firstIndex);
    EXPECT_EQ(mesh2.indexCount, actualAlphaTarget.indexCount);
}

TEST_F(MaterialPassCacheTest, UpdateStaticTargetPasses_reassignsPasses)
{
    constexpr auto initialPasses = nc::MaterialPass::Toon;
    constexpr auto updatedPasses = nc::MaterialPass::Shadow |
                                   nc::MaterialPass::Toon |
                                   nc::MaterialPass::Alpha;
    constexpr auto id = 10u;
    constexpr auto instance = 42u;
    uut.AddStaticTarget(initialPasses, id, instance, mesh1);

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);

    uut.UpdateStaticTargetPasses(initialPasses, updatedPasses, id, instance, mesh1);
    EXPECT_EQ(instance, actualShadowTargets.staticTargets.at(0).instance);
    EXPECT_EQ(instance, actualToonTargets.staticTargets.at(0).instance);
    EXPECT_EQ(instance, actualAlphaTargets.staticTargets.at(0).instance);

    uut.UpdateStaticTargetPasses(updatedPasses, nc::MaterialPass::type{0}, id, instance, mesh1);
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargets.empty());
}

TEST_F(MaterialPassCacheTest, ClearDynamicTargets_clearsOnlyDynamicData)
{
    constexpr auto dynamicInstance = 1u;
    constexpr auto staticInstance = 0u;
    constexpr auto staticId = 10u;
    constexpr auto passes = nc::MaterialPass::Shadow |
                            nc::MaterialPass::Toon |
                            nc::MaterialPass::Alpha;
    uut.AddDynamicTarget(passes, dynamicInstance, mesh1);
    uut.AddStaticTarget(passes, staticId, staticInstance, mesh1);
    uut.ClearDynamicTargets();

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.dynamicTargets.empty());
    EXPECT_EQ(1, actualShadowTargets.staticTargets.size());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_TRUE(actualToonTargets.dynamicTargets.empty());
    EXPECT_EQ(1, actualToonTargets.staticTargets.size());

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_TRUE(actualAlphaTargets.dynamicTargets.empty());
    EXPECT_EQ(1, actualAlphaTargets.staticTargets.size());
}

TEST_F(MaterialPassCacheTest, Clear_clearsAllData)
{
    constexpr auto dynamicInstance = 1u;
    constexpr auto staticInstance = 0u;
    constexpr auto staticId = 10u;
    constexpr auto passes = nc::MaterialPass::Shadow |
                            nc::MaterialPass::Toon |
                            nc::MaterialPass::Alpha;
    uut.AddDynamicTarget(passes, dynamicInstance, mesh1);
    uut.AddStaticTarget(passes, staticId, staticInstance, mesh1);
    uut.Clear();

    const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
    EXPECT_TRUE(actualShadowTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargets.empty());
    EXPECT_TRUE(actualShadowTargets.staticTargetEntities.empty());

    const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
    EXPECT_TRUE(actualToonTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargets.empty());
    EXPECT_TRUE(actualToonTargets.staticTargetEntities.empty());

    const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
    EXPECT_TRUE(actualAlphaTargets.dynamicTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargets.empty());
    EXPECT_TRUE(actualAlphaTargets.staticTargetEntities.empty());
}

TEST_F(MaterialPassCacheTest, BuildState_returnsExpectedTargets)
{
    constexpr auto dynamicPasses1 = nc::MaterialPass::Shadow | nc::MaterialPass::Toon;
    constexpr auto dynamicPasses2 = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
    constexpr auto staticPasses = nc::MaterialPass::Shadow | nc::MaterialPass::Toon;
    constexpr auto dynamicInstance1 = 0u;
    constexpr auto dynamicInstance2 = 0u;
    constexpr auto staticInstance = 0u;
    constexpr auto staticId = 10u;
    uut.AddDynamicTarget(dynamicPasses1, dynamicInstance1, mesh1);
    uut.AddDynamicTarget(dynamicPasses2, dynamicInstance2, mesh1);
    uut.AddStaticTarget(staticPasses, staticId, staticInstance, mesh1);
    const auto actualState = uut.BuildState();

    ASSERT_EQ(3, actualState.size());
    const auto& shadowTargets = actualState.at(0);
    const auto& toonTargets = actualState.at(1);
    const auto& alphaTargets = actualState.at(2);

    EXPECT_EQ(1, shadowTargets.dynamicTargets.size());
    EXPECT_EQ(1, shadowTargets.staticTargets.size());
    EXPECT_EQ(2, toonTargets.dynamicTargets.size());
    EXPECT_EQ(1, toonTargets.staticTargets.size());
    EXPECT_EQ(1, alphaTargets.dynamicTargets.size());
    EXPECT_EQ(0, alphaTargets.staticTargets.size());
}
