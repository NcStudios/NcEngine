#include "component/ParticleEmitter.h"
#include "ecs/ParticleEmitterSystem.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    ParticleEmitter::ParticleEmitter(Entity entity, ParticleInfo info)
        : ComponentBase{entity},
          m_info{info},
          m_emitterSystem{nullptr}
    {
    }

    const ParticleInfo& ParticleEmitter::GetInfo() const noexcept
    {
        return m_info;
    }

    void ParticleEmitter::Emit(size_t count)
    {
        m_emitterSystem->Emit(GetParentEntity(), count);
    }

    void ParticleEmitter::RegisterSystem(ecs::ParticleEmitterSystem* system)
    {
        m_emitterSystem = system;
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<ParticleEmitter>(ParticleEmitter*)
    {
        ImGui::Text("Particle Emitter");
    }
    #endif
} // namespace nc