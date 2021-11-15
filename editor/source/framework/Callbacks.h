#pragma once

#include "ecs/Entity.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace nc::editor
{
    class DialogBase;

    struct ProjectCallbacks
    {
        using CreateProjectCallbackType = std::function<void()>;
        using OpenProjectCallbackType = std::function<void()>;

        CreateProjectCallbackType createProject;
        OpenProjectCallbackType openProject;
    };

    struct SceneCallbacks
    {
        using NewSceneCallbackType = std::function<void()>;
        using SaveSceneCallbackType = std::function<void()>;
        using ChangeSceneCallbackType = std::function<void(const std::string&)>;
        using DeleteCurrentSceneCallbackType = std::function<void()>;

        NewSceneCallbackType newScene;
        SaveSceneCallbackType saveScene;
        ChangeSceneCallbackType changeScene;
        DeleteCurrentSceneCallbackType deleteCurrentScene;
    };

    struct EntityCallbacks
    {
        using ChangeTagCallbackType = std::function<void(Entity)>;

        ChangeTagCallbackType changeTag;
    };

    struct UICallbacks
    {
        using RegisterDialogCallbackType = std::function<void(DialogBase*)>;
        using UpdateScenesCallbackType = std::function<void(std::vector<std::string>, int)>;
        using SetProjectNameCallbackType = std::function<void(std::string)>;

        RegisterDialogCallbackType registerDialog;
        UpdateScenesCallbackType updateScenes;
        SetProjectNameCallbackType setProjectName;
    };

    struct DialogCallbacks
    {
        using FileBrowserOnConfirmCallbackType = std::function<bool(const std::filesystem::path&)>;
        using NewSceneOnConfirmCallbackType = std::function<bool(const std::string&)>;
        using NewProjectOnConfirmCallbackType = std::function<bool(const std::string&, const std::filesystem::path&)>;

        using OpenFileBrowserCallbackType = std::function<void(FileBrowserOnConfirmCallbackType)>;
        using OpenNewSceneDialogCallbackType = std::function<void(NewSceneOnConfirmCallbackType)>;
        using OpenNewProjectDialogCallbackType = std::function<void(NewProjectOnConfirmCallbackType)>;
        using OpenAssetBrowserCallbackType = std::function<void()>;

        OpenFileBrowserCallbackType openFileBrowser;
        OpenNewSceneDialogCallbackType openNewSceneDialog;
        OpenNewProjectDialogCallbackType openNewProjectDialog;
        OpenAssetBrowserCallbackType openAssetBrowser;
    };
}