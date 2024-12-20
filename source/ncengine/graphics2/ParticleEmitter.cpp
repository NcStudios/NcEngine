#include "ncengine/graphics/ParticleEmitter.h"
// #include "graphics/system/ParticleEmitterSystem.h"

namespace nc::graphics
{
ParticleEmitter::ParticleEmitter(Entity entity, ParticleInfo info)
    : ComponentBase{entity},
      m_info{info},
      m_emitterSystem{nullptr}
{
}

void ParticleEmitter::SetInfo(const ParticleInfo&)
{
    // m_info = info;
    // return m_emitterSystem->UpdateInfo(*this);
}

void ParticleEmitter::Emit(size_t)
{
    // m_emitterSystem->Emit(ParentEntity(), count);
}

void ParticleEmitter::RegisterSystem(ParticleEmitterSystem*)
{
    // m_emitterSystem = system;
}
} // namespace nc::graphics
