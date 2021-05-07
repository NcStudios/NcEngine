#include "component/ParticleEmitter.h"
#include "ecs/ParticleEmitterSystem.h"
//#include "particle/EmitterState.h"

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
} // namespace nc