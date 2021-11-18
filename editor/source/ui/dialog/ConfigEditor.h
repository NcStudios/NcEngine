#pragma once

#include "config/Config.h"
#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class ConfigEditor : public DialogFixedCentered
    {
        static constexpr size_t BufferSize = 128u;

        public:
            ConfigEditor();

            void Open(const std::filesystem::path& configPath);
            void Draw() override;
            void RegisterDialog(UICallbacks::RegisterDialogCallbackType registerDialog);

        private:
            config::Config m_config;
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            std::filesystem::path m_path;
            char m_nameBuffer[BufferSize];
            char m_logBuffer[BufferSize];
            char m_shadersBuffer[BufferSize];
    };
}