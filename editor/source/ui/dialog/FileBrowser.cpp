#include "FileBrowser.h"

#include <cstring>
#include <iostream>

namespace
{
    constexpr nc::Vector2 FileBrowserSize{600.0f, 400.0f};
    const ImVec2 BrowserWindowSize{FileBrowserSize.x * 0.95f, FileBrowserSize.y * 0.65};
}

namespace nc::editor
{
    DirectoryContents::DirectoryContents(std::filesystem::path directory)
        : m_currentDirectory{std::move(directory)},
          m_parsedDirectory{},
          m_directoryEntries{},
          m_entryStrings{},
          m_selectedEntryIndex{-1}
    {
    }

    void DirectoryContents::OpenDirectory(const std::filesystem::path& path)
    {
        m_currentDirectory = path;
        m_selectedEntryIndex = -1;
        m_parsedDirectory.clear();
        m_directoryEntries.clear();
        m_entryStrings.clear();

        for(auto cur = m_currentDirectory.begin(); cur != m_currentDirectory.end(); ++cur)
        {
            m_parsedDirectory.push_back(cur->string());
        }

        for(const auto& entry : std::filesystem::directory_iterator{m_currentDirectory})
        {
            if(!entry.is_directory()) continue;
            m_directoryEntries.push_back(entry);
            m_entryStrings.push_back(std::string{"> "} + entry.path().filename().string());
        }

        for(const auto& entry : std::filesystem::directory_iterator{m_currentDirectory})
        {
            if(!entry.is_regular_file()) continue;
            m_directoryEntries.push_back(entry);
            m_entryStrings.push_back(entry.path().filename().string());
        }
    }

    void DirectoryContents::OpenDirectory(size_t index)
    {
        OpenDirectory(m_directoryEntries.at(index).path());
    }

    void DirectoryContents::OpenParentDirectory(size_t parsedDirectoryEndIndex)
    {
        if(parsedDirectoryEndIndex >= m_parsedDirectory.size())
            throw std::runtime_error("FileBrowser::SetDirectoryToSubpath - Invalid range");

        std::filesystem::path directory;

        for(auto cur = m_parsedDirectory.begin(); cur != m_parsedDirectory.begin() + parsedDirectoryEndIndex + 1u; ++cur)
        {
            directory.append(*cur);
        }

        if(!directory.has_root_directory())
        {
            directory.append(std::string{std::filesystem::path::preferred_separator});
        }

        OpenDirectory(directory);
    }

    void DirectoryContents::SelectEntry(int index)
    {
        m_selectedEntryIndex = index;
    }
    
    void DirectoryContents::UnselectEntry()
    {
        m_selectedEntryIndex = -1;
    }

    auto DirectoryContents::CurrentDirectory() const -> const std::filesystem::path&
    {
        return m_currentDirectory;
    }

    auto DirectoryContents::CurrentDirectoryAsSubdirectoryList() const -> std::span<const std::string>
    {
        return m_parsedDirectory;
    }

    auto DirectoryContents::SelectedIndex() const -> int
    {
        return m_selectedEntryIndex;
    }

    auto DirectoryContents::Entry(size_t index) const -> const std::filesystem::directory_entry&
    {
        return m_directoryEntries.at(index);
    }

    auto DirectoryContents::Entries() const -> std::span<const std::filesystem::directory_entry>
    {
        return m_directoryEntries;
    }

    auto DirectoryContents::FormattedEntries() const -> std::span<const std::string>
    {
        return m_entryStrings;
    }

    bool DirectoryContents::IsDirectory(size_t index) const
    {
        return m_directoryEntries.at(index).is_directory();
    }

    bool DirectoryContents::IsFile(size_t index) const
    {
        return m_directoryEntries.at(index).is_regular_file();
    }

    FileBrowser::FileBrowser(UICallbacks::AddDialogCallbackType addDialogCallback, std::filesystem::path initialDirectory)
        : DialogFixedCentered{FileBrowserSize},
          m_addDialog{std::move(addDialogCallback)},
          m_callback{},
          m_directoryContents{std::move(initialDirectory)}
    {
    }

    void FileBrowser::Open(DialogCallbacks::FileBrowserOnConfirmCallbackType callback)
    {
        m_callback = std::move(callback);
        isOpen = true;
        m_addDialog(this);
        ResetTextEntryBuffer();
        m_directoryContents.OpenDirectory(m_directoryContents.CurrentDirectory());
    }

    void FileBrowser::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());

        if(ImGui::Begin("File Browser", &isOpen))
        {
            ImGui::SetWindowFocus();
            DrawCurrentDirectoryWidget();
            ImGui::Spacing();
            DrawBrowserWindow();
            ImGui::Spacing();
            ImGui::Spacing();
            DrawFileSelectWidget();
        }
        
        ImGui::End();
    }

    void FileBrowser::DrawCurrentDirectoryWidget()
    {
        size_t subsectionIndex = 0u;
        for(const auto& subsection : m_directoryContents.CurrentDirectoryAsSubdirectoryList())
        {
            ImGui::SameLine();
            if(ImGui::Button(subsection.c_str()))
            {
                ResetTextEntryBuffer();
                m_directoryContents.OpenParentDirectory(subsectionIndex);
                break;
            }

            ++subsectionIndex;
        }
    }

    void FileBrowser::DrawBrowserWindow()
    {
        if(ImGui::BeginChild("", BrowserWindowSize, true))
        {
            int currentIndex = 0;
            int selectedIndex = m_directoryContents.SelectedIndex();

            for(const auto& entry : m_directoryContents.FormattedEntries())
            {
                if(ImGui::Selectable(entry.c_str(), selectedIndex == currentIndex))
                {
                    auto name = m_directoryContents.Entry(currentIndex).path().filename().string();
                    SetTextEntryBuffer(name);
                    m_directoryContents.SelectEntry(currentIndex);
                }

                if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    if(m_directoryContents.IsDirectory(currentIndex))
                    {
                        ResetTextEntryBuffer();
                        m_directoryContents.OpenDirectory(currentIndex);
                        break;
                    }
                }

                ++currentIndex;
                ImGui::Separator();
            }
        }
        
        ImGui::EndChild();
    }

    void FileBrowser::DrawFileSelectWidget()
    {
        ImGui::Text("File Name");
        ImGui::SameLine();
        ImGui::InputText("", m_textEntryBuffer, TextEntryBufferSize, ImGuiInputTextFlags_ReadOnly);
        ImGui::Spacing();
        
        if(ImGui::Button("Ok"))
        {
            int selectedIndex = m_directoryContents.SelectedIndex();

            if(selectedIndex == -1)
            {
                auto path = m_directoryContents.CurrentDirectory() / std::filesystem::path{m_textEntryBuffer};
                if(m_callback(path))
                    isOpen = false;
            }
            else
            {
                if(m_callback(m_directoryContents.Entry(selectedIndex)))
                    isOpen = false;
            }
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel"))
        {
            isOpen = false;
        }
    }

    void FileBrowser::ResetTextEntryBuffer()
    {
        std::memset(m_textEntryBuffer, '\0', TextEntryBufferSize);
    }

    void FileBrowser::SetTextEntryBuffer(const std::string& text)
    {
        ResetTextEntryBuffer();
        auto textLength = text.size();
        std::memcpy(m_textEntryBuffer, text.c_str(), textLength >= TextEntryBufferSize ? TextEntryBufferSize : textLength);
    }
}