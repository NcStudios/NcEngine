#pragma once

#include "graphics/MeshRenderer.h"

#include <vector>

namespace nc
{
    class Registry;
}

namespace nc::graphics
{
    struct Batch
    {
        MeshView mesh;
        TextureIndices textures;
        uint32_t count;
    };

    /** Sorts MeshRenderers by mesh and material and generates batches for
     *  indirect rendering.
     *  @todo We probably want to do the transform copying (in PerFrameRenderState)
     *  within here as well as we already visit each transform for culling. */
    class MeshBatcher
    {
        public:
            MeshBatcher(Registry* registry);
            MeshBatcher(MeshBatcher&&) = delete;
            MeshBatcher(const MeshBatcher&) = delete;
            MeshBatcher& operator =(MeshBatcher&&) = delete;
            MeshBatcher& operator =(const MeshBatcher&) = delete;

            auto BuildBatches(const physics::Frustum& cameraFrustum) -> std::span<const Batch>;
            void Clear() noexcept;

        private:
            Registry* m_registry;
            std::vector<Batch> m_batches;
            bool m_isDirty;

            void Sort();
            auto AddNewBatch(const MeshRenderer& renderer) -> Batch*;
    };
}