#include "NcAudioImpl.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/utility/Log.h"

#include <cstring>

namespace
{
constexpr auto g_outputChannelCount = 2u;
constexpr auto g_sampleRate = 44100u;
constexpr auto g_bufferCount = 3u;

auto IndividualBufferSize(uint32_t bufferFrames) -> uint32_t
{
    return bufferFrames * g_outputChannelCount;
}

auto BuildBufferPool(uint32_t bufferFrames) -> std::vector<double>
{
    return std::vector<double>(IndividualBufferSize(bufferFrames) * g_bufferCount, 0.0);
}

auto BuildBufferQueue(std::span<double> bufferPool) -> std::queue<std::span<double>>
{
    if (bufferPool.size() % g_bufferCount != 0)
    {
        throw nc::NcError("Invalid buffer pool size");
    }

    const auto bufferLength = bufferPool.size() / g_bufferCount;
    auto queue = std::queue<std::span<double>>{};
    for(auto i = 0ull; i < g_bufferCount; ++i)
    {
        auto begin = bufferPool.begin() + i * bufferLength;
        queue.emplace(begin, bufferLength);
    }

    return queue;
}

int AudioSystemCallback(void* outputBuffer, void*, unsigned nBufferFrames, double, unsigned, void* userData)
{
    auto* system = static_cast<nc::audio::NcAudioImpl*>(userData);
    return system->WriteToDeviceBuffer(static_cast<double*>(outputBuffer), nBufferFrames);
}

auto CreateStreamParams(uint32_t deviceId, uint32_t bufferFrames, nc::audio::NcAudioImpl* impl) -> nc::audio::StreamParameters
{
    return nc::audio::StreamParameters
    {
        deviceId,
        bufferFrames,
        g_outputChannelCount,
        g_sampleRate,
        ::AudioSystemCallback,
        static_cast<void*>(impl)
    };
}

struct NcAudioStub : public nc::audio::NcAudio
{
    nc::audio::AudioDevice nullDevice{"NoDevice", nc::audio::InvalidDeviceId};
    nc::Signal<const nc::audio::AudioDevice&> nullSignal;

    void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks&)
    {
        update.Add(
            nc::update_task_id::AudioSourceUpdate,
            "AudioSourceUpdate(stub)",
            []{}
        );
    }

    void RegisterListener(nc::Entity) noexcept override{}
    auto GetStreamTime() const noexcept -> double override { return 0.0; }
    void SetStreamTime(double) noexcept override {}
    auto EnumerateOutputDevices() noexcept -> std::vector<nc::audio::AudioDevice> override { return {}; }
    auto GetOutputDevice() const noexcept -> const nc::audio::AudioDevice& override { return nullDevice; }
    auto SetOutputDevice(uint32_t) noexcept -> bool override { return false; }
    auto OnChangeOutputDevice() noexcept -> nc::Signal<const nc::audio::AudioDevice&>& override { return nullSignal; }
};
} // anonymous namespace

namespace nc::audio
{
auto BuildAudioModule(const config::AudioSettings& settings, ecs::ExplicitEcs<Entity, Transform, AudioSource> gameState) -> std::unique_ptr<NcAudio>
{
    if(settings.enabled)
    {
        NC_LOG_TRACE("Building NcAudio module");
        return std::make_unique<NcAudioImpl>(settings, gameState);
    }

    NC_LOG_TRACE("Audio disabled - building NcAudio stub");
    return std::make_unique<NcAudioStub>();
}

NcAudioImpl::NcAudioImpl(const config::AudioSettings& settings, ecs::ExplicitEcs<Entity, Transform, AudioSource> gameState)
    : m_gameState{gameState},
      m_deviceStream{::CreateStreamParams(DefaultDeviceId, settings.bufferFrames, this)},
      m_bufferMemory(::BuildBufferPool(m_deviceStream.GetBufferFrames())),
      m_readyBuffers{},
      m_staleBuffers{::BuildBufferQueue(m_bufferMemory)},
      m_readyMutex{},
      m_staleMutex{},
      m_listener{Entity::Null()},
      m_configBufferFrames{settings.bufferFrames}
{
}

NcAudioImpl::~NcAudioImpl() noexcept
{
    Clear();
}

void NcAudioImpl::Clear() noexcept
{
    m_listener = Entity::Null();
    auto lock = std::lock_guard{m_readyMutex};
    while(!m_readyBuffers.empty())
    {
        m_staleBuffers.push(m_readyBuffers.front());
        m_readyBuffers.pop();
    }
}

void NcAudioImpl::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&)
{
    NC_LOG_TRACE("Building NcAudio Tasks");
    update.Add(
        update_task_id::AudioSourceUpdate,
        "AudioSourceUpdate",
        [this]{ Run(); }
    );
}

