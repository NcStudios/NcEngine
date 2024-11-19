// #include "gtest/gtest.h"
// #include "graphics2/frontend/subsystem/MaterialPassCache.h"

// #include <array>

// class MaterialPassCacheTest : public testing::Test
// {
//     protected:
//         static constexpr auto allPasses = std::array{
//             nc::MaterialPass::Shadow,
//             nc::MaterialPass::Toon,
//             nc::MaterialPass::Alpha
//         };

//         nc::graphics::MaterialPassCache uut;

//         MaterialPassCacheTest()
//             : uut{allPasses}
//         {
//         }
// };

// constexpr auto mesh1 = nc::asset::MeshView{
//     .id = 0,
//     .firstVertex = 0,
//     .vertexCount = 1,
//     .firstIndex = 0,
//     .indexCount = 1
// };

// constexpr auto mesh2 = nc::asset::MeshView{
//     .id = 1,
//     .firstVertex = 10,
//     .vertexCount = 11,
//     .firstIndex = 10,
//     .indexCount = 10
// };

// TEST_F(MaterialPassCacheTest, AddTarget_addsToEnabledPasses)
// {
//     constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
//     constexpr auto id = 10u;
//     constexpr auto instance = 42u;
//     uut.AddTarget(passes, id, instance, mesh1);

//     const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
//     EXPECT_TRUE(actualShadowTargets.targets.empty());
//     EXPECT_TRUE(actualShadowTargets.entities.empty());

//     const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
//     EXPECT_EQ(1, actualToonTargets.targets.size());
//     EXPECT_EQ(instance, actualToonTargets.targets.at(0).instance);
//     EXPECT_EQ(1, actualToonTargets.entities.size());
//     EXPECT_EQ(id, actualToonTargets.entities.at(0));

//     const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
//     EXPECT_EQ(1, actualAlphaTargets.targets.size());
//     EXPECT_EQ(instance, actualAlphaTargets.targets.at(0).instance);
//     EXPECT_EQ(1, actualAlphaTargets.entities.size());
//     EXPECT_EQ(id, actualAlphaTargets.entities.at(0));
// }

// TEST_F(MaterialPassCacheTest, AddTarget_noEnabledPasses_addsToNone)
// {
//     constexpr auto passes = nc::MaterialPass::type{0};
//     constexpr auto id = 10u;
//     constexpr auto instance = 42u;
//     uut.AddTarget(passes, id, instance, mesh1);

//     const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
//     EXPECT_TRUE(actualShadowTargets.targets.empty());
//     EXPECT_TRUE(actualShadowTargets.entities.empty());

//     const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
//     EXPECT_TRUE(actualToonTargets.targets.empty());
//     EXPECT_TRUE(actualToonTargets.entities.empty());

//     const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
//     EXPECT_TRUE(actualAlphaTargets.targets.empty());
//     EXPECT_TRUE(actualAlphaTargets.entities.empty());
// }

// TEST_F(MaterialPassCacheTest, RemoveTarget_removesFromPasses)
// {
//     constexpr auto passes = nc::MaterialPass::Shadow |
//                             nc::MaterialPass::Toon |
//                             nc::MaterialPass::Alpha;
//     constexpr auto id = 10u;
//     constexpr auto instance = 42u;
//     uut.AddTarget(passes, id, instance, mesh1);
//     uut.RemoveTarget(passes, id);

//     const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
//     EXPECT_TRUE(actualShadowTargets.targets.empty());
//     EXPECT_TRUE(actualShadowTargets.entities.empty());

//     const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
//     EXPECT_TRUE(actualToonTargets.targets.empty());
//     EXPECT_TRUE(actualToonTargets.entities.empty());

//     const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
//     EXPECT_TRUE(actualAlphaTargets.targets.empty());
//     EXPECT_TRUE(actualAlphaTargets.entities.empty());
// }

// TEST_F(MaterialPassCacheTest, UpdateTargetInstance_updatesInstanceAcrossPasses)
// {
//     constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
//     constexpr auto id = 10u;
//     constexpr auto initialInstance = 42u;
//     constexpr auto updatedInstance = 10u;
//     uut.AddTarget(passes, id, initialInstance, mesh1);
//     uut.UpdateTargetInstance(passes, id, updatedInstance);

