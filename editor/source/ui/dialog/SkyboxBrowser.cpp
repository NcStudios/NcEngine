#include "SkyboxBrowser.h"
#include "ui/ImGuiUtility.h"
#include "assets/Asset.h"

namespace
{
    constexpr nc::Vector2 WindowSize{300.0f, 500.0f};
}

namespace nc::editor
{
    SkyboxBrowser::SkyboxBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback, AssetManifest* manifest)
    : DialogFixedCentered("Select Skybox Faces", WindowSize),
      m_addDialog{std::move(registerDialog)},
      m_onConfirm{},
      m_openFileBrowser{std::move(openFileBrowserCallback)},
      m_facesPaths{},
      m_assetManifest{manifest},
      m_skyboxName{}
    {
    }

    void SkyboxBrowser::Open(DialogCallbacks::SkyboxOnConfirmCallbackType callback)
    {
        m_onConfirm = std::move(callback);
        m_addDialog(this);
        m_isOpen = true;
        m_facesPaths = CubeMapFaces{};
    }

    void SkyboxBrowser::Draw()
    {
        if (!m_isOpen) return;

        if (BeginWindow())
        {
            InputText("Skybox name: ", &m_skyboxName);
            ImGui::Spacing();

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
                    m_assetManifest->AddSkybox(m_facesPaths, m_skyboxName);
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

        InputText(label.c_str(), &facePath, ImGuiInputTextFlags_ReadOnly);
    }
}