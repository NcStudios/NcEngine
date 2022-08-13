#pragma once

#include "Geometry.h"
#include "LayerMask.h"
#include "ecs/component/Component.h"
#include "math/DirectXMathUtility.h"

#include <variant>
#include <optional>

namespace nc
{
#ifdef NC_EDITOR_ENABLED
struct DebugWidget;
#endif

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

        Collider(Entity entity, SphereProperties properties, bool isTrigger);
        Collider(Entity entity, BoxProperties properties, bool isTrigger);
        Collider(Entity entity, CapsuleProperties properties, bool isTrigger);
        Collider(Entity entity, HullProperties properties, bool isTrigger);
        ~Collider() = default;
        Collider(const Collider&) = delete;
        Collider(Collider&&) = default;
        Collider& operator=(const Collider&) = delete;
        Collider& operator=(Collider&&) = default;

        void SetProperties(BoxProperties properties);
        void SetProperties(CapsuleProperties properties);
        void SetProperties(HullProperties properties);
        void SetProperties(SphereProperties properties);

        void Wake() { m_awake = true; }
        void Sleep() { m_awake = false; }

        auto GetInfo() const -> const VolumeInfo& { return m_info; }
        auto GetType() const -> ColliderType { return m_info.type; }
        auto GetVolume() const -> const BoundingVolume& { return m_volume; }
        auto IsTrigger() const -> bool { return m_info.isTrigger; }
        auto IsAwake() const -> bool { return m_awake; }
        auto EstimateBoundingVolume(DirectX::FXMMATRIX matrix) const -> Sphere;

        #ifdef NC_EDITOR_ENABLED
        void SetEditorSelection(bool state);
        bool GetEditorSelection();
        DebugWidget GetDebugWidget();
        #endif

    private:
        VolumeInfo m_info;
        BoundingVolume m_volume;
        bool m_awake;

        #ifdef NC_EDITOR_ENABLED
        bool m_selectedInEditor;
        friend void nc::ComponentGuiElement<Collider>(Collider*);
        #endif
};

const char* ToCString(ColliderType type);
} // namespace physics

#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<physics::Collider>(physics::Collider*);
#endif
} // namespace nc