//     const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
//     EXPECT_EQ(updatedInstance, actualToonTargets.targets.at(0).instance);

//     const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
//     EXPECT_EQ(updatedInstance, actualAlphaTargets.targets.at(0).instance);
// }

// TEST_F(MaterialPassCacheTest, UpdateTargetMesh_updatesMeshAcrossPasses)
// {
//     constexpr auto passes = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
//     constexpr auto id = 10u;
//     constexpr auto instance = 42u;
//     uut.AddTarget(passes, id, instance, mesh1);
//     uut.UpdateTargetMesh(passes, id, mesh2);

//     const auto& actualToonTarget = uut.GetTargets(nc::MaterialPass::Toon).targets.at(0);
//     EXPECT_EQ(instance, actualToonTarget.instance);
//     EXPECT_EQ(mesh2.firstVertex, actualToonTarget.firstVertex);
//     EXPECT_EQ(mesh2.firstIndex, actualToonTarget.firstIndex);
//     EXPECT_EQ(mesh2.indexCount, actualToonTarget.indexCount);

//     const auto& actualAlphaTarget = uut.GetTargets(nc::MaterialPass::Alpha).targets.at(0);
//     EXPECT_EQ(instance, actualAlphaTarget.instance);
//     EXPECT_EQ(mesh2.firstVertex, actualAlphaTarget.firstVertex);
//     EXPECT_EQ(mesh2.firstIndex, actualAlphaTarget.firstIndex);
//     EXPECT_EQ(mesh2.indexCount, actualAlphaTarget.indexCount);
// }

// TEST_F(MaterialPassCacheTest, Clear_clearsAllData)
// {
//     constexpr auto passes = nc::MaterialPass::Shadow |
//                             nc::MaterialPass::Toon |
//                             nc::MaterialPass::Alpha;
//     uut.AddTarget(passes, 0, 0, mesh1);
//     uut.Clear();

//     const auto& actualShadowTargets = uut.GetTargets(nc::MaterialPass::Shadow);
//     EXPECT_TRUE(actualShadowTargets.targets.empty());
//     EXPECT_TRUE(actualShadowTargets.entities.empty());

//     const auto& actualToonTargets = uut.GetTargets(nc::MaterialPass::Toon);
//     EXPECT_TRUE(actualToonTargets.targets.empty());
//     EXPECT_TRUE(actualToonTargets.entities.empty());

//     const auto& actualAlphaTargets = uut.GetTargets(nc::MaterialPass::Alpha);
//     EXPECT_TRUE(actualAlphaTargets.targets.empty());
//     EXPECT_TRUE(actualAlphaTargets.entities.empty());
// }

// TEST_F(MaterialPassCacheTest, BuildState_returnsExpectedTargets)
// {
//     constexpr auto passes1 = nc::MaterialPass::Shadow | nc::MaterialPass::Toon;
//     constexpr auto passes2 = nc::MaterialPass::Toon | nc::MaterialPass::Alpha;
//     constexpr auto passes3 = nc::MaterialPass::Shadow | nc::MaterialPass::Toon;
//     constexpr auto instance1 = 0u;
//     constexpr auto instance2 = 1u;
//     constexpr auto instance3 = 2u;
//     constexpr auto id1 = 10u;
//     constexpr auto id2 = 11u;
//     constexpr auto id3 = 12u;
//     uut.AddTarget(passes1, instance1, id1, mesh1);
//     uut.AddTarget(passes2, instance2, id2, mesh1);
//     uut.AddTarget(passes3, instance3, id3, mesh1);
//     const auto actualState = uut.BuildState();

//     ASSERT_EQ(3, actualState.size());
//     const auto& shadowTargets = actualState.at(0);
//     const auto& toonTargets = actualState.at(1);
//     const auto& alphaTargets = actualState.at(2);

//     EXPECT_EQ(2, shadowTargets.targets.size());
//     EXPECT_EQ(3, toonTargets.targets.size());
//     EXPECT_EQ(1, alphaTargets.targets.size());
// }
