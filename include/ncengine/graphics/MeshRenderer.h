/**
 * @file MeshRenderer.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/ecs/Component.h"

#include <memory>

namespace nc::graphics
{
struct PbrMaterial
{
    std::string baseColor = asset::DefaultBaseColor;
    std::string normal = asset::DefaultNormal;
    std::string roughness = asset::DefaultRoughness;
    std::string metallic = asset::DefaultMetallic;
};

struct PbrMaterialView
{
    TextureView baseColor;
    TextureView normal;
    TextureView roughness;
    TextureView metallic;
};

enum class TechniqueType : uint8_t
{
    None = 0,
    PhongAndUi = 1
};

class MeshRenderer : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(MeshRenderer)

    public:
        static constexpr auto ComponentId = 4ull;

        MeshRenderer(Entity entity,
                     std::string meshUid = asset::CubeMesh,
                     PbrMaterial material = {},
                     TechniqueType techniqueType = TechniqueType::PhongAndUi);

        auto GetMeshView() const noexcept -> const MeshView&
        {
            return m_meshView;
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
        MeshView m_meshView;
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
