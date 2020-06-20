#include "NCWin32.h"
#include "Window.h"
#include "ProjectSettings.h"
#include "engine/Engine.h"
#include "debug/NcException.h"
#include "graphics/Graphics.h"

#ifdef NC_EDITOR_ENABLED
#include "utils/editor/EditorManager.h"
#endif

#include <iostream>

const char* PROJECT_SETTINGS_FILEPATH = "project/Settings/projectsettings.txt";

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    (void)prevInstance;
    (void)commandLine;
    (void)showCommand;

    nc::ProjectSettings::Load(PROJECT_SETTINGS_FILEPATH);
    
    nc::Window window(instance);
    
    nc::engine::Engine* enginePtr = new nc::engine::Engine();//window.GetHWND());

    #ifdef NC_EDITOR_ENABLED
    window.BindEditorManager(enginePtr->GetEditorManager());
    #endif

    try
    {
        enginePtr->MainLoop();
    }
    catch(const nc::NcException& e)
    {
        e.what();
        enginePtr->Exit();
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
    }

	return 0;
}