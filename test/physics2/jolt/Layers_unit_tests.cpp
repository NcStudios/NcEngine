#include "gtest/gtest.h"
#include "physics2/jolt/Layers.h"

constexpr auto g_staticObject = nc::physics::ObjectLayer::Static;
constexpr auto g_dynamicObject = nc::physics::ObjectLayer::Dynamic;
constexpr auto g_staticPhase = nc::physics::BroadPhaseLayer::Static;
constexpr auto g_dynamicPhase = nc::physics::BroadPhaseLayer::Dynamic;

TEST(LayersTest, ValidateLayerAssumptions)
{
    static_assert(nc::physics::ObjectLayer::LayerCount == 2u);
    static_assert(nc::physics::BroadPhaseLayer::LayerCount == 2u);
    EXPECT_EQ(g_staticObject, g_staticPhase.GetValue());
    EXPECT_EQ(g_dynamicObject, g_dynamicPhase.GetValue());
}

TEST(LayersTest, LayerMap_GetNumBroadPhaseLayers_returnsCorrectLayerCount)
{
    auto uut = nc::physics::LayerMap{};
    EXPECT_EQ(2u, uut.GetNumBroadPhaseLayers());
}

TEST(LayersTest, LayerMap_GetBroadPhaseLayer_mapsExpectedLayers)
{
    auto uut = nc::physics::LayerMap{};
    EXPECT_EQ(g_staticPhase, uut.GetBroadPhaseLayer(g_staticObject));
    EXPECT_EQ(g_dynamicPhase, uut.GetBroadPhaseLayer(g_dynamicObject));
}

TEST(LayersTest, ObjectLayerPairFilter_ShouldCollide_dynamicVsAll_returnsTrue)
{
    const auto uut = nc::physics::ObjectLayerPairFilter{};
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_dynamicObject));
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_staticObject));
    EXPECT_TRUE(uut.ShouldCollide(g_staticObject, g_dynamicObject));
}

TEST(LayersTest, ObjectLayerPairFilter_ShouldCollide_staticVsStatic_returnsFalse)
{
    const auto uut = nc::physics::ObjectLayerPairFilter{};
    EXPECT_FALSE(uut.ShouldCollide(g_staticObject, g_staticObject));
}

TEST(LayersTest, ObjectVsBroadPhaseLayerFilter_ShouldCollide_dynamicVsAll_returnsTrue)
{
    const auto uut = nc::physics::ObjectVsBroadPhaseLayerFilter{};
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_dynamicPhase));
    EXPECT_TRUE(uut.ShouldCollide(g_dynamicObject, g_staticPhase));
    EXPECT_TRUE(uut.ShouldCollide(g_staticObject, g_dynamicPhase));
}

TEST(LayersTest, ObjectVsBroadPhaseLayerFilter_ShouldCollide_staticVsStatic_returnsFalse)
{
    const auto uut = nc::physics::ObjectVsBroadPhaseLayerFilter{};
    EXPECT_FALSE(uut.ShouldCollide(g_staticObject, g_staticPhase));
}
