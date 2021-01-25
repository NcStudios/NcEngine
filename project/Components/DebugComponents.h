#pragma once

#include "nc/source/component/Component.h"

#include <memory>

class SceneReset : public nc::Component
{
    public:
        SceneReset(nc::EntityHandle handle);
        void FrameUpdate(float dt);
};

class Timer : public nc::Component
{
    public:
        Timer(nc::EntityHandle handle);
        void FrameUpdate(float dt) override;

    private:
        int m_frames = 0;
        float m_seconds = 0.0f; 
        bool m_started = false;

        void start();
        void stop();
};