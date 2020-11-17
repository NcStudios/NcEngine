#include "ApiBinder.h"

//api
#include "DebugUtils.h"
#include "Engine.h"
#include "MainCamera.h"
#include "Physics.h"
#include "SceneManager.h"
#include "UI.h"
#include "Window.h"

//impl
#include "engine/EngineImpl.h"
#include "camera/MainCameraImpl.h"
#include "debug/LogImpl.h"
#include "physics/PhysicsSystem.h"
#include "scene/SceneManagerImpl.h"
#include "ui/UIImpl.h"
#include "window/WindowImpl.h"

#include <functional>

namespace nc::engine
{
    void ApiBinder::Bind(window::WindowImpl* window,
                         scene::SceneManagerImpl* sceneManager,
                         camera::MainCameraImpl* camera,
                         debug::LogImpl* log,
                         ui::UIImpl* ui,
                         EngineImpl* engineImpl,
                         physics::PhysicsSystem* physics)
    {
        nc::Window::GetDimensions_ = std::bind(window->GetDimensions, window);
        nc::scene::SceneManager::ChangeScene_ = std::bind(sceneManager->QueueSceneChange, sceneManager, std::placeholders::_1);
        nc::camera::MainCamera::Set_ = std::bind(camera->Set, camera, std::placeholders::_1);
        nc::camera::MainCamera::GetTransform_ = std::bind(camera->GetTransform, camera);
        nc::debug::Log::LogToDiagnostics_ = std::bind(log->LogToDiagnostics, log, std::placeholders::_1);
        nc::ui::UI::Set_ = std::bind(ui->BindProjectUI, ui, std::placeholders::_1);
        nc::ui::UI::IsHovered_ = std::bind(ui->IsProjectUIHovered, ui);
        nc::engine::Engine::GetConfig_ = std::bind(engineImpl->GetConfig, engineImpl);
        nc::physics::Physics::RegisterClickable_ = std::bind(physics->RegisterClickable, physics, std::placeholders::_1);
        nc::physics::Physics::UnregisterClickable_ = std::bind(physics->UnregisterClickable, physics, std::placeholders::_1);
        nc::physics::Physics::RaycastToClickables_ = std::bind(physics->RaycastToClickables, physics, std::placeholders::_1);
    }
}