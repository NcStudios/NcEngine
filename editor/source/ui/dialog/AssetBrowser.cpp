#include "AssetBrowser.h"

namespace
{
    nc::Vector2 DialogSize{700.0f, 600.0f};
}

namespace nc::editor
{
    AssetBrowser::AssetBrowser(AssetManifest* manifest)
        : DialogFixedCentered{DialogSize},
          m_assetManifest{manifest},
          m_openFileBrowser{}
    {
    }

    void AssetBrowser::RegisterCallback(DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback)
    {
        m_openFileBrowser = std::move(openFileBrowserCallback);
    }

    void AssetBrowser::Open()
    {
        isOpen = true;
    }

    void AssetBrowser::AssetTab(const char* label, std::span<const Asset> assets, AssetType type)
    {
        static size_t selected = 0u;

        if(ImGui::BeginTabItem(label))
        {
            ImGui::Spacing();

            if(ImGui::Button("Add"))
            {
                auto callback = [type, assetManifest = m_assetManifest]
                {
                    switch(type)
                    {
                        case AssetType::AudioClip:       return std::bind(AssetManifest::Add<AssetType::AudioClip>,       assetManifest, std::placeholders::_1);
                        case AssetType::ConcaveCollider: return std::bind(AssetManifest::Add<AssetType::ConcaveCollider>, assetManifest, std::placeholders::_1);
                        case AssetType::HullCollider:    return std::bind(AssetManifest::Add<AssetType::HullCollider>,    assetManifest, std::placeholders::_1);
                        case AssetType::Mesh:            return std::bind(AssetManifest::Add<AssetType::Mesh>,            assetManifest, std::placeholders::_1);
                        case AssetType::Texture:         return std::bind(AssetManifest::Add<AssetType::Texture>,         assetManifest, std::placeholders::_1);
                    }

                    throw std::runtime_error("AssetBrowser - Add - Unknown AssetType");
                }();

                m_openFileBrowser(std::move(callback));
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

    bool AssetBrowser::AddAsset(const std::filesystem::path& assetPath)
    {
        m_assetManifest->Add<AssetType::Mesh>(assetPath);
        return true;
    }

    void AssetBrowser::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());

        if(ImGui::Begin("Asset Browser", &isOpen))
        {
            ImGui::SetWindowFocus();

            if(ImGui::BeginTabBar("AssetTabBar"))
            {
                AssetTab("Audio Clips",       m_assetManifest->GetAll<AssetType::AudioClip>(),       AssetType::AudioClip);
                AssetTab("Concave Colliders", m_assetManifest->GetAll<AssetType::ConcaveCollider>(), AssetType::ConcaveCollider);
                AssetTab("Hull Colliders",    m_assetManifest->GetAll<AssetType::HullCollider>(),    AssetType::HullCollider);
                AssetTab("Meshes",            m_assetManifest->GetAll<AssetType::Mesh>(),            AssetType::Mesh);
                AssetTab("Textures",          m_assetManifest->GetAll<AssetType::Texture>(),         AssetType::Texture);

                ImGui::EndTabBar();
            }
        }

        ImGui::End();
    }
}