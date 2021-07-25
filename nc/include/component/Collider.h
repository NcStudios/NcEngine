#pragma once

#include "component/Component.h"
#include "physics/CollisionVolumes.h"
#include "physics/LayerMask.h"
#include "graphics/Model.h"

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
        Box, Sphere, Capsule, Hull
    };

    struct SphereProperties
    {
        Vector3 center = Vector3::Zero();
        float radius = 0.5f;
    };

    struct BoxProperties
    {
        Vector3 center = Vector3::Zero();
        Vector3 extents = Vector3::One();
    };

    struct CapsuleProperties
    {
        Vector3 center = Vector3::Zero();
        float height = 2.0f;
        float radius = 0.5f;
    };

    struct HullProperties
    {
        std::string assetPath;
    };

    class Collider final : public ComponentBase
    {
        public:
            struct VolumeInfo
            {
                ColliderType type;
                Vector3 offset;
                Vector3 scale;
                std::string hullAssetPath;
                bool isTrigger;
            };

            Collider(Entity entity, SphereProperties properties, bool isTrigger);
            Collider(Entity entity, BoxProperties properties, bool isTrigger);
            Collider(Entity entity, CapsuleProperties properties, bool isTrigger);
            Collider(Entity entity, HullProperties properties, bool isTrigger);
            ~Collider() = default;
            Collider(const Collider&) = delete;
            Collider(Collider&&) = default;
            Collider& operator=(const Collider&) = delete;
            Collider& operator=(Collider&&) = default;

            void Wake() { m_awake = true; }
            void Sleep() { m_awake = false; }

            auto GetInfo() const -> const VolumeInfo& { return m_info; }
            auto GetType() const -> ColliderType { return m_info.type; };
            auto IsTrigger() const -> bool { return m_info.isTrigger; }
            auto IsAwake() const -> bool { return m_awake; }
            auto GetVolume() const -> const BoundingVolume& { return m_volume; }
            auto EstimateBoundingVolume(DirectX::FXMMATRIX matrix) const -> SphereCollider;

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidget(graphics::FrameManager* frame);
            void SetEditorSelection(bool state);
            #endif

        private:
            VolumeInfo m_info;
            BoundingVolume m_volume;
            bool m_awake;

            #ifdef NC_EDITOR_ENABLED
            /** @todo this was made to be a unique_ptr for dx11, can remove with vulkan integration */
            std::unique_ptr<graphics::Model> m_widgetModel;
            bool m_selectedInEditor;
            #endif
    };

    template<>
    struct StoragePolicy<Collider>
    {
        #ifdef NC_EDITOR_ENABLED
        using allow_trivial_destruction = std::false_type;
        #else
        using allow_trivial_destruction = std::true_type;
        #endif

        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::false_type;
        using requires_on_remove_callback = std::false_type;
    };

    const char* ToCString(nc::ColliderType type);

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<Collider>(Collider*);
    #endif
}