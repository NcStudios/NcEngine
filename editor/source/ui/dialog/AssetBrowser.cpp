#include "AssetBrowser.h"
#include "utility/SkyboxEditor.h"

namespace
{
    nc::Vector2 DialogSize{700.0f, 600.0f};
}

namespace nc::editor
{
    AssetBrowser::AssetBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback, AssetManifest* manifest)
        : DialogFixedCentered{"Asset Browser", DialogSize},
          m_registerDialog{std::move(registerDialog)},
          m_openFileBrowser{std::move(openFileBrowserCallback)},
          m_assetManifest{manifest},
          m_skyboxBrowser{m_registerDialog, m_openFileBrowser, manifest} /** Pay Attention! These have already been std::moved */
    {
    }

    void AssetBrowser::Open()
    {
        m_isOpen = true;
        m_registerDialog(this);
    }

    void AssetBrowser::AssetTab(const char* label, std::span<const Asset> assets, AssetType type)
    {
        static size_t selected = 0u;

        if(ImGui::BeginTabItem(label))
        {
            ImGui::Spacing();

            if(ImGui::Button("Add"))
            {
                m_openFileBrowser([manifest = m_assetManifest, type](const std::filesystem::path& path){ return manifest->Add(path, type); });
            }

            ImGui::SameLine();

            if(ImGui::Button("Delete"))
            {
                if(selected < assets.size())
                {
                    m_assetManifest->Remove(assets[selected].sourcePath, type);
                    selected = 0u;
                }
            }

            ImGui::Spacing();

            if(ImGui::BeginChild("AssetTab", ImVec2{DialogSize.x * 0.97f, DialogSize.y * 0.85f}, true))
            {
                size_t currentIndex = 0u;

                for(const auto& asset : assets)
                {
                    bool isSelected = selected == currentIndex;
                    if(ImGui::Selectable(asset.sourcePath.string().c_str(), &isSelected))
                    {
                        selected = currentIndex;
                    }

                    ImGui::Separator();
                    ++currentIndex;
                }
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    void AssetBrowser::SkyboxAssetTab(const char* label, std::span<const Asset> assets)
    {
        static size_t selected = 0u;

        if(ImGui::BeginTabItem(label))
        {
            ImGui::Spacing();

            if(ImGui::Button("Add"))
            {
                m_skyboxBrowser.Open([](const std::filesystem::path&) { return true; });
            }

            ImGui::SameLine();

            if(ImGui::Button("Edit"))
            {
                auto fileExtension = GetFileExtensionFromNca(assets[selected].sourcePath); 
                const auto& parentPath = assets[selected].sourcePath.parent_path();

                auto cubeMapFaces = CubeMapFaces
                {
                    .frontPath = (parentPath / "front").string() + fileExtension,
                    .backPath  = (parentPath / "back").string() + fileExtension,
                    .upPath    = (parentPath / "up").string() + fileExtension,
                    .downPath  = (parentPath / "down").string() + fileExtension,
                    .rightPath = (parentPath / "right").string() + fileExtension,
                    .leftPath  = (parentPath / "left").string() + fileExtension
                };

                m_assetManifest->Remove(assets[selected].sourcePath, AssetType::Skybox);
                m_skyboxBrowser.Open([](const std::filesystem::path&) { return true; }, cubeMapFaces);
            }

            ImGui::SameLine();

            if(ImGui::Button("Delete"))
            {
                if(selected < assets.size())
                {
                    m_assetManifest->Remove(assets[selected].sourcePath, AssetType::Skybox);
                    selected = 0u;
                }
            }

            ImGui::Spacing();

            if(ImGui::BeginChild("AssetTab", ImVec2{DialogSize.x * 0.97f, DialogSize.y * 0.85f}, true))
            {
                size_t currentIndex = 0u;

                for(const auto& asset : assets)
                {
                    bool isSelected = selected == currentIndex;
                    if(ImGui::Selectable(asset.sourcePath.string().c_str(), &isSelected))
                    {
                        selected = currentIndex;
                    }

                    ImGui::Separator();
                    ++currentIndex;
                }
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    bool AssetBrowser::AddAsset(const std::filesystem::path& assetPath)
    {
        m_assetManifest->Add(assetPath, AssetType::Mesh);
        return true;
    }

    void AssetBrowser::Draw()
    {
        if(!m_isOpen) return;

        if(BeginWindow())
        {
            ImGui::SetWindowFocus();

            if(ImGui::BeginTabBar("AssetTabBar"))
            {
                AssetTab("Audio Clips",       m_assetManifest->View(AssetType::AudioClip),       AssetType::AudioClip);
                AssetTab("Concave Colliders", m_assetManifest->View(AssetType::ConcaveCollider), AssetType::ConcaveCollider);
                AssetTab("Hull Colliders",    m_assetManifest->View(AssetType::HullCollider),    AssetType::HullCollider);
                AssetTab("Meshes",            m_assetManifest->View(AssetType::Mesh),            AssetType::Mesh);
                AssetTab("Textures",          m_assetManifest->View(AssetType::Texture),         AssetType::Texture);
                SkyboxAssetTab("Skyboxes",    m_assetManifest->View(AssetType::Skybox));

                ImGui::EndTabBar();
            }
        }

        EndWindow();
    }
}