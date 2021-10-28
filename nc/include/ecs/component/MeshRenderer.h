#pragma once

#include "Component.h"
#include "Assets.h"

namespace nc
{
    struct Material
    {
        std::string baseColor;
        std::string normal;
        std::string roughness;
        std::string metallic;

        #ifdef NC_EDITOR_ENABLED
        void EditorGuiElement();
        #endif
    };

    enum class TechniqueType : uint8_t
    {
        None = 0,
        PhongAndUi = 1,
    };

    struct TextureIndices
    {
        TextureView baseColor;
        TextureView normal;
        TextureView roughness;
        TextureView metallic;
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
