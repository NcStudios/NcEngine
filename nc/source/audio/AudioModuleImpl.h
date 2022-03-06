#pragma once

#include "audio/AudioModule.h"
#include "ecs/Registry.h"
#include "rtaudio/RtAudio.h"

#include <mutex>
#include <queue>

/** @todo remove */
#include "task/TaskGraph.h"

namespace nc::audio
{
    auto BuildAudioModule(Registry* reg) -> std::unique_ptr<AudioModule>;

    class AudioModuleImpl final : public AudioModule
    {
        public:
            AudioModuleImpl(Registry* registry);
            ~AudioModuleImpl() noexcept;

            void RegisterListener(Entity listener) noexcept override;
            void Update() override;
            void Clear() noexcept override;

            /** @todo fix */
            auto GetTasks() -> TaskGraph& { static TaskGraph tg; return tg; }

            int WriteToDeviceBuffer(double* output);
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