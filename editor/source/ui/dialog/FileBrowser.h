#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

#include <filesystem>
#include <functional>

namespace nc::editor
{
    class FileBrowser : public DialogFixedCentered
    {
        public:
            FileBrowser(UICallbacks::AddDialogCallbackType addDialogCallback, std::filesystem::path initialDirectory = std::filesystem::current_path());

            void Open(DialogCallbacks::FileBrowserOnConfirmCallbackType callback);
            void Draw() override;

        private:
            UICallbacks::AddDialogCallbackType m_addDialog;
            DialogCallbacks::FileBrowserOnConfirmCallbackType m_callback;
            std::filesystem::path m_directory;
            std::vector<std::string> m_parsedDirectory;
            std::vector<std::filesystem::directory_entry> m_directoryEntries;
            std::vector<std::string> m_entryStrings;
            int m_selectedIndex;

            void EnumerateDirectory();
            void SetDirectoryToSubpath(size_t endIndex);
    };
}