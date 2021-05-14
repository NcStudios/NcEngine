#include "component/ParticleEmitter.h"
#include "ecs/ParticleEmitterSystem.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    ParticleEmitter::ParticleEmitter(EntityHandle handle, ecs::ParticleEmitterSystem* emitterSystem)
        : Component{handle},
          m_emitterSystem{emitterSystem}
    {
    }

    void ParticleEmitter::Emit(size_t count)
    {
        m_emitterSystem->Emit(GetParentHandle(), count);
    }

    #ifdef NC_EDITOR_ENABLED
    void ParticleEmitter::EditorGuiElement()
    {
        ImGui::Text("Particle Emitter");
    }
    #endif
} // namespace nc