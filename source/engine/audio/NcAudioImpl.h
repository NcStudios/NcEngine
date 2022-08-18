#pragma once

#include "audio/NcAudio.h"
#include "ecs/Registry.h"
#include "rtaudio/RtAudio.h"
#include "task/Job.h"

#include <mutex>
#include <queue>

namespace nc::audio
{
/** Factor to construct an audio module instance */
auto BuildAudioModule(bool enableModule, Registry* reg) -> std::unique_ptr<NcAudio>;

/** Audio module implementation */
class NcAudioImpl final : public NcAudio
{
    public:
        NcAudioImpl(Registry* registry);
        ~NcAudioImpl() noexcept;

        void RegisterListener(Entity listener) noexcept override;
        auto BuildWorkload() -> std::vector<task::Job> override;
        void Clear() noexcept override;
        void Run();
        auto WriteToDeviceBuffer(double* output) -> int;
        auto ProbeDevices() -> std::vector<RtAudio::DeviceInfo>;

    private:
        Registry* m_registry;
        RtAudio m_rtAudio;
        std::queue<std::span<double>> m_readyBuffers;
        std::queue<std::span<double>> m_staleBuffers;
        std::vector<double> m_bufferMemory;
        std::mutex m_readyMutex;
        std::mutex m_staleMutex;
        Entity m_listener;

        void MixToBuffer(double* buffer);
};
} // namespace nc::audio
