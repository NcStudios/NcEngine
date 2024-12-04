/**
 * @file StaticMesh.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Component.h"
#include "ncengine/graphics/Material.h"

namespace nc
{
namespace graphics
{
class MeshRendererSubsystem;
} // namespace graphics

/** @brief Component enabling rendering of an Entity with a given mesh and material. */
class StaticMesh
{
    public:
        explicit StaticMesh(Entity self,
                               const asset::MeshView& mesh,
                               const MaterialDesc& materialDesc);

        StaticMesh(StaticMesh&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_meshId{other.m_meshId},
              m_transformDataHandle{other.m_transformDataHandle},
              m_material{std::move(other.m_material)}
        {
        }

        StaticMesh& operator=(StaticMesh&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_self = std::exchange(other.m_self, Entity::Null());
                m_meshId = other.m_meshId;
                m_transformDataHandle = other.m_transformDataHandle;
                m_material = std::move(other.m_material);
            }

            return *this;
        }

        StaticMesh(const StaticMesh&) = delete;
        StaticMesh& operator=(const StaticMesh&) = delete;

        ~StaticMesh() noexcept
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

    private:
        friend class graphics::MeshRendererSubsystem;
        inline static graphics::MeshRendererSubsystem* s_subsystem = nullptr;
        Entity m_self;
        uint64_t m_meshId;
        uint32_t m_transformDataHandle;
        MaterialInstance m_material;

        void Release() noexcept;
};

template<>
struct StoragePolicy<StaticMesh> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnCommitCallbacks = false;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
