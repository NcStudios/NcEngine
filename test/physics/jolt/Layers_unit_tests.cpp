#include "gtest/gtest.h"
#include "physics2/jolt/Layers.h"

constexpr auto g_staticObject = nc::physics::ObjectLayer::Static;
constexpr auto g_dynamicObject = nc::physics::ObjectLayer::Dynamic;
constexpr auto g_triggerObject = nc::physics::ObjectLayer::Trigger;
constexpr auto g_staticPhase = nc::physics::BroadPhaseLayer::Static;
constexpr auto g_dynamicPhase = nc::physics::BroadPhaseLayer::Dynamic;
constexpr auto g_triggerPhase = nc::physics::BroadPhaseLayer::Trigger;

TEST(LayersTest, ValidateLayerAssumptions)
{
    static_assert(nc::physics::ObjectLayer::LayerCount == 3u);
    static_assert(nc::physics::BroadPhaseLayer::LayerCount == 3u);
    EXPECT_EQ(g_staticObject, g_staticPhase.GetValue());
    EXPECT_EQ(g_dynamicObject, g_dynamicPhase.GetValue());
    EXPECT_EQ(g_triggerObject, g_triggerPhase.GetValue());
}

TEST(LayersTest, LayerMap_GetNumBroadPhaseLayers_returnsCorrectLayerCount)
{
    auto uut = nc::physics::LayerMap{};
    EXPECT_EQ(3u, uut.GetNumBroadPhaseLayers());
}

TEST(LayersTest, LayerMap_GetBroadPhaseLayer_mapsExpectedLayers)
{
    auto uut = nc::physics::LayerMap{};
    EXPECT_EQ(g_staticPhase, uut.GetBroadPhaseLayer(g_staticObject));
    EXPECT_EQ(g_dynamicPhase, uut.GetBroadPhaseLayer(g_dynamicObject));
    EXPECT_EQ(g_triggerPhase, uut.GetBroadPhaseLayer(g_triggerObject));
}

TEST(LayersTest, ObjectLayerPairFilter_ShouldCollide_dynamicVsAll_returnsTrue)
{
    const auto uut = nc::physics::ObjectLayerPairFilter{};
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_dynamicObject));
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_staticObject));
    EXPECT_TRUE(uut.ShouldCollide(g_staticObject, g_dynamicObject));
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_triggerObject));
    EXPECT_TRUE(uut.ShouldCollide(g_triggerObject, g_dynamicObject));
}

TEST(LayersTest, ObjectLayerPairFilter_ShouldCollide_staticVsNonDynamic_returnsFalse)
{
    const auto uut = nc::physics::ObjectLayerPairFilter{};
    EXPECT_FALSE(uut.ShouldCollide(g_staticObject, g_staticObject));
    EXPECT_FALSE(uut.ShouldCollide(g_staticObject, g_triggerObject));
    EXPECT_FALSE(uut.ShouldCollide(g_triggerObject, g_staticObject));
}

TEST(LayersTest, ObjectLayerPairFilter_ShouldCollide_triggerVsTrigger_returnsFalse)
{
    const auto uut = nc::physics::ObjectLayerPairFilter{};
    EXPECT_FALSE(uut.ShouldCollide(g_triggerObject, g_triggerObject));
}

TEST(LayersTest, ObjectVsBroadPhaseLayerFilter_ShouldCollide_dynamicVsAll_returnsTrue)
{
    const auto uut = nc::physics::ObjectVsBroadPhaseLayerFilter{};
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_dynamicPhase));
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_staticPhase));
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_triggerPhase));
}

TEST(LayersTest, ObjectVsBroadPhaseLayerFilter_ShouldCollide_staticVsAll_returnsTrueOnlyForDynamic)
{
    const auto uut = nc::physics::ObjectVsBroadPhaseLayerFilter{};
    EXPECT_TRUE(uut.ShouldCollide(g_staticObject, g_dynamicPhase));
    EXPECT_FALSE(uut.ShouldCollide(g_staticObject, g_staticPhase));
    EXPECT_FALSE(uut.ShouldCollide(g_staticObject, g_triggerPhase));
}

TEST(LayersTest, ObjectVsBroadPhaseLayerFilter_ShouldCollide_triggerVsAll_returnsTrueOnlyForDynamic)
{
    const auto uut = nc::physics::ObjectVsBroadPhaseLayerFilter{};
    EXPECT_TRUE(uut.ShouldCollide(g_triggerObject, g_dynamicPhase));
    EXPECT_FALSE(uut.ShouldCollide(g_triggerObject, g_staticPhase));
    EXPECT_FALSE(uut.ShouldCollide(g_triggerObject, g_triggerPhase));
}
