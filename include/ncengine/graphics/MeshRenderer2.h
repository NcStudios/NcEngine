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
/** @brief Component enabling rendering of an Entity with a given mesh and material. */
class MeshRenderer2 : public ComponentBase
{
    public:
        MeshRenderer2(Entity self,
                      const asset::MeshView& mesh,
                      const MaterialDesc& materialDesc)
            : ComponentBase{self},
              m_mesh{mesh},
              m_material{MaterialInstance{materialDesc}}
        {
        }

        /** @name Mesh Functions */
        auto GetMesh() const -> const asset::MeshView& { return m_mesh; }
        void SetMesh(const asset::MeshView& mesh) { m_mesh = mesh; }

        /** @name Material Functions */
        auto GetMaterial() const -> const MaterialInstance& { return m_material; }
        auto GetMaterial() -> MaterialInstance& { return m_material; }
        void SetMaterial(const MaterialDesc& materialDesc) { m_material = MaterialInstance{materialDesc}; }

    private:
        asset::MeshView m_mesh;
        MaterialInstance m_material;
};

template<>
struct StoragePolicy<MeshRenderer2> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnCommitCallbacks = false;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
