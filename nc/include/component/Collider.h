#pragma once

#include "component/Component.h"
#include "math/Vector3.h"
#include "graphics/Model.h"

#include <variant>

/**  Notes on colliders:
 * 
 *  -Colliders cannot have scale values of 0.
 * 
 *  -Sphere colliders do not support nonuniform scaling (also applies to transform scaling).
 * 
 *  -Collision bounds can be affected through changes to the parent transform, however the
 *   offset(implementation pending) and scale values specific to the collider are fixed. The editor allows tweaking
 *   these parameters for debug/visualization purposes only.
*/

namespace DirectX
{
    struct BoundingOrientedBox;
    struct BoundingSphere;
}

namespace nc
{
    enum class ColliderType : uint8_t
    {
        Box = 0u, Sphere = 1u
    };

    class Collider final : public ComponentBase
    {
        public:
            using BoundingVolume = std::variant<DirectX::BoundingOrientedBox, DirectX::BoundingSphere>;

            Collider(EntityHandle handle, ColliderType type, Vector3 scale);
            ~Collider();

            ColliderType GetType() const;
            BoundingVolume GetBoundingVolume() const;

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidget(graphics::FrameManager& frame);
            void EditorGuiElement() override;
            void SetEditorSelection(bool state);
            #endif

        protected:
            DirectX::FXMMATRIX m_transformMatrix;
            /** @todo test performance of BoundingVolume vs unique_ptr<BoundingVolume> */
            std::unique_ptr<BoundingVolume> m_boundingVolume;
            const ColliderType m_type;

            #ifdef NC_EDITOR_ENABLED
            graphics::Model m_widgetModel;
            Vector3 m_scale;
            bool m_selectedInEditor;
            #endif
    };
}