#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/Component.h"

#include <memory>

namespace nc::graphics
{
struct ToonMaterial
{
    std::string baseColor;
    std::string overlay;
    std::string hatching;
    uint32_t hatchingTiling;
};

struct ToonMaterialView
{
    TextureView baseColor;
    TextureView overlay;
    TextureView hatching;
    uint32_t hatchingTiling;
};

class ToonRenderer : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(ToonRenderer)

    public:
        ToonRenderer(Entity entity,
                     std::string meshUid,
                     ToonMaterial material);

        auto GetMeshView() const noexcept -> const MeshView&
        {
            return m_meshView;
        }

        auto GetMaterialView() const noexcept -> const ToonMaterialView&
        {
            return m_materialView;
        }

        auto GetMaterial() const noexcept -> const ToonMaterial&
        {
            return m_coldData->material;
        }

        auto GetMeshPath() const noexcept -> const std::string&
        {
            return m_coldData->meshPath;
        }

        void SetMesh(std::string meshUid);
        void SetMaterial(ToonMaterial material);
        void SetBaseColor(std::string texturePath);
        void SetOverlay(std::string texturePath);
        void SetHatching(std::string texturePath);
        void SetHatchingTiling(uint32_t tiling);

    private:
        MeshView m_meshView;
        ToonMaterialView m_materialView;

        struct ToonRendererColdData
        {
            std::string meshPath;
            ToonMaterial material;
        };

        std::unique_ptr<ToonRendererColdData> m_coldData;
};
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::ToonRenderer>(graphics::ToonRenderer* toonRenderer);
#endif
} // namespace nc
