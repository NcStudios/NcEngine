#include "ParticleSubsystem.h"

#include "ncasset/DefaultAssets.h"
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

auto GetCameraProperties(nc::ecs::Ecs world, nc::Camera* mainCamera) -> CameraProperties
{
    if (mainCamera)
    {
        const auto& transform = world.Get<nc::Transform>(mainCamera->ParentEntity());
        return ::CameraProperties
        {
            .position = transform.PositionXM(),
            .rotation = transform.RotationXM(),
            .forward = transform.ForwardXM()
        };
    }

    return ::CameraProperties{};
}

struct PermutationData
{
    int index = 0;
    float distance = 0.0f;
};

auto FindState(std::vector<nc::graphics::EmitterState>& states,
               std::vector<nc::graphics::EmitterState>& staged,
               nc::Entity entity)
{
    constexpr auto proj = &nc::graphics::EmitterState::GetEntity;
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

void ParticleSubsystem::AddEmitter(ParticleEmitter& emitter)
{
    const auto& transform = m_world.Get<Transform>(emitter.GetEntity());
    m_toAdd.emplace_back(
        transform.PositionXM(),
        emitter.GetEntity(),
        emitter.GetTexture().index,
        emitter.GetInfo(),
        &m_random
    );
}

void ParticleSubsystem::RemoveEmitter(Entity entity)
{
    m_toRemove.push_back(entity);
}

void ParticleSubsystem::UpdateEmitterInfo(Entity entity, const ParticleInfo& info)
{
    FindState(m_emitterStates, m_toAdd, entity)->UpdateInfo(info);
}

void ParticleSubsystem::UpdateEmitterTexture(Entity entity, uint32_t textureIndex)
{
    FindState(m_emitterStates, m_toAdd, entity)->UpdateTexture(textureIndex);
}

void ParticleSubsystem::Emit(Entity entity, size_t count)
{
    const auto pos = m_world.Get<Transform>(entity).PositionXM();
    FindState(m_emitterStates, m_toAdd, entity)->Emit(pos, count);
}

void ParticleSubsystem::Update(Camera* mainCamera)
{
    NC_PROFILE_TASK("ParticleSubystem::Update()", ProfileCategory::VFX);
    CommitPendingChanges();
    const float dt = time::DeltaTime();
    const auto [camPosition, camRotation, camForward] = GetCameraProperties(m_world, mainCamera);
    for (auto& state : m_emitterStates)
    {
        const auto position = m_world.Get<Transform>(state.GetEntity()).PositionXM();
        state.Update(position, camRotation, camForward, dt);
    }

    SortEmitters(camPosition);
    m_particleDataHostBuffer.clear();
    for (const auto& state : m_emitterStates)
    {
        const auto textureIndex = state.GetTextureIndex();
        for (const auto& [matrix, color] : state.GetFrameData())
        {
            m_particleDataHostBuffer.emplace_back(matrix, color, textureIndex);
        }
    }
}

void ParticleSubsystem::CommitPendingChanges()
{
    NC_PROFILE_SCOPE("ParticleSubsystem::CommitPendingChanges()", ProfileCategory::VFX);
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
        ? BufferUpdateInfo<ParticleData>{m_particleDataHostBuffer, { {0, count} }}
        : BufferUpdateInfo<ParticleData>{};

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
    m_particleDataHostBuffer.clear();
    m_particleDataHostBuffer.shrink_to_fit();
}

void ParticleSubsystem::SortEmitters(DirectX::FXMVECTOR cameraPosition)
{
    NC_PROFILE_SCOPE("ParticleSubsystem::SortEmitters()", ProfileCategory::VFX);

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
