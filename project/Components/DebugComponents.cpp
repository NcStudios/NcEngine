#include "DebugComponents.h"
#include "input/Input.h"
#include "SceneManager.h"
#include "scenes/MenuScene.h"
#include "scenes/DemoScene.h"

#include <chrono>
#include <iostream>

using namespace nc;
using namespace nc::input;

SceneReset::SceneReset(ComponentHandle handle, EntityHandle parentHandle)
    : Component(handle, parentHandle)
{
}

void SceneReset::FrameUpdate(float dt)
{
    (void)dt;
    if (GetKeyDown(KeyCode::F1))
    {
        nc::scene::SceneManager::ChangeScene(std::make_unique<project::MenuScene>());
    }
    if (GetKeyDown(KeyCode::F2))
    {
        nc::scene::SceneManager::ChangeScene(std::make_unique<project::DemoScene>());
    }
}

Timer::Timer(ComponentHandle handle, EntityHandle parentHandle)
    : Component(handle, parentHandle)
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