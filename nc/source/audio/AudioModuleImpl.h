#pragma once

#include "audio/AudioModule.h"
#include "ecs/Registry.h"
#include "rtaudio/RtAudio.h"
#include "task/Job.h"

#include <mutex>
#include <queue>

namespace nc::audio
{
    auto BuildAudioModule(Registry* reg) -> std::unique_ptr<AudioModule>;

    class AudioModuleImpl final : public AudioModule
    {
        public:
            AudioModuleImpl(Registry* registry);
            ~AudioModuleImpl() noexcept;

            void RegisterListener(Entity listener) noexcept override;
            auto BuildWorkload() -> std::vector<Job> override;
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
}