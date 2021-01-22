#pragma once

#include "component/Transform.h"
#include "math/Vector3.h"
#include "graphics/Model.h"

namespace nc
{
    class Collider final : public ComponentBase
    {
        public:
            Collider(EntityHandle handle, Vector3 scale);
            ~Collider();
            void Update(graphics::FrameManager& frame);

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

        private:
            Transform* m_transform;
            Vector3 m_scale;
            graphics::Model m_model;
    };
}