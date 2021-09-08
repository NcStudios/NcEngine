#pragma once

#include "component/Component.h"
#include "math/Vector.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    constexpr size_t PointLightInfoSize = 128;

    class Transform;
    
    struct alignas(PointLightInfoSize) PointLightInfo
    {
        DirectX::XMMATRIX viewProjection = {};
        Vector3 pos = Vector3::Zero();
        float attConst = 2.61f;
        Vector3 ambient = Vector3::Splat(0.35f);
        float attLin = 0.1819f;
        Vector3 diffuseColor = Vector3::One();
        float attQuad = 0.0000001f;
        float diffuseIntensity = 2.5f;
        int isInitialized = 1;
        float padding[2] = {};
    };

    #ifdef NC_EDITOR_ENABLED
    void SerializeToFile(const std::string& filePath, const PointLightInfo& info); // @todo: remove or build out into formal asset generator
    #endif

    class PointLight final : public ComponentBase
    {
        public:
            PointLight(Entity entity, PointLightInfo info);

            const PointLightInfo& GetInfo() const;
            bool IsDirty() const;

            void SetInfo(PointLightInfo info);
            bool Update(const DirectX::XMMATRIX& cameraView);

        private:
            DirectX::XMMATRIX CalculateLightViewMatrix();

            PointLightInfo m_info;
            DirectX::XMMATRIX m_lightProjectionMatrix;
            alignas(16)Vector3 m_projectedPos;

            bool m_isDirty;
    };

    static_assert(sizeof(PointLightInfo) == PointLightInfoSize, "PointLight::PointLight Size of m_info must be 128 bytes.");
    
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