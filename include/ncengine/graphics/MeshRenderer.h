#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/Component.h"

#include <memory>

namespace nc::graphics
{
struct PbrMaterial
{
    std::string baseColor;
    std::string normal;
    std::string roughness;
    std::string metallic;

    #ifdef NC_EDITOR_ENABLED
    void EditorGuiElement() const;
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

        auto GetMeshView() const noexcept -> const MeshView&
        {
            return m_mesh;
        }

        auto GetMaterialView() const noexcept -> const PbrMaterialView&
        {
            return m_materialView;
        }

        auto GetTechniqueType() const noexcept -> TechniqueType
        {
            return m_techniqueType;
        }

        auto GetMaterial() const noexcept -> const PbrMaterial&
        {
            return m_coldData->material;
        }

        auto GetMeshPath() const noexcept -> const std::string&
        {
            return m_coldData->meshPath;
        }

        void SetMesh(std::string meshUid);
        void SetMaterial(PbrMaterial material);
        void SetBaseColor(std::string texturePath);
        void SetNormal(std::string texturePath);
        void SetRoughness(std::string texturePath);
        void SetMetallic(std::string texturePath);

    private:
        MeshView m_mesh;
        PbrMaterialView m_materialView;
        TechniqueType m_techniqueType;

        struct MeshRendererColdData
        {
            std::string meshPath;
            PbrMaterial material;
        };

        std::unique_ptr<MeshRendererColdData> m_coldData;
};
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::MeshRenderer>(graphics::MeshRenderer* meshRenderer);
#endif
} // namespace nc
