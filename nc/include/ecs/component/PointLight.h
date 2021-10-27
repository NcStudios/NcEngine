#pragma once

#include "Component.h"
#include "math/Vector.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    constexpr size_t PointLightInfoSize = 128;
    namespace ecs { class PointLightSystem; }
    namespace graphics { struct PerFrameRenderState; }

    /** @todo As this is a shader resource, we should be using a different
     *  type for initializing point lights. It would be reasonable to assume
     *  that 'pos' controls the position or an offset for the light, when in
     *  reality it is just overwritten with the transform position. */
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

    class PointLight final : public ComponentBase
    {
        public:
            PointLight(Entity entity, PointLightInfo info);

            auto GetInfo() const -> const PointLightInfo& { return m_info; }
            auto IsDirty() const -> bool { return m_isDirty; }
            void SetInfo(PointLightInfo info);

        private:
            DirectX::XMMATRIX CalculateLightViewProjectionMatrix();

            PointLightInfo m_info;
            DirectX::XMMATRIX m_lightProjectionMatrix;
            bool m_isDirty;

            bool Update(const Vector3& position, const DirectX::XMMATRIX& lightViewProj);

            friend ecs::PointLightSystem;
            friend graphics::PerFrameRenderState;
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
    void SerializeToFile(const std::string& filePath, const PointLightInfo& info); // @todo: remove or build out into formal asset generator
    #endif
}