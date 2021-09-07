#pragma once

#include "component/Component.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "graphics/TechniqueType.h"
#include "graphics/Texture.h"

#include <string>

namespace nc { class Transform; }

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
            auto GetMaterial() const -> const graphics::Material& { return m_material; }
            #endif

        private:
            #ifdef NC_EDITOR_ENABLED
            graphics::Material m_material;
            #endif
            graphics::Mesh m_mesh;
            graphics::TextureIndices m_textureIndices;
            graphics::TechniqueType m_techniqueType;
    };
}

namespace nc
{
    template<>
    struct StoragePolicy<MeshRenderer>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<MeshRenderer>(MeshRenderer* meshRenderer);
    #endif
}