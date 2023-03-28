#pragma once

#include "DeviceStream.h"
#include "audio/NcAudio.h"
#include "ecs/Registry.h"
#include "task/Job.h"

#include <mutex>
#include <queue>

namespace nc
{
namespace config
{
struct AudioSettings;
} // namespace config

namespace audio
{
/** Factory to construct an audio module instance */
auto BuildAudioModule(const config::AudioSettings& settings, Registry* reg) -> std::unique_ptr<NcAudio>;

/** Audio module implementation */
class NcAudioImpl final : public NcAudio
{
    public:
        NcAudioImpl(Registry* registry);
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
        auto BuildWorkload() -> std::vector<task::Job> override;
        void Clear() noexcept override;

        void Run();
        auto WriteToDeviceBuffer(double* output, uint32_t bufferFrames) -> int;

    private:
        Registry* m_registry;
        DeviceStream m_deviceStream;
        std::vector<double> m_bufferMemory;
        std::queue<std::span<double>> m_readyBuffers;
        std::queue<std::span<double>> m_staleBuffers;
        std::mutex m_readyMutex;
        std::mutex m_staleMutex;
        Entity m_listener;
        Signal<const AudioDevice&> m_outputDeviceChanged;

        void MixToBuffer(double* buffer);
};
} // namespace audio
} // namespace nc
