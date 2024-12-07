/**
 * @file SkinnedMesh.h
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

/** @brief Component enabling rendering of an Entity with a given mesh, material, and skeletal animation. */
class SkinnedMesh
{
    public:
        explicit SkinnedMesh(Entity self,
                             const asset::MeshView& mesh,
                             const MaterialDesc& materialDesc,
                             const asset::SkeletalAnimationView& animation = asset::SkeletalAnimationView{});

        SkinnedMesh(SkinnedMesh&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_meshId{other.m_meshId},
              m_transformDataHandle{other.m_transformDataHandle},
              m_material{std::move(other.m_material)},
              m_boneDataHandle{other.m_boneDataHandle},
              m_controller{std::move(other.m_controller)}
        {
        }

        SkinnedMesh& operator=(SkinnedMesh&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_self = std::exchange(other.m_self, Entity::Null());
                m_meshId = other.m_meshId;
                m_transformDataHandle = other.m_transformDataHandle;
                m_material = std::move(other.m_material);
                m_boneDataHandle = other.m_boneDataHandle;
                m_controller = std::move(other.m_controller);
            }

            return *this;
        }

        SkinnedMesh(const SkinnedMesh&) = delete;
        SkinnedMesh& operator=(const SkinnedMesh&) = delete;

        ~SkinnedMesh() noexcept
        {
            Release();
        }

        /** @name General Functions */
        auto GetEntity() const -> Entity { return m_self; }

        /** @name Mesh Functions */
        auto GetMeshId() const -> uint64_t { return m_meshId; }
        void SetMesh(const asset::MeshView& mesh);

        /** @name Material Functions */
        auto GetMaterial() const -> const MaterialInstance& { return m_material; }
        auto GetMaterial() -> MaterialInstance& { return m_material; }
        void SetMaterial(const MaterialDesc& materialDesc);

        /** @name Animation Functions */
        auto GetAnimationController() -> SkeletalAnimationController& { return m_controller; }

    private:
        friend class graphics::MeshSubsystem;
        inline static graphics::MeshSubsystem* s_subsystem = nullptr;
        Entity m_self;
        uint64_t m_meshId;
        uint32_t m_transformDataHandle;
        MaterialInstance m_material;
        uint32_t m_boneDataHandle = 0;
        SkeletalAnimationController m_controller;

        void Release() noexcept;
};
} // namespace nc
