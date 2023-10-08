#pragma once

#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"
#include "DirectXMath.h"

namespace nc::graphics
{
class PointLight final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(PointLight)

    public:
        PointLight(Entity entity, const Vector3& ambient, const Vector3& diffuseColor, float diffuseIntensity)
            : ComponentBase{entity},
              m_ambient{ambient},
              m_diffuseColor{diffuseColor},
              m_diffuseIntensity{diffuseIntensity}
        {
        }

        auto GetAmbient() const noexcept -> const Vector3&
        {
            return m_ambient;
        }

        auto GetDiffuseColor() const noexcept -> const Vector3&
        {
            return m_diffuseColor;
        }

        auto GetDiffuseIntensity() const noexcept -> float
        {
            return m_diffuseIntensity;
        }

        void SetAmbient(const Vector3& ambient) noexcept
        {
            m_ambient = ambient;
        }

        void SetDiffuseColor(const Vector3& diffuseColor) noexcept
        {
            m_diffuseColor = diffuseColor;
        }

        void SetDiffuseIntensity(float diffuseIntensity) noexcept
        {
            m_diffuseIntensity = diffuseIntensity;
        }

    private:
        Vector3 m_ambient;
        Vector3 m_diffuseColor;
        float m_diffuseIntensity;
};
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::PointLight> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
