#include "BuildNca.h"
#include "platform/win32/NcWin32.h"

#include <iostream>

namespace
{
    const std::string ExeName{"tools/asset_builder/build.exe"};

    std::string BuildAssetTypeArg(nc::editor::AssetType type)
    {
        if(type == nc::editor::AssetType::Mesh)
            return std::string{" -a mesh "};
        if(type == nc::editor::AssetType::ConcaveCollider)
            return std::string{" -a concave-collider "};
        if(type == nc::editor::AssetType::HullCollider)
            return std::string{" -a hull-collider "};

        return std::string{};
    }
}

namespace nc::editor
{
    bool BuildNcaFile(const std::filesystem::path& path, AssetType type)
    {
        STARTUPINFOA startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

        auto outPath = path;
        outPath.remove_filename();

        std::string cmdLine = ExeName + BuildAssetTypeArg(type) + std::string{" -t "} + path.string() + std::string{" -o "} + outPath.string();

        auto result = CreateProcessA
        (
            TEXT(ExeName.data()),
            TEXT(cmdLine.data()),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &startupInfo,
            &processInfo
        );

        if(!result)
        {
            std::cerr << "BuildNcaFile - error: " << result << '\n';
            return false;
        }
        else
        {
            WaitForSingleObject(processInfo.hProcess, INFINITE);
            DWORD exitCode;
            auto result = GetExitCodeProcess(processInfo.hProcess, &exitCode);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);

            // return false? pretty sure cmd finished
            if(!result)
            {
                std::cerr << "BuildNcaFile - error: couldn't get exit cod\n";
                return false;
            }
        }
        
        return true;
    }
}