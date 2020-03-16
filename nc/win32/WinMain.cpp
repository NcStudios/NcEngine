#include "NCWin32.h"
#include "Window.h"
#include "ProjectSettings.h"
#include "Engine.h"
#include "EditorManager.h"

#include "Graphics.h"

const char* PROJECT_SETTINGS_FILEPATH = "project/Settings/projectsettings.txt";

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    nc::ProjectSettings::Load(PROJECT_SETTINGS_FILEPATH);
    
    nc::Window window(instance);
    
    nc::engine::Engine* enginePtr = new nc::engine::Engine(window.GetHWND());

    window.BindEditorManager(enginePtr->GetEditorManager());
    
    enginePtr->MainLoop();

	return 0;
}