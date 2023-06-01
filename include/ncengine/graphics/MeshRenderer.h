#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/Component.h"

namespace nc::graphics
{
    struct PbrMaterial
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
        PhongAndUi = 1
    };

    struct PbrMaterialView
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
            MeshRenderer(Entity entity,
                         std::string meshUid,
                         PbrMaterial material,
                         TechniqueType techniqueType = TechniqueType::PhongAndUi);

            auto GetMesh() const -> const MeshView& { return m_mesh; }
            auto GetMaterialView() const -> const PbrMaterialView& { return m_materialView; }
            auto GetTechniqueType() const -> TechniqueType { return m_techniqueType; }

            void SetMesh(std::string meshUid);
            void SetBaseColor(const std::string& texturePath);
            void SetNormal(const std::string& texturePath);
            void SetRoughness(const std::string& texturePath);
            void SetMetallic(const std::string& texturePath);

            #ifdef NC_EDITOR_ENABLED
            auto GetMaterial() -> PbrMaterial& { return m_material; }
            auto GetMeshPath() const -> const std::string& { return m_meshPath; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            PbrMaterial m_material;
            std::string m_meshPath;
            #endif
            MeshView m_mesh;
            PbrMaterialView m_materialView;
            TechniqueType m_techniqueType;
    };
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::MeshRenderer>(graphics::MeshRenderer* meshRenderer);
#endif
} // namespace nc
