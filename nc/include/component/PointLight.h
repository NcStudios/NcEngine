#pragma once
#include "Component.h"
#include "directx/math/DirectXMath.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; }

namespace nc
{
    class Transform;

    class PointLight final: public ComponentBase
    {
        public:
            struct Properties
            {
                alignas(16)Vector3 pos = Vector3::Zero();
                alignas(16)Vector3 ambient = Vector3::Splat(0.65f);
                alignas(16)Vector3 diffuseColor = Vector3::One();
                float diffuseIntensity = 0.9f;
                float attConst = 2.61f;
                float attLin = 0.1819f;
                float attQuad = 0.0000001f;
            } PixelConstBufData;

            alignas(16)Vector3 ProjectedPos;

            PointLight(EntityHandle handle, Properties properties) noexcept;
            ~PointLight() = default;
            PointLight(PointLight&&) = default;
            PointLight& operator=(PointLight&&) = default;
            PointLight(const PointLight&) = delete;
            PointLight& operator=(const PointLight&) = delete;

            void Set(const PointLight::Properties& lightProperties);
            void SetPositionFromCameraProjection(const DirectX::FXMMATRIX& view);

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

        private:
            //Transform * m_transform;
    };

    template<>
    struct StoragePolicy<PointLight>
    {
        using allow_trivial_destruction = std::true_type;
        using sort_dense_storage_by_address = std::true_type;
    };
}