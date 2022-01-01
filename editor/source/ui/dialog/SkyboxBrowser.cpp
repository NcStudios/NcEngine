#include "SkyboxBrowser.h"
#include "ui/ImGuiUtility.h"
#include "assets/Asset.h"
#include <iostream>

namespace
{
    constexpr nc::Vector2 WindowSize{400.0f, 550.0f};
}

namespace nc::editor
{
    SkyboxBrowser::SkyboxBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback, AssetManifest* manifest)
    : DialogFixedCentered("Select Skybox Faces", WindowSize),
      m_addDialog{std::move(registerDialog)},
      m_onConfirm{},
      m_openFileBrowser{std::move(openFileBrowserCallback)},
      m_facesPaths{},
      m_previousPaths{},
      m_assetManifest{manifest},
      m_skyboxName{},
      m_isEditMode{false}
    {
    }

    void SkyboxBrowser::Open(DialogCallbacks::SkyboxOnConfirmCallbackType callback)
    {
        m_onConfirm = std::move(callback);
        m_addDialog(this);
        m_isOpen = true;
        m_facesPaths = CubeMapFaces{};
    }

    void SkyboxBrowser::Open(DialogCallbacks::SkyboxOnConfirmCallbackType callback, CubeMapFaces existingFaces)
    {
        m_onConfirm = std::move(callback);
        m_addDialog(this);
        m_isOpen = true;
        m_facesPaths = existingFaces;
        m_previousPaths = existingFaces;
        m_isEditMode = true;
    }

    void SkyboxBrowser::Draw()
    {
        if (!m_isOpen) return;

        if (BeginWindow())
        {
            if (!m_isEditMode)
            {
                InputText("Skybox name ", &m_skyboxName);
                ImGui::Spacing();
            }

            if(ImGui::BeginChild("Faces", ImVec2{WindowSize.x * 0.97f, WindowSize.y * 0.85f}, true))
            {
                ImGui::SetWindowFocus();
                SelectFace(m_facesPaths.frontPath, "Front");
                SelectFace(m_facesPaths.backPath,  "Back");
                SelectFace(m_facesPaths.upPath,    "Up");
                SelectFace(m_facesPaths.downPath,  "Down");
                SelectFace(m_facesPaths.rightPath, "Right");
                SelectFace(m_facesPaths.leftPath,  "Left");
            }
            ImGui::EndChild();

            if (ImGui::Button("Confirm"))
            {
                if (m_onConfirm(""))
                {
                    if (m_isEditMode)
                    {
                        m_skyboxName = std::filesystem::path(m_previousPaths.frontPath).parent_path().stem().string();
                        m_assetManifest->EditSkybox(m_previousPaths, m_facesPaths, m_skyboxName);
                    }
                    else
                    {
                        m_assetManifest->AddSkybox(m_facesPaths, m_skyboxName);
                    }

                    m_isOpen = false;
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                m_isOpen = false;
            }
        }

        EndWindow();
    }

    void SkyboxBrowser::SelectFace(std::string& facePath, const std::string& label)
    {
        IMGUI_SCOPE(ImGuiId, label.c_str());

        if (ImGui::Button("Edit"))
        {
            m_openFileBrowser([&facePath](const std::filesystem::path& path)
            {
                auto isValid = HasValidExtensionForAssetType(path, AssetType::Skybox);
                if (isValid)
                {
                    facePath = path.string();
                }

                return isValid;
            });
        }

        ImGui::SameLine();
        InputText(label.c_str(), &facePath, ImGuiInputTextFlags_ReadOnly);
    }
}