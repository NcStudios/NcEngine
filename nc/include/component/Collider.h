#pragma once

#include "component/Component.h"
#include "math/Vector3.h"
#include "physics/LayerMask.h"
#include "graphics/Model.h"
#include "directx/math/DirectXCollision.h"

#include <variant>

/**  Notes on colliders:
 * 
 *  -Colliders on static entities do not interact with other static colliders.
 * 
 *  -Colliders cannot have scale values of 0.
 * 
 *  -Sphere colliders do not support nonuniform scaling (also applies to transform scaling).
 * 
 *  -Collision bounds can be affected through changes to the parent transform, however the
 *   offset(implementation pending) and scale values specific to the collider are fixed. The editor allows tweaking
 *   these parameters for debug/visualization purposes only.
*/

namespace nc
{
    enum class ColliderType : uint8_t
    {
        Box = 0u, Sphere = 1u
    };

    struct ColliderInfo
    {
        ColliderType type = ColliderType::Box;
        Vector3 offset = Vector3::Zero();
        Vector3 scale = Vector3::One();
        physics::LayerMask mask = physics::LayerMaskAll;
    };

    class Collider final : public ComponentBase
    {
        public:
            using BoundingVolume = std::variant<DirectX::BoundingOrientedBox, DirectX::BoundingSphere>;

            Collider(EntityHandle handle, ColliderInfo info);
            ~Collider();
            Collider(const Collider&) = delete;
            Collider(Collider&&) = delete;
            Collider& operator=(const Collider&) = delete;
            Collider& operator=(Collider&&) = delete;

            ColliderType GetType() const;

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidget(graphics::FrameManager& frame);
            void EditorGuiElement() override;
            void SetEditorSelection(bool state);
            #endif

        protected:
            const ColliderType m_type;
            physics::LayerMask m_mask;

            #ifdef NC_EDITOR_ENABLED
            DirectX::FXMMATRIX m_transformMatrix;
            BoundingVolume m_boundingVolume;
            graphics::Model m_widgetModel;
            bool m_selectedInEditor;
            #endif
    };
}