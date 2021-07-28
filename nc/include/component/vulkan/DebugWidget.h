#pragma once

#include "component/Component.h"
#include "directx/math/DirectXMath.h"

#include <string>

namespace nc { class Transform; }

const auto CubeMeshPath = std::string{"project/assets/mesh/cube.nca"};
const auto SphereMeshPath = std::string{"project/assets/mesh/sphere.nca"};
const auto CapsuleMeshPath = std::string{"project/assets/mesh/capsule.nca"};

namespace nc::vulkan
{
    enum class WidgetShape : uint8_t
    {
        Cube,
        Sphere,
        Capsule,
        Hull
    };

    class DebugWidget : public ComponentBase
    {
        public:
            DebugWidget(Entity entity, WidgetShape shape);

            DirectX::FXMMATRIX GetTransformationMatrix() const noexcept;
            const std::string& GetMeshUid() const;
            bool IsEnabled() const noexcept;

            void Enable(bool isEnabled) noexcept;
            void SetTransformationMatrix(DirectX::FXMMATRIX matrix) noexcept;

        private:
            bool m_isEnabled;
            std::string m_meshUid;
            DirectX::XMMATRIX m_transformationMatrix;
    };
}

namespace nc
{
    template<>
    struct StoragePolicy<vulkan::DebugWidget>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::DebugWidget>(vulkan::DebugWidget* debugWidget);
    #endif
}