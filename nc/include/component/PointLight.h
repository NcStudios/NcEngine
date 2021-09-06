#pragma once

#include "component/Component.h"
#include "math/Vector.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    class Transform;

    struct Attenuation
    {
        float constant = 1.0f;
        float linear = 0.7f;
        float quadratic = 1.8f;
    };

    Attenuation GetAttenuationFromRange(float range);

    struct PointLightInfo
    {
        alignas(16)Vector3 pos = Vector3::Zero();
        alignas(16)Vector3 ambient = Vector3::Splat(0.35f);
        alignas(16)Vector3 diffuseColor = Vector3::One();
        float diffuseIntensity = 2.5f;
        float attConst = 2.61f;
        float attLin = 0.1819f;
        float attQuad = 0.0000001f;
        int isInitialized = 1;
        float padding[15] = {};
    };

    #ifdef NC_EDITOR_ENABLED
    void SerializeToFile(std::string filePath, const PointLightInfo& info); // @todo: remove or build out into formal asset generator
    #endif

    class PointLight final : public ComponentBase
    {
        public:
            PointLight(Entity entity, PointLightInfo info);

            const PointLightInfo& GetInfo() const;
            float GetRange() const;
            bool IsDirty() const;

            void SetInfo(PointLightInfo info);
            void SetRange(float range);
            bool Update(const DirectX::XMMATRIX& view);

        private:
            float m_range;
            PointLightInfo m_info;
            bool m_isDirty;

            alignas(16)Vector3 ProjectedPos;
    };
    
    template<>
    struct StoragePolicy<PointLight>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PointLight>(PointLight* light);
    #endif
}