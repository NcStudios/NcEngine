#include "RenderingSystem.h"
#include "Graphics.h"
#include "Transform.h"
#include "ProjectSettings.h"
#include "NCTime.h"

#include "Drawable.h"

#include "Camera.h"
#include "PointLight.h"
#include "EditorManager.h"
#include "Input.h"


#include "NCE.h"
#include "imgui.h"

namespace nc::engine{

RenderingSystem::RenderingSystem(int initialWidth, int initialHeight, HWND hwnd)
    : m_graphics(std::make_unique<graphics::Graphics>(hwnd, initialWidth, initialHeight))
{
    //make light
    m_pointLight = std::make_unique<graphics::PointLight>(GetGraphics(), DirectX::XMFLOAT3(0,0,0));

    m_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)initialHeight / (float)initialWidth, 0.2f, 80.0f));
}

RenderingSystem::~RenderingSystem() = default;

void RenderingSystem::StartRenderCycle(const std::vector<Transform> &transforms)
{
    static float speed_factor = 1.0f;

    if(input::GetKeyUp(input::KeyCode::Tilde))
    {
        m_editorManager->ToggleGui();
    }
    
    //auto camMatrixXM = NCE::GetMainCamera()->Transform()->GetLookMatrix();
    auto camMatrixXM = NCE::GetMainCamera()->Transform()->CamGetMatrix();
    //auto camMatrixXM = NCE::GetMainCamera()->Transform()->GetMatrixXM();


    m_graphics->SetCamera(camMatrixXM);

    m_pointLight->Bind(GetGraphics(), camMatrixXM);

    m_editorManager->BeginFrame();
    BeginFrame();

    auto dt = nc::time::Time::FrameDeltaTime * speed_factor / 2.0f;
    auto& graphics = GetGraphics();

    for(auto& r : m_components)
    {
        r.Update(graphics, dt);
    }

    if(m_editorManager->IsGuiActive())
    {
        m_editorManager->SpeedControl(&speed_factor);

        //Transform* camT = NCE::GetMainCamera()->Transform();

        utils::editor::EditorManager::CameraControl();
        m_pointLight->SpawnControlWindow();

        if(ImGui::Begin("Boxes"))
        {
            const auto preview = m_comboBoxIndex ? std::to_string(*m_comboBoxIndex) : "Choose a box...";
            if(ImGui::BeginCombo("Box Number", preview.c_str()))
            {
                for(size_t i = 0; i < m_components.size(); ++i)
                {
                    const bool selected = *m_comboBoxIndex == i;
                    if(ImGui::Selectable(std::to_string(i).c_str(), selected))
                    {
                        m_comboBoxIndex = i;
                    }
                    if(selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if(ImGui::Button("Spawn Control Window") && m_comboBoxIndex)
            {
                m_boxControlIds.insert(*m_comboBoxIndex);
                m_comboBoxIndex.reset();
            }
        }
        ImGui::End();

        for(auto id : m_boxControlIds)
        {
            m_components[id].SpawnControlWindow(id, graphics);
        }
    }

    m_editorManager->EndFrame();
    EndFrame();
}

nc::graphics::Graphics& RenderingSystem::GetGraphics()
{
    return *m_graphics;
}

void RenderingSystem::BindEditorManager(utils::editor::EditorManager* editorManager)
{
    m_editorManager = editorManager;
}

void RenderingSystem::BeginFrame()
{
    m_graphics->ClearBuffer(0.0f, 0.0f, 0.0f);
}

void RenderingSystem::EndFrame()
{
    m_graphics->EndFrame();
}

} //end namespace nc::internal