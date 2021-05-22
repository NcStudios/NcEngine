#pragma once

#include "component/Component.h"
#include "math/Vector4.h"
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

    class PointLight final : public Component
    {
        public:
            PointLight(EntityHandle parentHandle);
            PointLight(EntityHandle parentHandle, PointLightInfo info);
            const PointLightInfo& GetInfo();
            void Set(const PointLightInfo& info);
            void Update();

        #ifdef NC_EDITOR_ENABLED
        void EditorGuiElement() override;
        #endif

        private:
            float m_range;
            PointLightInfo m_info;
            nc::Transform* m_transform;
    };
}