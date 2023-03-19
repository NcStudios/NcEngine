#pragma once

#include "DeviceStream.h"
#include "audio/NcAudio.h"
#include "ecs/Registry.h"
#include "task/Job.h"

#include <mutex>
#include <queue>

namespace nc::config { struct AudioSettings; }

namespace nc::audio
{
/** Factor to construct an audio module instance */
auto BuildAudioModule(const config::AudioSettings& settings, Registry* reg) -> std::unique_ptr<NcAudio>;

/** Audio module implementation */
class NcAudioImpl final : public NcAudio
{
    public:
        NcAudioImpl(Registry* registry);
        ~NcAudioImpl() noexcept;

        void RegisterListener(Entity listener) noexcept override;
        auto EnumerateOutputDevices() noexcept -> std::vector<AudioDevice> override;
        auto GetOutputDevice() const noexcept -> const AudioDevice& override;
        auto SetOutputDevice(uint32_t deviceId) noexcept -> bool override;

        auto BuildWorkload() -> std::vector<task::Job> override;
        void Clear() noexcept override;
        void Run();
        auto WriteToDeviceBuffer(double* output) -> int;

    private:
        Registry* m_registry;
        std::vector<double> m_bufferMemory;
        std::queue<std::span<double>> m_readyBuffers;
        std::queue<std::span<double>> m_staleBuffers;
        std::mutex m_readyMutex;
        std::mutex m_staleMutex;
        DeviceStream m_deviceStream;
        Entity m_listener;

        void MixToBuffer(double* buffer);
};
} // namespace nc::audio
