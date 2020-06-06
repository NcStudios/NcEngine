#include "NCWin32.h"
#include "Window.h"
#include "ProjectSettings.h"
#include "engine/Engine.h"

#include "graphics/Graphics.h"

#ifdef NC_DEBUG
#include "utils/editor/EditorManager.h"
#endif

const char* PROJECT_SETTINGS_FILEPATH = "project/Settings/projectsettings.txt";

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    (void)prevInstance;
    (void)commandLine;
    (void)showCommand;

    nc::ProjectSettings::Load(PROJECT_SETTINGS_FILEPATH);
    
    nc::Window window(instance);
    
    nc::engine::Engine* enginePtr = new nc::engine::Engine(window.GetHWND());

    #ifdef NC_DEBUG
    window.BindEditorManager(enginePtr->GetEditorManager());
    #endif

    enginePtr->MainLoop();

	return 0;
}