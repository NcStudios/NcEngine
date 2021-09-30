#include "FileBrowser.h"

#include <iostream>

namespace
{
    constexpr nc::Vector2 FileBrowserSize{600.0f, 400.0f};
}

namespace nc::editor
{
    FileBrowser::FileBrowser(UICallbacks::AddDialogCallbackType addDialogCallback, std::filesystem::path initialDirectory)
        : DialogFixedCentered{FileBrowserSize},
          m_addDialog{std::move(addDialogCallback)},
          m_callback{},
          m_directory{std::move(initialDirectory)},
          m_parsedDirectory{},
          m_directoryEntries{},
          m_entryStrings{},
          m_selectedIndex{-1}
    {
    }

    void FileBrowser::Open(DialogCallbacks::FileBrowserOnConfirmCallbackType callback)
    {
        m_callback = std::move(callback);
        EnumerateDirectory();
        isOpen = true;
        m_addDialog(this);
    }

    void FileBrowser::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());

        int currentIndex = 0;

        if(ImGui::Begin("File Browser", &isOpen))
        {
            ImGui::SetWindowFocus();

            size_t subsectionIndex = 0u;
            for(const auto& subsection : m_parsedDirectory)
            {
                ImGui::SameLine();
                if(ImGui::Button(subsection.c_str()))
                {
                    SetDirectoryToSubpath(subsectionIndex);
                    EnumerateDirectory();
                    break;
                }

                ++subsectionIndex;
            }

            ImGui::Spacing();

            if(ImGui::BeginChild("", ImVec2{FileBrowserSize.x * 0.95f, FileBrowserSize.y * 0.65}, true))
            {
                for(const auto& entry : m_entryStrings)
                {
                    if(ImGui::Selectable(entry.c_str(), m_selectedIndex == currentIndex))
                    {
                        m_selectedIndex = currentIndex;
                    }

                    if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        if(m_directoryEntries.at(currentIndex).is_directory())
                        {
                            m_directory = m_directoryEntries.at(currentIndex);
                            EnumerateDirectory();
                            break;
                        }
                    }

                    ++currentIndex;
                    ImGui::Separator();
                }
            } ImGui::EndChild();

            ImGui::Spacing();
            ImGui::Spacing();

            if(m_selectedIndex == -1)
            {
                char null[1] = {'\0'};
                ImGui::InputText("", null, 1, ImGuiInputTextFlags_ReadOnly);
            }
            else
            {
                auto& sel = m_entryStrings.at(m_selectedIndex);
                ImGui::InputText("", sel.data(), sel.size(), ImGuiInputTextFlags_ReadOnly);
            }

            ImGui::Spacing();
            
            if(ImGui::Button("Open") && m_selectedIndex != -1)
            {
                if(m_callback(m_directoryEntries.at(m_selectedIndex).path()))
                    isOpen = false;
            }
            
            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
                isOpen = false;
        } ImGui::End();
    }

    void FileBrowser::EnumerateDirectory()
    {
        m_parsedDirectory.clear();
        m_directoryEntries.clear();
        m_entryStrings.clear();
        m_selectedIndex = -1;

        for(auto cur = m_directory.begin(); cur != m_directory.end(); ++cur)
        {
            m_parsedDirectory.push_back(cur->string());
        }

        for(const auto& entry : std::filesystem::directory_iterator{m_directory})
        {
            if(!entry.is_directory()) continue;
            m_directoryEntries.push_back(entry);
            m_entryStrings.push_back(std::string{"> "} + entry.path().filename().string());
        }

        for(const auto& entry : std::filesystem::directory_iterator{m_directory})
        {
            if(!entry.is_regular_file()) continue;
            m_directoryEntries.push_back(entry);
            m_entryStrings.push_back(entry.path().filename().string());
        }
    }

    void FileBrowser::SetDirectoryToSubpath(size_t endIndex)
    {
        if(endIndex >= m_parsedDirectory.size())
            throw std::runtime_error("FileBrowser::SetDirectoryToSubpath - Invalid range");

        m_directory.clear();
        for(auto cur = m_parsedDirectory.begin(); cur != m_parsedDirectory.begin() + endIndex + 1u; ++cur)
        {
            m_directory.append(*cur);
        }

        if(!m_directory.has_root_directory())
        {
            m_directory.append(std::string{std::filesystem::path::preferred_separator});
        }
    }
}