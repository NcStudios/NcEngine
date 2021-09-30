#pragma once

#include "Entity.h"

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

        CreateProjectCallbackType createProjectCallback;
        OpenProjectCallbackType openProjectCallback;
    };

    struct SceneCallbacks
    {
        using NewSceneCallbackType = std::function<void()>;
        using SaveSceneCallbackType = std::function<void()>;
        using ChangeSceneCallbackType = std::function<void(const std::string&)>;
        using DeleteCurrentSceneCallbackType = std::function<void()>;

        NewSceneCallbackType newSceneCallback;
        SaveSceneCallbackType saveSceneCallback;
        ChangeSceneCallbackType changeSceneCallback;
        DeleteCurrentSceneCallbackType deleteCurrentSceneCallback;
    };

    struct EntityCallbacks
    {
        using ChangeTagCallbackType = std::function<void(Entity)>;

        ChangeTagCallbackType changeTagCallback;
    };

    struct UICallbacks
    {
        using AddDialogCallbackType = std::function<void(DialogBase*)>;
        using UpdateScenesCallbackType = std::function<void(std::vector<std::string>, int)>;
        using SetProjectNameCallbackType = std::function<void(std::string)>;

        AddDialogCallbackType addDialogCallback;
        UpdateScenesCallbackType updateScenesCallback;
        SetProjectNameCallbackType setProjectNameCallback;
    };

    struct DialogCallbacks
    {
        using FileBrowserOnConfirmCallbackType = std::function<bool(const std::filesystem::path&)>;
        using NewSceneOnConfirmCallbackType = std::function<bool(const std::string&)>;
        using OpenFileBrowserCallbackType = std::function<void(FileBrowserOnConfirmCallbackType)>;
        using OpenNewSceneDialogCallbackType = std::function<void(NewSceneOnConfirmCallbackType)>;

        OpenFileBrowserCallbackType openFileBrowser;
        OpenNewSceneDialogCallbackType openNewSceneDialog;
    };
}