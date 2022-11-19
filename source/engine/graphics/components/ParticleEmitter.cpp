#include "ecs/ParticleEmitterSystem.h"
#include "graphics/ParticleEmitter.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc::graphics
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
    m_emitterSystem->Emit(ParentEntity(), count);
}

void ParticleEmitter::RegisterSystem(ecs::ParticleEmitterSystem* system)
{
    m_emitterSystem = system;
}
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::ParticleEmitter>(graphics::ParticleEmitter*)
{
    ImGui::Text("Particle Emitter");
}
#endif
} // namespace nc
