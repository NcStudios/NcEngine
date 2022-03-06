#pragma once

#include "audio_module.h"
#include "ecs/Registry.h"
#include "rtaudio/RtAudio.h"

#include <mutex>
#include <queue>

/** @todo remove */
#include "task/TaskGraph.h"

namespace nc::audio
{
    auto build_audio_module(Registry* reg) -> std::unique_ptr<audio_module>;

    class audio_module_impl final : public audio_module
    {
        public:
            audio_module_impl(Registry* registry);
            ~audio_module_impl() noexcept;

            void register_listener(Entity listener) noexcept override;
            void update() override;
            void clear() noexcept override;

            /** @todo fix */
            auto get_tasks() -> TaskGraph& { static TaskGraph tg; return tg; }

            int write_to_device_buffer(double* output);
            auto probe_devices() -> std::vector<RtAudio::DeviceInfo>;

        private:
            Registry* m_registry;
            RtAudio m_rtAudio;
            std::queue<std::span<double>> m_readyBuffers;
            std::queue<std::span<double>> m_staleBuffers;
            std::vector<double> m_bufferMemory;
            std::mutex m_readyMutex;
            std::mutex m_staleMutex;
            Entity m_listener;

            void mix_to_buffer(double* buffer);
    };
}