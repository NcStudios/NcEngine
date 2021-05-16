#pragma once

#include "component/Component.h"
#include "directx/math/DirectXMath.h"

namespace nc { class Transform; }

namespace nc::vulkan
{
    struct PointLightInfo
    {
        alignas(16)Vector3 pos = Vector3::Zero();
        alignas(16)Vector3 ambient = Vector3::Splat(0.65f);
        alignas(16)Vector3 diffuseColor = Vector3::One();
        float diffuseIntensity = 0.9f;
        float attConst = 2.61f;
        float attLin = 0.1819f;
        float attQuad = 0.0000001f;
    };

    class PointLight final : public Component
    {
        public:
            PointLight(EntityHandle parentHandle);
            PointLight(EntityHandle parentHandle, const PointLightInfo& info);
            const PointLightInfo& GetInfo();
            void Update(const PointLightInfo& info);

        #ifdef NC_EDITOR_ENABLED
        void EditorGuiElement() override;
        #endif

        private:
            PointLightInfo m_info;
            nc::Transform* m_transform;
    };
}