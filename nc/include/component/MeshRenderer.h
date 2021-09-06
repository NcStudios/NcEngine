#pragma once

#include "component/Component.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "graphics/TechniqueType.h"
#include "directx/math/DirectXMath.h"

#include <string>

namespace nc { class Transform; }

namespace nc
{
    struct ObjectData
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX modelView;
        DirectX::XMMATRIX viewProjection;

        uint32_t baseColorIndex; // Todo: Make this more generic for materials
        uint32_t normalIndex;  // Todo: Make this more generic for materials
        uint32_t roughnessIndex;  // Todo: Make this more generic for materials

        uint32_t isInitialized = 0;
    };

    class MeshRenderer : public ComponentBase
    {
        public:
            MeshRenderer(Entity entity, std::string meshUid, nc::graphics::Material material, nc::graphics::TechniqueType techniqueType);
            const nc::graphics::Mesh& GetMesh() const;
            nc::graphics::Material& GetMaterial();
            nc::graphics::TechniqueType GetTechniqueType() const;
            const ObjectData& GetObjectData() const;
            void Update(Transform* transform, const DirectX::FXMMATRIX& viewMatrix, const DirectX::FXMMATRIX& projectionMatrix);

        private:
            nc::graphics::Mesh m_mesh;
            nc::graphics::Material m_material;
            nc::graphics::TechniqueType m_techniqueType;
            ObjectData m_objectData;
    };
}

namespace nc
{
    template<>
    struct StoragePolicy<MeshRenderer>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}