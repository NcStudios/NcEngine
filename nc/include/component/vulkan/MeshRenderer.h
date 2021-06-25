#pragma once

#include "component/Component.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/TechniqueType.h"

#include <string>

namespace nc { class Transform; }

namespace nc::vulkan
{
    class MeshRenderer : public ComponentBase
    {
        public:
            MeshRenderer(Entity entity, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType);
            Transform* GetTransform();
            const std::string& GetMeshUid() const;
            nc::graphics::vulkan::Material& GetMaterial();
            nc::graphics::vulkan::TechniqueType GetTechniqueType() const;

        private:
            std::string m_meshUid;
            nc::graphics::vulkan::Material m_material;
            nc::graphics::vulkan::TechniqueType m_techniqueType;
    };
}

namespace nc
{
    template<>
    struct StoragePolicy<vulkan::MeshRenderer>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::MeshRenderer>(vulkan::MeshRenderer* meshRenderer);
    #endif
}