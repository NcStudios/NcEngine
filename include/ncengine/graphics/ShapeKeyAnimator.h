/**
 * @file ShapeKeyAnimator.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */

 #pragma once

 #include "ncengine/ecs/Component.h"
 #include "ncengine/graphics/AnimationController.h"
 #include "ncengine/graphics/Mesh.h"

 #include "ncasset/Assets.h"

 namespace nc
 {

/** @brief Component enabling shape key animation on a mesh. */
class ShapeKeyAnimator
{
    public:
        explicit ShapeKeyAnimator(Entity ,
                                  MeshInstanceContext& meshContext,
                                  const asset::ShapeKeyAnimation& ,
                                  asset::AssetId rootAnimationId = asset::NullAssetId)
            : m_ctx{meshContext},
              m_controller{rootAnimationId}{}

        ShapeKeyAnimator(const ShapeKeyAnimator& other) = delete;
        ShapeKeyAnimator& operator=(const ShapeKeyAnimator& other) = delete;

        ShapeKeyAnimator(ShapeKeyAnimator&& other) noexcept
            : m_ctx{other.m_ctx}, 
            m_controller{std::move(other.m_controller)} 
        {
        }

        ShapeKeyAnimator& operator=(ShapeKeyAnimator&& other) noexcept
        {
            if (this != &other)
            {
                m_ctx = other.m_ctx;
                m_controller = std::move(other.m_controller);
            }
            return *this;
        }

        auto GetEntity() const -> Entity { return m_ctx.entity; }
        auto GetMeshContext() -> MeshInstanceContext& { return m_ctx; }
        auto GetAnimationController() ->  AnimationController& { return m_controller; }

    private:
        MeshInstanceContext& m_ctx;
        AnimationController m_controller;
};
} // namespace nc