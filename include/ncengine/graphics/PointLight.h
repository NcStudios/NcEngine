#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"
#include "DirectXMath.h"

namespace nc
{
namespace ecs
{
class PointLightSystem;
}

namespace graphics
{
struct PerFrameRenderState;
constexpr size_t PointLightInfoSize = 128;

/** @todo As this is a shader resource, we should be using a different
 *  type for initializing point lights. It would be reasonable to assume
 *  that 'pos' controls the position or an offset for the light, when in
 *  reality it is just overwritten with the transform position.
 *  @todo alignas was moved from the definition to the m_info member to
 *  make msvc happy. This pushes alignment concerns off to other users,
 *  which doesn't seem wise. */
struct /** alignas(PointLightInfoSize) */ PointLightInfo
{
    DirectX::XMMATRIX viewProjection = {};
    Vector3 pos = Vector3::Zero();
    int castShadows = 1;
    Vector3 ambient = Vector3::Splat(0.35f);
    float paddingA = 0.0f; /** todo - Remove */
    Vector3 diffuseColor = Vector3::One();
    float paddingB = 0.0f;  /** todo - Remove */
    float diffuseIntensity = 2.5f;
    int isInitialized = 1;
    float padding[2] = {};
};

class PointLight final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(PointLight)

    public:
        PointLight(Entity entity, PointLightInfo info);

        auto GetInfo() const -> const PointLightInfo& { return m_info; }
        void SetInfo(const PointLightInfo& info);

    private:
        alignas(PointLightInfoSize) PointLightInfo m_info;
        DirectX::XMMATRIX m_lightProjectionMatrix;
        bool m_isDirty;

        auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix) -> DirectX::XMMATRIX;
        auto Update(const Vector3& position, const DirectX::XMMATRIX& lightViewProj) -> bool;
        void SetDirty();

        friend ecs::PointLightSystem;
        friend graphics::PerFrameRenderState;
};

static_assert(sizeof(PointLightInfo) == PointLightInfoSize, "PointLight::PointLight Size of m_info must be 128 bytes.");
} // namespace graphics

template<>
struct StoragePolicy<graphics::PointLight> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};

#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::PointLight>(graphics::PointLight* light);
void SerializeToFile(const std::string& filePath, const graphics::PointLightInfo& info); // @todo: remove or build out into formal asset generator
#endif
} // namespace nc
