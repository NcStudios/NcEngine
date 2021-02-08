#include "DebugComponents.h"
#include "Input.h"
#include "Scene.h"
#include "scenes/MenuScene.h"
#include "scenes/DemoScene.h"

#include <chrono>
#include <iostream>

using namespace nc;
using namespace nc::input;

SceneReset::SceneReset(EntityHandle handle)
    : Component(handle)
{
}

void SceneReset::FrameUpdate(float dt)
{
    (void)dt;

    // @todo: Remove, currently we are keeping Vulkan contained to a single dummy scene.
    #ifndef USE_VULKAN
        if (GetKeyDown(KeyCode::One))
        {
            nc::scene::Change(std::make_unique<project::MenuScene>());
        }
        if (GetKeyDown(KeyCode::Two))
        {
            nc::scene::Change(std::make_unique<project::DemoScene>());
        }
    #endif
}

Timer::Timer(EntityHandle handle)
    : Component(handle)
{
}

void Timer::FrameUpdate(float dt)
{
    if (m_started)
    {
        ++m_frames;
        m_seconds += dt;
    }

    if (GetKeyDown(KeyCode::Space))
    {
        m_started ? stop() : start();
    }
}

void Timer::start()
{
    std::cout << "\n--Starting Timer--\n";
    m_frames  = 0;
    m_seconds = 0.0f;
    m_started = true;
}

void Timer::stop()
{
    std::cout << "--Timer Results--" << '\n'
              << "-  Frames:  " << m_frames << '\n'
              << "-  Seconds: " << m_seconds << '\n'
              << "-  FPS:     " << (float)m_frames / m_seconds << '\n'
              << "--Stopping Timer--\n";
    m_started = false;
}