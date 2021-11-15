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
        NC_ENABLE_IN_EDITOR(MeshRenderer)
        
        public:
            MeshRenderer(Entity entity, std::string meshUid, Material material, TechniqueType techniqueType);

            auto GetMesh() const -> const MeshView& { return m_mesh; }
            auto GetTextureIndices() const -> const TextureIndices& { return m_textureIndices; }
            auto GetTechniqueType() const -> TechniqueType { return m_techniqueType; }

            void SetMesh(std::string meshUid);
            void SetBaseColor(const std::string& texturePath);
            void SetNormal(const std::string& texturePath);
            void SetRoughness(const std::string& texturePath);

            #ifdef NC_EDITOR_ENABLED
            auto GetMaterial() -> Material& { return m_material; }
            auto GetMeshPath() const -> const std::string& { return m_meshPath; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            Material m_material;
            std::string m_meshPath;
            #endif
            MeshView m_mesh;
            TextureIndices m_textureIndices;
            TechniqueType m_techniqueType;
    };
    
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}
