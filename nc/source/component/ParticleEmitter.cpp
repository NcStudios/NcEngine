#include "component/ParticleEmitter.h"
#include "ecs/ParticleEmitterSystem.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    ParticleEmitter::ParticleEmitter(EntityHandle handle, ParticleInfo info, ecs::ParticleEmitterSystem* emitterSystem)
        : Component{handle},
          m_info{info},
          m_emitterSystem{emitterSystem}
    {
    }

    const ParticleInfo& ParticleEmitter::GetInfo() const noexcept
    {
        return m_info;
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