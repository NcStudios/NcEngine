#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"
#include "assets/AssetManifest.h"

namespace nc::editor
{
    class AssetBrowser : public DialogFixedCentered
    {
        public:
            AssetBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, AssetManifest* manifest);

            void Open();
            void Draw() override;

            void RegisterCallback(DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback);

        private:
            AssetManifest* m_assetManifest;
            UICallbacks::RegisterDialogCallbackType m_registerDialog;
            DialogCallbacks::OpenFileBrowserCallbackType m_openFileBrowser;

            void AssetTab(const char* label, std::span<const Asset> assets, AssetType type);
            bool AddAsset(const std::filesystem::path& assetPath);
    };
}