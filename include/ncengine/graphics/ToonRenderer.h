#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/Component.h"

namespace nc::graphics
{
    struct ToonMaterial
    {
        std::string baseColor;
        std::string overlay;
        std::string hatching;
        uint32_t hatchingTiling;

        #ifdef NC_EDITOR_ENABLED
        void EditorGuiElement();
        #endif
    };

    struct ToonTextureIndices
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

            auto GetMesh() const -> const MeshView& { return m_mesh; }
            auto GetTextureIndices() const -> const ToonTextureIndices& { return m_textureIndices; }

            #ifdef NC_EDITOR_ENABLED
            auto GetMaterial() -> ToonMaterial& { return m_material; }
            auto GetMeshPath() const -> const std::string& { return m_meshPath; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            ToonMaterial m_material;
            std::string m_meshPath;
            #endif
            MeshView m_mesh;
            ToonTextureIndices m_textureIndices;
    };
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::ToonRenderer>(graphics::ToonRenderer* toonRenderer);
#endif
} // namespace nc
