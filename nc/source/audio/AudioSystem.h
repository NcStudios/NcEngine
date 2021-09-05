#pragma once

#include "Ecs.h"
#include "rtaudio/RtAudio.h"

namespace nc::audio
{
    class AudioSystem
    {
        public:
            AudioSystem(registry_type* registry);
            ~AudioSystem();

            int WriteCallback(double* output, unsigned bufferFrames, RtAudioStreamStatus status);
            auto ProbeDevices() -> std::vector<RtAudio::DeviceInfo>;

        private:
            registry_type* m_registry;
            RtAudio m_rtAudio;
    };
}