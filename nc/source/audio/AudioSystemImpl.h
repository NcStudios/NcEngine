#pragma once

#include "AudioSystem.h"
#include "ecs/Registry.h"
#include "rtaudio/RtAudio.h"

#include <mutex>
#include <queue>

namespace nc::audio
{
    class AudioSystemImpl final : public AudioSystem
    {
        public:
            AudioSystemImpl(Registry* registry);
            ~AudioSystemImpl() noexcept;

            void RegisterListener(Entity listener) noexcept override;
            int WriteToDeviceBuffer(double* output);
            void Update();
            auto ProbeDevices() -> std::vector<RtAudio::DeviceInfo>;
            void Clear() noexcept;

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