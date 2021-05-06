#include "component/ParticleEmitter.h"
#include "particle/EmitterState.h"

namespace nc
{
    ParticleEmitter::ParticleEmitter(EntityHandle handle, particle::EmitterState* emitterState)
        : Component{handle},
          m_emitterState{emitterState}
    {
    }

    void ParticleEmitter::Emit(size_t count)
    {
        m_emitterState->Emit(count);
    }
} // namespace nc