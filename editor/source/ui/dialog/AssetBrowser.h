#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"
#include "assets/AssetManifest.h"
#include "SkyboxBrowser.h"

namespace nc::editor
{
    class AssetBrowser : public DialogFixedCentered
    {
        public:
            AssetBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback, AssetManifest* manifest);

            void Open();
            void Draw() override;

        private:
            UICallbacks::RegisterDialogCallbackType m_registerDialog;
            DialogCallbacks::OpenFileBrowserCallbackType m_openFileBrowser;
            AssetManifest* m_assetManifest;
            SkyboxBrowser m_skyboxBrowser;

            void AssetTab(const char* label, std::span<const Asset> assets, AssetType type);
            void SkyboxAssetTab(const char* label, std::span<const Asset> assets);
            bool AddAsset(const std::filesystem::path& assetPath);
    };
}