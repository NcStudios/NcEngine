#pragma once
#include "Dialog.h"
#include "framework/Callbacks.h"
#include "assets/AssetUtilities.h"
#include "assets/AssetManifest.h"

namespace nc::editor
{
    class SkyboxBrowser : public DialogFixedCentered
    {
        public: 
            SkyboxBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, DialogCallbacks::OpenFileBrowserCallbackType openFileBrowserCallback, AssetManifest* manifest);

            void Open(DialogCallbacks::SkyboxOnConfirmCallbackType callback);
            void Open(DialogCallbacks::SkyboxOnConfirmCallbackType callback, CubeMapFaces existingFaces);
            void Draw() override;

        private:
            void SelectFace(std::string& facePath, const std::string& label);

            UICallbacks::RegisterDialogCallbackType m_addDialog;
            DialogCallbacks::SkyboxOnConfirmCallbackType m_onConfirm;
            DialogCallbacks::OpenFileBrowserCallbackType m_openFileBrowser;
            CubeMapFaces m_facesPaths;
            CubeMapFaces m_previousPaths;
            AssetManifest* m_assetManifest;
            std::string m_skyboxName;
            bool m_isEditMode;
    };
}