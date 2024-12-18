#include "ParticleSubsystem.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/time/Time.h"
#include "ncengine/debug/Profile.h"

#include <algorithm>

namespace
{
struct CameraProperties
{
    DirectX::XMVECTOR position = DirectX::g_XMZero;
    DirectX::XMVECTOR rotation = DirectX::XMQuaternionIdentity();
    DirectX::XMVECTOR forward = DirectX::g_XMIdentityR2;
};

struct PermutationData
{
    int index;
    float distance;
};

auto FindState(std::vector<nc::particle::EmitterState>& states,
               std::vector<nc::particle::EmitterState>& staged,
               nc::Entity entity)
{
    constexpr auto proj = &nc::particle::EmitterState::GetEntity;
    auto pos = std::ranges::find(states, entity, proj);
    if (pos == states.end())
    {
        pos = std::ranges::find(staged, entity, proj);
        NC_ASSERT(pos != staged.end(), "ParticleEmitter does not exist");
    }

    return pos;
}
} // anonymous namespace

namespace nc::graphics
{
ParticleSubsystem::ParticleSubsystem(ecs::Ecs world,
                    uint32_t maxParticles)
    : m_world{world},
      m_maxParticles{maxParticles}
{
    ParticleEmitter::RegisterSubsystem(this);
}

// API Facing Functions
void ParticleSubsystem::AddEmitter(graphics::ParticleEmitter& emitter)
{
    m_toAdd.emplace_back(m_world, emitter.ParentEntity(), emitter.GetInfo(), &m_random);
}

void ParticleSubsystem::RemoveEmitter(Entity entity)
{
    m_toRemove.push_back(entity);
}


void ParticleSubsystem::UpdateEmitter(graphics::ParticleEmitter& emitter)
{
    auto pos = FindState(m_emitterStates, m_toAdd, emitter.ParentEntity());
    pos->UpdateInfo(emitter.GetInfo());
}

void ParticleSubsystem::Emit(Entity entity, size_t count)
{
    auto pos = FindState(m_emitterStates, m_toAdd, entity);
    pos->Emit(count);
}

void ParticleSubsystem::Update()
{
    NC_PROFILE_TASK("ParticleSubystem::Update()", Optick::Category::VFX);
    const float dt = time::DeltaTime();
    const auto [camPosition, camRotation, camForward] = [this]()
    {
        // todo: ...
        // if (auto camera = m_getCamera())
        // {
        //     const auto& transform = m_world.Get<Transform>(camera->ParentEntity());
        //     return ::CameraProperties
        //     {
        //         .position = transform->PositionXM(),
        //         .rotation = transform->RotationXM(),
        //         .forward = transform->ForwardXM()
        //     };
        // }

        return ::CameraProperties{};
    }();

    for (auto& state : m_emitterStates)
    {
        state.Update(dt, camRotation, camForward);
    }

    SortEmitters(camPosition);

    m_particleDataHostBuffer.clear();
    for (const auto& state : m_emitterStates)
    {
        // todo: should just id store in emitter...
        const auto texture = asset::AcquireTextureAsset(state.GetTexture());
        for (const auto& m : state.GetMatrices())
        {
            m_particleDataHostBuffer.emplace_back(m, texture.index);
        }
    }
}

void ParticleSubsystem::CommitPendingChanges()
{
    NC_PROFILE_TASK("ParticleSubsystem::CommitPendingChanges()", Optick::Category::VFX);
    m_emitterStates.insert(
        m_emitterStates.cend(),
        std::make_move_iterator(m_toAdd.begin()),
        std::make_move_iterator(m_toAdd.end())
    );

    m_toAdd.clear();

    for (const auto entity : m_toRemove)
    {
        std::erase_if(m_emitterStates, [entity](auto& state){
            return state.GetEntity() == entity;
        });
    }

    m_toRemove.clear();
}

auto ParticleSubsystem::BuildState() -> ParticleRenderState
{
    const auto count = std::min(static_cast<uint32_t>(m_particleDataHostBuffer.size()), m_maxParticles); // we don't want to crash when exceeding maxParticles, just discard
    const auto updateInfo = count > 0u
        ? BufferUpdateInfo<ParticleData2>{m_particleDataHostBuffer, { {0, count} }}
        : BufferUpdateInfo<ParticleData2>{};

    return ParticleRenderState{
        .particleData = updateInfo,
        .mesh = asset::AcquireMeshAsset(asset::PlaneMesh)
    };
}

void ParticleSubsystem::Clear() noexcept
{
    m_emitterStates.clear();
    m_emitterStates.shrink_to_fit();
    m_toAdd.clear();
    m_toAdd.shrink_to_fit();
    m_toRemove.clear();
    m_toRemove.shrink_to_fit();

    // ?
    m_particleDataHostBuffer.clear();
    m_particleDataHostBuffer.shrink_to_fit();
}

void ParticleSubsystem::SortEmitters(DirectX::FXMVECTOR cameraPosition)
{
    NC_PROFILE_SCOPE("ParticleSubsystem::SortEmitters()", Optick::Category::VFX);

    // Build up an index array for sorting to help minimize number of swaps and distance calculations
    auto permutation = std::vector<PermutationData>{};
    permutation.reserve(m_emitterStates.size());
    for (auto [i, emitter] : std::views::enumerate(m_emitterStates))
    {
        const auto offsetFromCamera = DirectX::XMVectorSubtract(cameraPosition, emitter.GetLastPosition());
        const auto sqLength = DirectX::XMVector3LengthSq(offsetFromCamera);
        permutation.emplace_back(static_cast<int>(i), DirectX::XMVectorGetX(sqLength));
    }

    // Sort back to front based on distance from camera
    std::ranges::sort(permutation, std::greater<>{}, &PermutationData::distance);

    // Apply the permutation by walking cycles
    const auto emitterCount = static_cast<int>(m_emitterStates.size());
    for (int cycleStart = 0; cycleStart < emitterCount; ++cycleStart)
    {
        auto cycleCurrent = cycleStart;
        while (permutation[cycleCurrent].index >= 0)
        {
            const auto emitterIndex = permutation[cycleCurrent].index;
            if (cycleCurrent != emitterIndex && permutation[emitterIndex].index >= 0)
            {
                std::swap(m_emitterStates[cycleCurrent], m_emitterStates[emitterIndex]);
            }

            cycleCurrent = std::exchange(permutation[cycleCurrent].index, -1);
        }
    }
}
} // namespace nc::graphics
