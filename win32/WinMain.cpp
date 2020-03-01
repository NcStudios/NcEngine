#include "NCWin32.h"
#include "Window.h"
#include "ProjectSettings.h"
#include "Engine.h"
#include "EditorManager.h"

#include "Graphics.h"

const char* PROJECT_SETTINGS_FILEPATH = "../project/Settings/projectsettings.txt";

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    nc::ProjectSettings::Load(PROJECT_SETTINGS_FILEPATH);

    nc::editor::EditorManager* editorManager;
    
    nc::Window window(instance);
    
    nc::internal::Engine* enginePtr = new nc::internal::Engine();


    editorManager = new nc::editor::EditorManager(window.GetHWND(), enginePtr->GetGraphics());

    window.BindEditorManager(editorManager);
    enginePtr->BindEditorManager(editorManager);
    
    enginePtr->MainLoop();

	return 0;
}