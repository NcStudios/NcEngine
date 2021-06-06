#pragma once

#include "component/Component.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/TechniqueType.h"

#include <string>

namespace nc { class Transform; }

namespace nc::vulkan
{
    class MeshRenderer : public Component
    {
        public:
            MeshRenderer(EntityHandle parentHandle, std::string meshUid, nc::graphics::vulkan::Material material, Transform* transform);
            Transform* GetTransform();
            const std::string& GetMeshUid() const;
            const nc::graphics::vulkan::Material& GetMaterial() const;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
        
        private:
            Transform* m_transform;
            std::string m_meshUid;
            nc::graphics::vulkan::Material m_material;
    };
}