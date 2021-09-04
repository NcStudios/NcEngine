#pragma once

#include "component/Component.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/TechniqueType.h"
#include "directx/math/DirectXMath.h"

#include <string>

namespace nc { class Transform; }

namespace nc::vulkan
{
    struct ObjectData
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX normal;
        DirectX::XMMATRIX viewProjection;

        uint32_t baseColorIndex; // Todo: Make this more generic for materials
        uint32_t normalIndex;  // Todo: Make this more generic for materials
        uint32_t roughnessIndex;  // Todo: Make this more generic for materials

        uint32_t isInitialized = 0;
    };

    class MeshRenderer : public ComponentBase
    {
        public:
            MeshRenderer(Entity entity, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType);
            const nc::graphics::vulkan::Mesh& GetMesh() const;
            nc::graphics::vulkan::Material& GetMaterial();
            nc::graphics::vulkan::TechniqueType GetTechniqueType() const;
            const ObjectData& GetObjectData() const;
            void Update(Transform* transform, const DirectX::FXMMATRIX& viewMatrix, const DirectX::FXMMATRIX& projectionMatrix);

        private:
            nc::graphics::vulkan::Mesh m_mesh;
            nc::graphics::vulkan::Material m_material;
            nc::graphics::vulkan::TechniqueType m_techniqueType;
            ObjectData m_objectData;
    };
}

namespace nc
{
    template<>
    struct StoragePolicy<vulkan::MeshRenderer>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::MeshRenderer>(vulkan::MeshRenderer* meshRenderer);
    #endif
}