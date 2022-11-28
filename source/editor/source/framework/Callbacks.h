#pragma once

#include "ProjectData.h"

#include "ncengine/config/Config.h"
#include "ncengine/ecs/Entity.h"

#include <functional>

namespace nc::editor
{
class DialogBase;

using GetProjectConfigCallbackType = std::function<const config::Config&()>;

struct ProjectCallbacks
{
    using CreateProjectCallbackType = std::function<void()>;
    using OpenProjectCallbackType = std::function<void()>;
    using GetProjectDataCallbackType = std::function<const ProjectData&()>;

    CreateProjectCallbackType createProject;
    OpenProjectCallbackType openProject;
    GetProjectDataCallbackType getProjectData;
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
    using SkyboxOnConfirmCallbackType = std::function<bool(const std::filesystem::path&)>;

    using OpenFileBrowserCallbackType = std::function<void(FileBrowserOnConfirmCallbackType)>;
    using OpenNewSceneDialogCallbackType = std::function<void(NewSceneOnConfirmCallbackType)>;
    using OpenNewProjectDialogCallbackType = std::function<void(NewProjectOnConfirmCallbackType)>;
    using OpenAssetBrowserCallbackType = std::function<void()>;
    using OpenConfigEditorCallbackType = std::function<void(const std::filesystem::path&)>;

    OpenFileBrowserCallbackType openFileBrowser;
    OpenNewSceneDialogCallbackType openNewSceneDialog;
    OpenNewProjectDialogCallbackType openNewProjectDialog;
    OpenAssetBrowserCallbackType openAssetBrowser;
    OpenConfigEditorCallbackType openConfigEditor;
};
} // namespace nc::editor