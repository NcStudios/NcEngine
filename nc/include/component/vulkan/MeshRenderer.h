#pragma once

#include "component/Component.h"
#include "graphics/vulkan/PhongMaterial.h"
#include "graphics/vulkan/TechniqueType.h"

#include <string>

namespace nc { class Transform; }

namespace nc::vulkan
{
    class MeshRenderer : public Component
    {
        public:
            MeshRenderer(EntityHandle parentHandle, std::string meshUid, nc::graphics::vulkan::PhongMaterial material, Transform* transform);
            Transform* GetTransform();
            const std::string& GetMeshUid() const;
            const nc::graphics::vulkan::PhongMaterial& GetMaterial() const;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
        
        private:
            Transform* m_transform;
            std::string m_meshUid;
            nc::graphics::vulkan::PhongMaterial m_material;
    };
}