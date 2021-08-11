#pragma once

#include "component/Component.h"
#include "physics/LayerMask.h"
#include "vulkan/DebugWidget.h"

#include <string>

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
                std::string assetPath;
            };

            Collider(Entity entity, SphereProperties properties);
            Collider(Entity entity, BoxProperties properties);
            Collider(Entity entity, CapsuleProperties properties);
            Collider(Entity entity, HullProperties properties);
            ~Collider() = default;
            Collider(const Collider&) = delete;
            Collider(Collider&&) = default;
            Collider& operator=(const Collider&) = delete;
            Collider& operator=(Collider&&) = default;

            const VolumeInfo& GetInfo() const;
            ColliderType GetType() const;

           #ifdef NC_EDITOR_ENABLED
            void UpdateWidget();
            void SetEditorSelection(bool state);
           #endif

        private:
            VolumeInfo m_info;

            #ifdef NC_EDITOR_ENABLED
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
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<Collider>(Collider*);
    #endif
}