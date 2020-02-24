#include <windows.h>
#include "include/Window.h"
#include "include/ProjectSettings.h"
#include "include/Engine.h"

const char* PROJECT_SETTINGS_FILEPATH = "../project/Settings/projectsettings.txt";

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    nc::ProjectSettings::Load(PROJECT_SETTINGS_FILEPATH);
    nc::Window window(instance);
    nc::internal::Engine* enginePtr = new nc::internal::Engine();
    enginePtr->MainLoop();

	return 0;
}