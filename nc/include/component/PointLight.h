#pragma once

#include "component/Component.h"
#include "math/Vector.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    namespace ecs { class PointLightSystem; }
    namespace graphics { struct PerFrameRenderState; }

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

    class PointLight final : public ComponentBase
    {
        public:
            PointLight(Entity entity, PointLightInfo info);

            auto GetInfo() const -> const PointLightInfo& { return m_info; }
            auto IsDirty() const -> bool { return m_isDirty; }
            void SetInfo(PointLightInfo info);

        private:
            PointLightInfo m_info;
            alignas(16)Vector3 m_projectedPos;
            bool m_isDirty;

            bool Update(const Vector3& position, const DirectX::XMMATRIX& view);

            friend ecs::PointLightSystem;
            friend graphics::PerFrameRenderState;
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
    void SerializeToFile(const std::string& filePath, const PointLightInfo& info); // @todo: remove or build out into formal asset generator
    #endif
}