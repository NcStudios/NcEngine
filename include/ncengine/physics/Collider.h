#pragma once

#include "LayerMask.h"
#include "ncengine/ecs/Component.h"

#include "ncmath/DirectXMathUtility.h"
#include "ncmath/Geometry.h"

#include <memory>
#include <string_view>
#include <variant>

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
/** @brief The type of geometry used by a Collider. */
enum class ColliderType : uint8_t
{
    Box, Sphere, Capsule, Hull
};

/** @brief Convert a ColliderType to a std::string_view. */
auto ToString(ColliderType type) -> std::string_view;

/** @brief Parse a ColliderType from a std::string_view. */
auto FromString(std::string_view type) -> ColliderType;

/** @brief Properties for initializing a Sphere Collider. */
struct SphereProperties
{
    Vector3 center = Vector3::Zero();
    float radius = 0.5f;
};

/** @brief Properties for initializing a Box Collider. */
struct BoxProperties
{
    Vector3 center = Vector3::Zero();
    Vector3 extents = Vector3::One();
};

/** @brief Properties for initializing a Capsule Collider. */
struct CapsuleProperties
{
    Vector3 center = Vector3::Zero();
    float height = 2.0f;
    float radius = 0.5f;
};

/** @brief Properties for initializing a Hull Collider. */
struct HullProperties
{
    std::string assetPath;
};

/** @brief Normalized Collider properties. */
struct VolumeInfo
{
    ColliderType type;
    Vector3 offset;
    Vector3 scale;
};

/** @brief A variant of possible Collider geometry types. */
using BoundingVolume = std::variant<Box, Sphere, Capsule, ConvexHull>;

/** @brief Component managing collision geometry. */
class Collider final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(Collider)

    public:
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
        void SetTrigger(bool isTrigger) noexcept { m_isTrigger = isTrigger; }

        auto GetInfo() const noexcept -> const VolumeInfo& { return m_coldData->info; }
        auto GetType() const noexcept -> ColliderType { return m_coldData->info.type; }
        auto GetVolume() const noexcept -> const BoundingVolume& { return m_volume; }
        auto GetAssetPath() const noexcept -> const std::string& { return m_coldData->assetPath; }
        auto IsTrigger() const noexcept -> bool { return m_isTrigger; }
        auto IsAwake() const noexcept -> bool { return m_awake; }
        auto EstimateBoundingVolume(DirectX::FXMMATRIX matrix) const -> Sphere;

        /** @todo #446 Handle this in editor code. */
        #ifdef NC_EDITOR_ENABLED
        void SetEditorSelection(bool state);
        bool GetEditorSelection();
        graphics::DebugWidget GetDebugWidget();
        #endif

    private:
        BoundingVolume m_volume;
        bool m_isTrigger;
        bool m_awake = true;

        struct ColliderColdData
        {
            VolumeInfo info;
            std::string assetPath;
            bool selectedInEditor = false;
        };

        std::unique_ptr<ColliderColdData> m_coldData;
};
} // namespace physics
} // namespace nc
