#pragma once

#include "DeviceStream.h"
#include "ncengine/audio/NcAudio.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/task/TaskGraph.h"

#include <mutex>
#include <queue>

namespace nc::audio
{
/** Audio module implementation */
class NcAudioImpl final : public NcAudio
{
    public:
        NcAudioImpl(const config::AudioSettings& settings, ecs::ExplicitEcs<Entity, Transform, AudioSource> gameState);
        ~NcAudioImpl() noexcept;

        /** NcAudio API */
        void RegisterListener(Entity listener) noexcept override;
        auto GetStreamTime() const noexcept -> double override;
        void SetStreamTime(double time) noexcept override;
        auto EnumerateOutputDevices() noexcept -> std::vector<AudioDevice> override;
        auto GetOutputDevice() const noexcept -> const AudioDevice& override;
        auto SetOutputDevice(uint32_t deviceId) noexcept -> bool override;
        auto OnChangeOutputDevice() noexcept -> Signal<const AudioDevice&>& override;

        /** Module API */
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&) override;
        void Clear() noexcept override;

        void Run();
        auto WriteToDeviceBuffer(double* output, uint32_t bufferFrames) -> int;

    private:
        ecs::ExplicitEcs<Entity, Transform, AudioSource> m_gameState;
        DeviceStream m_deviceStream;
        std::vector<double> m_bufferMemory;
        std::queue<std::span<double>> m_readyBuffers;
        std::queue<std::span<double>> m_staleBuffers;
        std::mutex m_readyMutex;
        std::mutex m_staleMutex;
        Entity m_listener;
        Signal<const AudioDevice&> m_outputDeviceChanged;
        unsigned m_configBufferFrames;

        void MixToBuffer(double* buffer);
};
} // namespace nc::audio
