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
        Box = 0u, Sphere = 1u, Mesh = 2u
    };

    struct ColliderInfo
    {
        ColliderType type = ColliderType::Box;
        Vector3 offset = Vector3::Zero();
        Vector3 scale = Vector3::One();
    };

    struct SphereCollider
    {
        Vector3 center;
        float radius;
    };

    struct BoxCollider
    {
        Vector3 center;
        Vector3 extents;
    };

    struct MeshCollider
    {
        static std::vector<Vector3> vertices;
    };

    inline std::vector<Vector3> MeshCollider::vertices
    {
        Vector3{  0.5f,  0.5f,  0.5f },
        Vector3{  0.5f,  0.5f, -0.5f },
        Vector3{  0.5f, -0.5f,  0.5f },
        Vector3{  0.5f, -0.5f, -0.5f },
        Vector3{ -0.5f,  0.5f,  0.5f },
        Vector3{ -0.5f,  0.5f, -0.5f },
        Vector3{ -0.5f, -0.5f,  0.5f },
        Vector3{ -0.5f, -0.5f, -0.5f }
    };

    class Collider final : public ComponentBase
    {
        public:
            using BoundingVolume = std::variant<BoxCollider, SphereCollider, MeshCollider>;

            Collider(Entity entity, ColliderInfo info);
            ~Collider() = default;
            Collider(const Collider&) = delete;
            Collider(Collider&&) = default;
            Collider& operator=(const Collider&) = delete;
            Collider& operator=(Collider&&) = default;

            const ColliderInfo& GetInfo() const;
            ColliderType GetType() const;

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidget(graphics::FrameManager* frame);
            void SetEditorSelection(bool state);
            #endif

        private:
            ColliderInfo m_info;

            #ifdef NC_EDITOR_ENABLED
            /** @todo this was made to be a unique_ptr for dx11, can remove with vulkan integration */
            std::unique_ptr<graphics::Model> m_widgetModel;
            bool m_selectedInEditor;
            #endif
    };

    Collider::BoundingVolume CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale);


    template<>
    struct StoragePolicy<Collider>
    {
        #ifdef NC_EDITOR_ENABLED
        using allow_trivial_destruction = std::false_type;
        #else
        using allow_trivial_destruction = std::true_type;
        #endif

        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<Collider>(Collider*);
    #endif
}