void NcAudioImpl::RegisterListener(Entity listener) noexcept
{
    NC_LOG_TRACE("Registering audio listener: {}", listener.Index());
    m_listener = listener;
}

auto NcAudioImpl::EnumerateOutputDevices() noexcept -> std::vector<AudioDevice>
{
    return m_deviceStream.EnumerateDevices();
}

auto NcAudioImpl::GetOutputDevice() const noexcept -> const AudioDevice&
{
    return m_deviceStream.GetDevice();
}

auto NcAudioImpl::SetOutputDevice(uint32_t deviceId) noexcept -> bool
{
    const auto result = m_deviceStream.OpenStream(::CreateStreamParams(deviceId, m_configBufferFrames, this));
    if (result)
    {
        m_outputDeviceChanged.Emit(m_deviceStream.GetDevice());
    }

    return result;
}

auto NcAudioImpl::OnChangeOutputDevice() noexcept -> Signal<const AudioDevice&>&
{
    return m_outputDeviceChanged;
}

auto NcAudioImpl::GetStreamTime() const noexcept -> double
{
    return m_deviceStream.GetStreamTime();
}

void NcAudioImpl::SetStreamTime(double time) noexcept
{
    m_deviceStream.SetStreamTime(time);
}

int NcAudioImpl::WriteToDeviceBuffer(double* output, uint32_t bufferFrames)
{
    assert(bufferFrames == m_deviceStream.GetBufferFrames());
    const auto bufferSizeInBytes = ::IndividualBufferSize(bufferFrames) * sizeof(double);
    // empty check before lock is only safe with 1 consumer
    if(m_readyBuffers.empty())
    {
        std::memset(output, 0, bufferSizeInBytes);
        return 0;
    }

    auto buffer = std::span<double>{};

    {
        std::lock_guard lock{m_readyMutex};
        buffer = m_readyBuffers.front();
        m_readyBuffers.pop();
    }

    std::memcpy(output, buffer.data(), bufferSizeInBytes);

    {
        std::lock_guard lock{m_staleMutex};
        m_staleBuffers.push(buffer);
    }

    return 0;
}

void NcAudioImpl::Run()
{
    NC_PROFILE_TASK("AudioModule", ProfileCategory::Audio);
    if(!m_listener.Valid())
    {
        return;
    }

    if (m_deviceStream.GetStreamStatus() == StreamStatus::Failed)
    {
        // TODO #376: We'd like to attempt to reopen a stream on the same device, but device persistence may not be possible
        return;
    }

    auto buffer = std::span<double>{};

    for(auto i = 0u; i < g_bufferCount; ++i)
    {
        if(m_staleBuffers.empty())
        {
            return;
        }

        {
            auto lock = std::lock_guard{m_staleMutex};
            buffer = m_staleBuffers.front();
            m_staleBuffers.pop();
        }

        MixToBuffer(buffer.data());

        {
            auto lock = std::lock_guard{m_readyMutex};
            m_readyBuffers.push(buffer);
        }
    }
}

void NcAudioImpl::MixToBuffer(double* buffer)
{
    const auto bufferFrames = m_deviceStream.GetBufferFrames();
    const auto bufferSizeInBytes = ::IndividualBufferSize(bufferFrames) * sizeof(double);
    std::memset(buffer, 0, bufferSizeInBytes);

    const auto& listenerTransform = m_gameState.Get<Transform>(m_listener);
    const auto listenerPosition = listenerTransform.Position();
    const auto rightEar = listenerTransform.Right();

    for(auto& source : m_gameState.GetAll<AudioSource>())
    {
        if(!source.IsPlaying())
        {
            continue;
        }

        if(source.IsSpatial())
        {
            auto& transform = m_gameState.Get<Transform>(source.ParentEntity());
            source.WriteSpatialSamples(buffer, bufferFrames, transform.Position(), listenerPosition, rightEar);
        }
        else
        {
            source.WriteNonSpatialSamples(buffer, bufferFrames);
        }
    }
}
} // namespace nc::audio
