#pragma once

#include "component/Component.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "graphics/TechniqueType.h"
#include "graphics/Texture.h"

namespace nc
{
    class MeshRenderer : public ComponentBase
    {
        NC_ENABLE_IN_EDITOR(MeshRenderer)
        
        public:
            MeshRenderer(Entity entity, std::string meshUid, graphics::Material material, graphics::TechniqueType techniqueType);
            
            auto GetMesh() const -> const graphics::Mesh& { return m_mesh; }
            auto GetTextureIndices() const -> const graphics::TextureIndices& { return m_textureIndices; }
            auto GetTechniqueType() const -> graphics::TechniqueType { return m_techniqueType; }

            void SetMesh(std::string meshUid);
            void SetBaseColor(const std::string& texturePath);
            void SetNormal(const std::string& texturePath);
            void SetRoughness(const std::string& texturePath);

            #ifdef NC_EDITOR_ENABLED
            auto GetMaterial() -> graphics::Material& { return m_material; }
            auto GetMeshPath() const -> const std::string& { return m_meshPath; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            graphics::Material m_material;
            std::string m_meshPath;
            #endif
            graphics::Mesh m_mesh;
            graphics::TextureIndices m_textureIndices;
            graphics::TechniqueType m_techniqueType;
    };
    
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}
