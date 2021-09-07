#pragma once

#include "component/Component.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "graphics/TechniqueType.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    class Transform;
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
            MeshRenderer(Entity entity, std::string meshUid, graphics::Material material, graphics::TechniqueType techniqueType);
            const graphics::Mesh& GetMesh() const;
            graphics::Material& GetMaterial();
            graphics::TechniqueType GetTechniqueType() const;
            const ObjectData& GetObjectData() const;
            void Update(Transform* transform, const DirectX::FXMMATRIX& viewMatrix, const DirectX::FXMMATRIX& projectionMatrix);

        private:
            graphics::Mesh m_mesh;
            graphics::Material m_material;
            graphics::TechniqueType m_techniqueType;
            ObjectData m_objectData;
    };
    
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}