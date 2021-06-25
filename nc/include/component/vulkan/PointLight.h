#pragma once

#include "component/Component.h"
#include "math/Vector.h"
#include "directx/math/DirectXMath.h"

namespace nc { class Transform; }

namespace nc::vulkan
{
    struct Attenuation
    {
        float constant = 1.0f;
        float linear = 0.7f;
        float quadratic = 1.8f;
    };

    Attenuation GetAttenuationFromRange(float range);

    struct PointLightInfo
    {
        Vector4 pos = Vector4::Zero();
        Vector4 ambient = Vector4{0.2f, 0.2f, 0.2f, 1.0f};
        Vector4 diffuseColor = Vector4{0.75f, 0.75f, 0.75f, 1.0f};
        Vector4 specularColor = Vector4{0.75f, 0.75f, 0.75f, 1.0f};
        float diffuseIntensity = 10.0f;
        float attConst = 1.0f;
        float attLin = 0.22f;
        float attQuad = 0.20f;
        bool isInitialized = 0;
        float padding[3] = {0.0f, 0.0f, 0.0f};
    };

    class PointLight final : public ComponentBase
    {
        public:
            PointLight(Entity entity, PointLightInfo info);

            const PointLightInfo& GetInfo() const;
            float GetRange() const;

            void SetInfo(PointLightInfo info);
            void SetRange(float range);

            void Update();

        private:
            float m_range;
            PointLightInfo m_info;
    };
}

namespace nc
{
    template<>
    struct StoragePolicy<vulkan::PointLight>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::PointLight>(vulkan::PointLight* light);
    #endif
}