#pragma once

#include "LayerMask.h"
#include "ncengine/ecs/Component.h"

#include "ncmath/DirectXMathUtility.h"
#include "ncmath/Geometry.h"

#include <variant>
#include <optional>

namespace nc
{
namespace graphics
{
#ifdef NC_EDITOR_ENABLED
struct DebugWidget;
#endif
}

namespace physics
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

using BoundingVolume = std::variant<Box, Sphere, Capsule, ConvexHull>;

class Collider final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(Collider)

    public:
        struct VolumeInfo
        {
            ColliderType type;
            Vector3 offset;
            Vector3 scale;
            std::string hullAssetPath;
            bool isTrigger;
        };

        Collider(Entity entity, SphereProperties properties, bool isTrigger = false);
        Collider(Entity entity, BoxProperties properties, bool isTrigger = false);
        Collider(Entity entity, CapsuleProperties properties, bool isTrigger = false);
        Collider(Entity entity, HullProperties properties, bool isTrigger = false);
        ~Collider() = default;
        Collider(const Collider&) = delete;
        Collider(Collider&&) = default;
        Collider& operator=(const Collider&) = delete;
        Collider& operator=(Collider&&) = default;

        void SetProperties(BoxProperties properties);
        void SetProperties(CapsuleProperties properties);
        void SetProperties(HullProperties properties);
        void SetProperties(SphereProperties properties);

        void Wake() noexcept { m_awake = true; }
        void Sleep() noexcept { m_awake = false; }

        auto GetInfo() const noexcept -> const VolumeInfo& { return m_info; }
        auto GetType() const noexcept -> ColliderType { return m_info.type; }
        auto GetVolume() const noexcept -> const BoundingVolume& { return m_volume; }
        auto IsTrigger() const noexcept -> bool { return m_info.isTrigger; }
        auto IsAwake() const noexcept -> bool { return m_awake; }
        auto EstimateBoundingVolume(DirectX::FXMMATRIX matrix) const -> Sphere;

        /** @todo #446 Handle this in editor code. */
        #ifdef NC_EDITOR_ENABLED
        void SetEditorSelection(bool state);
        bool GetEditorSelection();
        graphics::DebugWidget GetDebugWidget();
        #endif

    private:
        VolumeInfo m_info;
        BoundingVolume m_volume;
        bool m_awake;

        #ifdef NC_EDITOR_ENABLED
        bool m_selectedInEditor;
        #endif
};

const char* ToCString(ColliderType type);
} // namespace physics
} // namespace nc
