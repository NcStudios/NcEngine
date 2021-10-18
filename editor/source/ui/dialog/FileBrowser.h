#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

#include <filesystem>
#include <functional>

namespace nc::editor
{
    class DirectoryContents
    {
        public:
            DirectoryContents(std::filesystem::path directory);

            void OpenDirectory(const std::filesystem::path& path);
            void OpenDirectory(size_t index);
            void OpenParentDirectory(size_t parsedDirectoryEndIndex);
            void SelectEntry(int index);
            void UnselectEntry();
            auto CurrentDirectory() const -> const std::filesystem::path&;
            auto CurrentDirectoryAsSubdirectoryList() const -> std::span<const std::string>;
            auto SelectedIndex() const -> int;
            auto Entry(size_t index) const -> const std::filesystem::directory_entry&;
            auto Entries() const -> std::span<const std::filesystem::directory_entry>;
            auto FormattedEntries() const -> std::span<const std::string>;
            bool IsDirectory(size_t index) const;
            bool IsFile(size_t index) const;

        private:
            std::filesystem::path m_currentDirectory;
            std::vector<std::string> m_parsedDirectory;
            std::vector<std::filesystem::directory_entry> m_directoryEntries;
            std::vector<std::string> m_entryStrings;
            int m_selectedEntryIndex;
    };

    class FileBrowser : public DialogFixedCentered
    {
        inline static constexpr size_t TextEntryBufferSize = 128;

        public:
            FileBrowser(UICallbacks::RegisterDialogCallbackType registerDialog, std::filesystem::path initialDirectory = std::filesystem::current_path());

            void Open(DialogCallbacks::FileBrowserOnConfirmCallbackType callback);
            void Draw() override;

        private:
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            DialogCallbacks::FileBrowserOnConfirmCallbackType m_callback;
            DirectoryContents m_directoryContents;
            char m_textEntryBuffer[TextEntryBufferSize];

            void DrawCurrentDirectoryWidget();
            void DrawBrowserWindow();
            void DrawFileSelectWidget();
            void ResetTextEntryBuffer();
            void SetTextEntryBuffer(const std::string& text);
    };
}