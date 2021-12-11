#pragma once

#include "config/Config.h"
#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class ConfigEditor : public DialogFixedCentered
    {
        public:
            ConfigEditor(const std::filesystem::path& configPath);

            void Open(const std::filesystem::path& configPath);
            void Draw() override;
            void RegisterDialog(UICallbacks::RegisterDialogCallbackType registerDialog);
            auto GetConfig() const -> const config::Config& { return m_config; }

        private:
            config::Config m_config;
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            std::filesystem::path m_path;
            std::string m_nameBuffer;
            std::string m_logPathBuffer;
            std::string m_audioClipPathBuffer;
            std::string m_concaveColliderPathBuffer;
            std::string m_hullColliderPathBuffer;
            std::string m_meshPathBuffer;
            std::string m_shaderPathBuffer;
            std::string m_texturePathBuffer;
            std::string m_cubeMapPathBuffer;
    };
}