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
        public:
            MeshRenderer(Entity entity, std::string meshUid, nc::graphics::Material material, nc::graphics::TechniqueType techniqueType);
            
            auto GetMesh() const -> const graphics::Mesh& { return m_mesh; }
            auto GetTextureIndices() const -> const graphics::TextureIndices& { return m_textureIndices; }
            auto GetTechniqueType() const -> graphics::TechniqueType { return m_techniqueType; }

            #ifdef NC_EDITOR_ENABLED
            auto GetMaterial() -> graphics::Material& { return m_material; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            graphics::Material m_material;
            #endif
            graphics::Mesh m_mesh;
            graphics::TextureIndices m_textureIndices;
            graphics::TechniqueType m_techniqueType;
    };
    
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}