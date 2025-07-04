#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Mesh.h"

namespace nc::graphics
{

// Gets the index of the current animation to pass to the shader.
class ShapeKeyAnimationSubsystem
{
    public:

    private:
        AnimationStateOrchestrator<StaticMesh> m_staticStateOrchestrator;
        AnimationStateOrchestrator<SkinnedMesh> m_skinnedStateOrchestrator;
};
}