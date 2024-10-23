#include "gtest/gtest.h"
#include "../EcsFixture.inl"
#include "graphics2/frontend/subsystem/CameraSubsystem.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"
#include "ncengine/graphics/Camera.h"

constexpr auto g_screenExtent = nc::Vector2{500.0f, 500.0f};

namespace nc::window
{
auto GetScreenExtent() -> Vector2
{
    return g_screenExtent;
}
} // namespace nc::window

class CameraSubsystemTest : public testing::Test,
                            public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 1ull;
        nc::graphics::CameraSubsystem uut;

        CameraSubsystemTest()
            : EcsFixture{MaxEntities},
              uut{}
        {
        }
};

TEST_F(CameraSubsystemTest, BuildState_nullCamera_returnsValidViewProjection)
{
    uut.Set(nullptr);
    const auto actualState = uut.BuildState(GetTestWorld());
    const auto& actualViewProjection = actualState.viewProjection;

    // Should give back something other than an identity matrix
    EXPECT_FALSE(
        DirectX::XMVector4Equal(DirectX::g_XMIdentityR0, actualViewProjection.r[0]) &&
        DirectX::XMVector4Equal(DirectX::g_XMIdentityR1, actualViewProjection.r[1]) &&
        DirectX::XMVector4Equal(DirectX::g_XMIdentityR2, actualViewProjection.r[2]) &&
        DirectX::XMVector4Equal(DirectX::g_XMIdentityR3, actualViewProjection.r[3])
    );

    // Don't care about exact values, but matrix should project in a reasonable way
    const auto nearPoint = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 1.0f);
    const auto farPoint = DirectX::XMVectorSet(0.0f, 0.0f, 50.0f, 1.0f);
    const auto projectedNearPoint = DirectX::XMVector4Transform(nearPoint, actualViewProjection);
    const auto projectedFarPoint = DirectX::XMVector4Transform(farPoint, actualViewProjection);
    EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetX(projectedNearPoint));
    EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetX(projectedFarPoint));
    EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetY(projectedNearPoint));
    EXPECT_FLOAT_EQ(0.0f, DirectX::XMVectorGetY(projectedFarPoint));
    EXPECT_LT(DirectX::XMVectorGetZ(projectedNearPoint), DirectX::XMVectorGetZ(projectedFarPoint));
}

TEST_F(CameraSubsystemTest, BuildState_validCamera_updatesCameraAndReturnsViewProjection)
{
    auto world = GetTestWorld();
    const auto entity = world.Emplace<nc::Entity>({});
    auto& camera = world.Emplace<nc::graphics::Camera>(entity);
    uut.Set(&camera);
    const auto projection = camera.ProjectionMatrix();
    const auto initialView = camera.ViewMatrix();

    world.Get<nc::Transform>(entity).Translate(nc::Vector3::Back());
    const auto actualState = uut.BuildState(world);
    const auto updatedView = camera.ViewMatrix();

    EXPECT_FALSE(
        DirectX::XMVector4Equal(initialView.r[0], updatedView.r[0]) &&
        DirectX::XMVector4Equal(initialView.r[1], updatedView.r[1]) &&
        DirectX::XMVector4Equal(initialView.r[2], updatedView.r[2]) &&
        DirectX::XMVector4Equal(initialView.r[3], updatedView.r[3])
    );

    const auto expectedViewProjection = DirectX::XMMatrixMultiply(updatedView, projection);
    const auto& actualViewProjection = actualState.viewProjection;

    EXPECT_TRUE(
        DirectX::XMVector4Equal(expectedViewProjection.r[0], actualViewProjection.r[0]) &&
        DirectX::XMVector4Equal(expectedViewProjection.r[1], actualViewProjection.r[1]) &&
        DirectX::XMVector4Equal(expectedViewProjection.r[2], actualViewProjection.r[2]) &&
        DirectX::XMVector4Equal(expectedViewProjection.r[3], actualViewProjection.r[3])
    );
}
