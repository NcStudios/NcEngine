/**
 * @file Mesh.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Component.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/SkeletalAnimationController.h"

namespace nc
{
namespace graphics
{
class MeshSubsystem;
} // namespace graphics

/** @brief Type of mesh components. */
enum class MeshInstanceType : uint8_t
{
    Static,
    Skinned
};

/** @brief Mesh component state for tracking internal instance data. */
struct MeshInstanceContext
{
    Entity entity = Entity::Null();
    uint64_t meshId = std::numeric_limits<uint64_t>::max();
    uint32_t transformDataHandle = std::numeric_limits<uint32_t>::max();
    uint32_t boneDataHandle = std::numeric_limits<uint32_t>::max();
    MeshInstanceType type = MeshInstanceType::Static;
};

/** @brief Base class for mesh components. */
class MeshBase
{
    public:
        explicit MeshBase(Entity self,
                          const asset::MeshView& meshAsset,
                          const MaterialDesc& materialDesc,
                          MeshInstanceType type);

        MeshBase(MeshBase&& other) noexcept
            : m_ctx{std::exchange(other.m_ctx, MeshInstanceContext{})},
              m_material{std::move(other.m_material)}
        {
        }

        MeshBase& operator=(MeshBase&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_ctx = std::exchange(other.m_ctx, MeshInstanceContext{});
                m_material = std::move(other.m_material);
            }

            return *this;
        }

        MeshBase(const MeshBase&) = delete;
        MeshBase& operator=(const MeshBase&) = delete;

        /** @name General Functions */
        auto GetEntity() const -> Entity { return m_ctx.entity; }

        /** @name Mesh Functions */
        auto GetMeshId() const -> uint64_t { return m_ctx.meshId; }
        void SetMesh(const asset::MeshView& meshAsset);

        /** @name Material Functions */
        auto GetMaterial() const -> const MaterialInstance& { return m_material; }
        auto GetMaterial() -> MaterialInstance& { return m_material; }
        void SetMaterial(const MaterialDesc& materialDesc);

        /** @cond internal */
        static void RegisterSubsystem(graphics::MeshSubsystem* subsystem)
        {
            s_subsystem = subsystem;
        }
        /** @endcond internal */

    protected:
        ~MeshBase() noexcept
        {
            Release();
        }

    private:
        inline static graphics::MeshSubsystem* s_subsystem = nullptr;
        MeshInstanceContext m_ctx;
        MaterialInstance m_material;

        void Release() noexcept;
};

/** @brief Component enabling rendering of an Entity with a given mesh and material. */
class StaticMesh : public MeshBase
{
    public:
        explicit StaticMesh(Entity self,
                            const asset::MeshView& meshAsset,
                            const MaterialDesc& materialDesc)
            : MeshBase{self, meshAsset, materialDesc, MeshInstanceType::Static}
        {
        }
};

/** @brief Component enabling rendering of an Entity with a given mesh, material, and skeletal animation. */
class SkinnedMesh : public MeshBase
{
    public:
        explicit SkinnedMesh(Entity self,
                             const asset::MeshView& meshAsset,
                             const MaterialDesc& materialDesc,
                             uint64_t rootAnimationId = NullAnimationId)
            : MeshBase{self, meshAsset, materialDesc, MeshInstanceType::Skinned},
              m_controller{rootAnimationId}
        {
        }

        /** @name Animation Functions */
        auto GetAnimationController() -> SkeletalAnimationController& { return m_controller; }

    private:
        SkeletalAnimationController m_controller;
};
} // namespace nc
