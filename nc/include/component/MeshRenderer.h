#pragma once

#include "component/Component.h"
#include "Assets.h"
#include "graphics/Material.h"
#include "graphics/TechniqueType.h"

namespace nc
{
    struct TextureIndices
    {
        TextureView baseColor;
        TextureView normal;
        TextureView roughness;
    };

    class MeshRenderer : public ComponentBase
    {
        public:
            MeshRenderer(Entity entity, std::string meshUid, Material material, TechniqueType techniqueType);

            auto GetMesh() const -> const MeshView& { return m_mesh; }
            auto GetTextureIndices() const -> const TextureIndices& { return m_textureIndices; }
            auto GetTechniqueType() const -> TechniqueType { return m_techniqueType; }

            #ifdef NC_EDITOR_ENABLED
            auto GetMaterial() -> Material& { return m_material; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            Material m_material;
            #endif
            MeshView m_mesh;
            TextureIndices m_textureIndices;
            TechniqueType m_techniqueType;
    };
    
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}
