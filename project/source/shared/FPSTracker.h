#pragma once

#include "component/Component.h"

namespace nc::sample
{
    class FPSTracker : public Component
    {
        public:
            FPSTracker(EntityHandle handle);
            void FrameUpdate(float dt) override;
            float GetFPS() const;

        private:
            unsigned m_frames = 0u;
            float m_seconds = 0.0f;
            float m_latestFPS = 0.0f;
    };

    inline FPSTracker::FPSTracker(EntityHandle handle)
        : Component{handle}
    {
    }

    inline void FPSTracker::FrameUpdate(float dt)
    {
        ++m_frames;
        m_seconds += dt;
        if(m_frames > 59u) // average across 1 frame
        {
            m_latestFPS = (float)m_frames / m_seconds;
            m_frames = 0u;
            m_seconds = 0.0f;
        }
    }

    inline float FPSTracker::GetFPS() const
    {
        return m_latestFPS;
    }
}