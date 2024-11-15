/**
 * @file MeshRenderer2.h
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
class MeshRenderer2
{
    public:
        explicit MeshRenderer2(Entity self,
                               const asset::MeshView& mesh,
                               const MaterialDesc& materialDesc);

        // todo: move instance for move ops

        MeshRenderer2(MeshRenderer2&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_meshId{other.m_meshId},
              m_material{std::move(other.m_material)}
        {
        }

        MeshRenderer2& operator=(MeshRenderer2&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_self = std::exchange(other.m_self, Entity::Null());
                m_meshId = other.m_meshId;
                m_material = std::move(other.m_material);
            }

            return *this;
        }

        MeshRenderer2(const MeshRenderer2&) = delete;
        MeshRenderer2& operator=(const MeshRenderer2&) = delete;

        ~MeshRenderer2() noexcept
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

    private:
        friend class graphics::MeshRendererSubsystem;
        inline static graphics::MeshRendererSubsystem* s_subsystem = nullptr;

        Entity m_self;
        uint64_t m_meshId;
        uint32_t m_instance;
        MaterialInstance m_material;

        void Release() noexcept;
};

template<>
struct StoragePolicy<MeshRenderer2> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnCommitCallbacks = false;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
